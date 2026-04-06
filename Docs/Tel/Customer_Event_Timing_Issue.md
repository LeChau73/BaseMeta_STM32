# CUSTOMER JOB: Event Log Timing Issue

## Problem Statement
```
Customer yêu cầu: Đổi thời điểm nhìn thấy log của event "EVT_NDLE_MIC_MAC_DIFF 20051"
Từ: LOT_END (hiện tại, khi kết thúc LOT)
Sang: WAFER_END (mới, khi kết thúc WAFER)

Vấn đề mà bạn phát hiện:
- Event này được send khi "ProberAlignment" (trong ALIGNMENT stage)
- Nhưng nó chỉ visible cho khách khi LOT_END
- Vậy tại sao sự kiện lại chỉ hiển thị ở LOT_END mà không ở WAFER_END?
```

---

## Root Cause Analysis

### Architecture Hiện Tại:

```
Stage => Event Generated => Buffered in Memory => Flushed at LOT_END => Visible to Customer

Timeline:
┌─────────────┬────────────┬──────────────┬──────────────┬─────────────┬──────────┐
│ LOT_START   │ LOAD       │ ALIGNMENT    │ BEFORE_TEST  │ TEST        │ ... (nhiều die)
│             │            │ (Event sent) │              │             │
└─────────────┴────────────┴──────────────┴──────────────┴─────────────┴──────────┘
                                ↓
                        Event stored in buffer
                        (NewTestingEventLog)
                                ↓
                        ┌─────────────────────────────────────────┐
                        │ WAFER_END (NOT flushed yet)             │
                        │ Event vẫn trong buffer, khách không thấy│
                        └─────────────────────────────────────────┘
                                ↓
                        ┌─────────────────────────────────────────┐
                        │ LOT_END (flushed NOW!)                  │
                        │ apSaveNewTestingEventLog() được gọi     │
                        │ → Buffer được flush                      │
                        │ → Event log visible cho khách            │
                        └─────────────────────────────────────────┘
```

### Key Functions Involved:

```c
// 1. Event được generate/sent khi Alignment
// trong appli/aAlignProc.c hoặc related files:
EventLogMsgSend( EVT_NDLE_MIC_MAC_DIFF, data, 0 );

// 2. Event được store trong global buffer:
extern tyNewTestingEventLog NewTestingEventLog;

// 3. Event log được save/flush ở LOT_END:
// trong appli/aLotEnd.c:
apSaveNewTestingEventLog( outputBuffer );
```

---

## Where Events Are Logged

### Các Điểm Logging Quan Trọng:

| Stage | File | Function | Event |
|-------|------|----------|-------|
| LOT_START | aLotStart.c | - | EVT_LOTSTART |
| ALIGNMENT | aAlignProc.c (?) | - | EVT_NDLE_MIC_MAC_DIFF |
| WAFER_START | aBeforeTest.c | - | EVT_WAFERSTART |
| BEFORE_TEST | aBeforeTest.c | - | EVT_... |
| TEST | aEdTestData.c | - | EVT_TESTSTART, EVT_TESTEND |
| WAFER_END | aDurTestNew.c | - | EVT_WAFEREND |
| CONTACT_FAIL | aCFail.c | - | EVT_CF_START, EVT_CF_END |

Tất cả những events này được **buffer** vào `NewTestingEventLog` chứ **KHÔNG** được flush ngay.

---

## Where Events Are Flushed/Saved

### Current Behavior (LOT_END):

**File: aLotEnd.c**
```c
// Somewhere in LOT_END flow:
if ( ChkNewTestingEventLog() == TRUE )
{
    // Event log exists, save it
    apSaveNewTestingEventLog( outputBuffer );
    
    // Now customer can see the log
    // Send to tester/MMI/Save to file
}
```

**File: aApFile.c** (supporting functions)
```c
// Check if new testing event log exists
if( ChkNewTestingEventLog() == TRUE )
{
    dNum = FlWtSGT( fp, NEWTESTINGLOG_STR, &NewTestingEventLog );
    // Write event log to GMS/file
}
```

---

## What You Need to Change

### Solution: Move Flush Point from LOT_END to WAFER_END

### Step 1: Find Where WAFER_END is Handled
```
Primary file: aDurTestNew.c (contains WAFER_END logic)

Look for:
- EventLogMsgSend( EVT_WAFEREND, ... )
- End of wafer processing
- WAFER_END logic block
```

### Step 2: Identify the Flush Points

**Current (LOT_END)**:
```
Location: appli/aLotEnd.c

SearchFor:
- if( ChkNewTestingEventLog() == TRUE )
- apSaveNewTestingEventLog()
- FlWtSGT( fp, NEWTESTINGLOG_STR, ... )
```

**New (WAFER_END)**:
```
Location: appli/aDurTestNew.c

Add similar code after WAFER_END processing
```

### Step 3: Changes Needed

#### In aDurTestNew.c (WAFER_END section):
```c
// Add after wafer processing complete
if( ChkNewTestingEventLog() == TRUE )  // Check if events exist
{
    // OPTION A: Save immediately to file
    apSaveNewTestingEventLog( outputBuffer );
    
    // OPTION B: Or just mark as "ready to send"
    // and let MMI/host retrieve it
}
```

#### In aLotEnd.c (LOT_END section):
```c
// REMOVE or COMMENT OUT the old flush
// if( ChkNewTestingEventLog() == TRUE )
// {
//     apSaveNewTestingEventLog( outputBuffer );
// }

// Alternative: Add a check to prevent double-flushing
if( IsEventLogAlreadyFlushed() == FALSE )  // Only flush if not already flushed at WAFER_END
{
    apSaveNewTestingEventLog( outputBuffer );
}
```

---

## Files to Modify

### PRIMARY Files (必须修改):

1. **appli/aDurTestNew.c** (WAFER_END)
   - Find the WAFER_END processing section
   - Add event log flush code
   - Add any necessary checks to avoid double-flush

2. **appli/aLotEnd.c** (LOT_END)
   - Find the event log flush code
   - Modify to NOT flush if already flushed at WAFER_END
   - Or completely remove if flushing at WAFER_END is sufficient

### SECONDARY Files (可能需要修改):

3. **appli/aApFile.c** (GMS file writing)
   - May need to adjust when GMS data is written
   - Especially if using FlWtSGT() for event log

4. **appli/aApMmiCom.c** (MMI communication)
   - May need to adjust when event log is sent to MMI

5. **h/ppcTypes.h** or related header
   - May need new flag/state to track "EventLogFlushed"

---

## Important Considerations

### ⚠️ Timing Issues:

1. **Loop Structure**:
   - Does aDurTestNew.c process one wafer at a time?
   - Or does it loop through multiple wafers?
   - If multiple wafers per LOT → Need to flush event log for EACH wafer

2. **Event Accumulation**:
   ```
   Timeline:
   WAFER1: Events logged → Flush at WAFER1_END ✓
   WAFER2: Events logged → Flush at WAFER2_END ✓
   ...
   WAFERN: Events logged → Flush at WAFERN_END ✓
   ```
   If not careful → Events from WAFER2 may overwrite WAFER1's events!

3. **Buffer Management**:
   - After flushing at WAFER_END, need to **CLEAR** the buffer
   - Otherwise next wafer's events will mix with previous

### 📋 Checklist Before Modifying:

- [ ] Find exact location of WAFER_END in aDurTestNew.c
- [ ] Find exact location of event log flush in aLotEnd.c
- [ ] Check if NewTestingEventLog has clear/reset function
- [ ] Verify buffer size is enough for single wafer
- [ ] Check if multiple flushes cause issues
- [ ] Test with 1 wafer, then multiple wafers
- [ ] Verify events appear at WAFER_END in log output

---

## Why Event Was Collected at ALIGNMENT But Shown at LOT_END

### Architecture Reason:

The system uses **"Event Logging Buffer"** pattern:

```
┌─────────────────────────────────────────┐
│  Event Logging Buffer (Ring/Queue)      │
│  - Collects events from all stages      │
│  - Not immediately sent to customer     │
│  - Flushed at pre-defined checkpoint    │
└─────────────────────────────────────────┘

Reasons for this design:
1. Performance: Fewer I/O operations (batch send at LOT_END)
2. Consistency: All events from lot saved together
3. Complex: Events may be modified after generation
4. Recovery: If something fails mid-lot, events not partially sent
```

### Events Are Generated Early But Flushed Late:

```
Event Generation Timeline:
│
├─ Alignment: EVT_NDLE_MIC_MAC_DIFF generated
│            ↓ stored in NewTestingEventLog[]
│
├─ Before Test: Other events...
│            ↓ appended to buffer
│
├─ Test: More events...
│            ↓ appended to buffer
│
├─ After Test: More events...
│            ↓ appended to buffer
│
└─ LOT_END: FLUSH ALL AT ONCE
            ↓ apSaveNewTestingEventLog()
            ↓ Now customer sees everything!
```

**Customer wanted**: Flush earlier (at WAFER_END instead of LOT_END)

---

## How to Find The Exact Code Locations

### Command to Search:

```bash
# Find where EVT_NDLE_MIC_MAC_DIFF is used
grep -r "EVT_NDLE_MIC_MAC_DIFF" code/appli/

# Find all event flush points
grep -r "apSaveNewTestingEventLog\|ChkNewTestingEventLog" code/appli/

# Find WAFER_END handling
grep -r "EVT_WAFEREND\|WAFER_END" code/appli/

# Find buffer definition
grep -r "NewTestingEventLog" code/appli/

# Find buffer clear function
grep -r "ClearEventLog\|ResetEventLog" code/appli/
```

---

## Next Steps For You

### 1. **Understand the Current Flow**:
   - [ ] Read aDurTestNew.c → Find WAFER_END section
   - [ ] Read aLotEnd.c → Find event log flush code
   - [ ] Trace how NewTestingEventLog is defined/used

### 2. **Implement Changes**:
   - [ ] Add flush code to WAFER_END
   - [ ] Modify LOT_END to prevent double-flush or conflict
   - [ ] Add buffer clear after flush (if needed)

### 3. **Test**:
   - [ ] Compile and verify no errors
   - [ ] Test with single wafer lot
   - [ ] Test with multiple wafer lot
   - [ ] Check event log appears at WAFER_END in output/log file
   - [ ] Check LOT_END still works correctly

### 4. **Documentation**:
   - [ ] Document where changes were made
   - [ ] Update comments explaining the reason
   - [ ] Note any side-effects or dependencies

---

## Summary Visual:

```
BEFORE (Current):
ALIGN → Event sent → Buffer → WAFER_END → LOT_END → Customer sees event ✓

AFTER (Customer wants):
ALIGN → Event sent → Buffer → WAFER_END → Customer sees event ✓
                                      └→ LOT_END → All events already flushed
```

