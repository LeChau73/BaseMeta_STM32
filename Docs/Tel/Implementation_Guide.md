# Implementation Guide: Event Log Flush Timing Change

## Quick Answer to "Tại sao lại thế?"

**Bạn hỏi**: "Event được send ở ALIGNMENT, tại sao lại chỉ thấy ở LOT_END?"

**Trả lời**: 
```
┌─────────────────────────────────────────────────────────┐
│ SỰ KIỆN ĐƯỢC SINH RA ≠ SỰ KIỆN ĐƯỢC FLUSH/GỬI ĐI      │
└─────────────────────────────────────────────────────────┘

Timeline:
┌─────────────┬────────┬──────────┬────────────┬──────────┐
│ EventSend() │ Stored │ Buffered │ Buffered   │ FLUSH    │
│ at ALIGN    │ in RAM │ (WAFER)  │ (WAFER+)   │ at LOT   │
│ ✓Generated  │ ✓OK    │ Still OK │ Still OK   │ ✓Visible │
└─────────────┴────────┴──────────┴────────────┴──────────┘

Vì sao dùng buffer?
1. Performance: Gộp events, gửi 1 lần (hiệu quả hơn)
2. Atomic: Tất cả events của lot gửi cùng lúc (consistency)
3. Safety: Nếu fail mid-lot, event chưa gửi → có thể retry
```

**Solution**: Di chuyển FLUSH point từ LOT_END → WAFER_END

---

## Files to Modify & Exact Locations

### FILE 1: appli/aDurTestNew.c (WAFER_END)

**Search for**: `EVT_WAFEREND` or `WAFER_END` processing

```c
// LOCATION: Somewhere in aDurTestNew.c after wafer testing complete

Example pattern to find:
────────────────────────────────────────────────────────

EventLogMsgSend( EVT_WAFEREND, &WafData.waferID[0], &ContCountbyWafer, 0 );

// ^^ This is near WAFER_END!

────────────────────────────────────────────────────────
```

**What to ADD** (after WAFER_END event is sent):

```c
// NEW CODE TO ADD:
// ────────────────────────────────────────────────────

/* @XXXX your-ID-here Event log flush at WAFER_END instead of LOT_END */
if( ChkNewTestingEventLog() == TRUE )  // Check if new events exist
{
    // Flush event log now (at WAFER_END)
    apSaveNewTestingEventLog( outputBuffer );  
    // NOTE: outputBuffer depends on context - adjust as needed
    
    // IMPORTANT: Clear the buffer for next wafer if multiple wafers
    // (look for buffer clear function, e.g., ClearEventLogBuffer())
}
// ────────────────────────────────────────────────────
```

---

### FILE 2: appli/aLotEnd.c (LOT_END)

**Search for**: `apSaveNewTestingEventLog` or `ChkNewTestingEventLog`

**Current code looks like:**
```c
// LOCATION: In LOT_END processing section

if( ChkNewTestingEventLog() == TRUE )  // If new events exist
{
    dNum = FlWtSGT( fp, NEWTESTINGLOG_STR, ( unsl )&NewTestingEventLog );
    // This FLUSHES the buffer
}
```

**What to CHANGE**:

**Option A: Comment out (if only WAFER_END flush is needed)**
```c
// @XXXX your-ID-here Moved event log flush from LOT_END to WAFER_END
/*
if( ChkNewTestingEventLog() == TRUE )
{
    dNum = FlWtSGT( fp, NEWTESTINGLOG_STR, ( unsl )&NewTestingEventLog );
}
*/
```

**Option B: Add guard flag (if need both)**
```c
// @XXXX your-ID-here Only flush if not already flushed at WAFER_END
extern BOOL bEventLogFlushedAtWaferEnd;  // Declared in header

if( (ChkNewTestingEventLog() == TRUE) && (bEventLogFlushedAtWaferEnd == FALSE) )
{
    dNum = FlWtSGT( fp, NEWTESTINGLOG_STR, ( unsl )&NewTestingEventLog );
}
```

---

## Supporting Files to Review

### FILE 3: Check Header Files

**Search in**: `h/ppcTypes.h` or similar

Look for:
```c
// Define for event log
typedef struct {
    // Event log structure
    // ...
} tyNewTestingEventLog;

extern tyNewTestingEventLog NewTestingEventLog;  // Global buffer

// Function declarations
extern STATUS apSaveNewTestingEventLog( unsb * );
extern BOOL ChkNewTestingEventLog( VOID );
```

If you need a flag to prevent double-flush:
```c
// Add to header if not already exist:
extern BOOL bEventLogFlushedAtWaferEnd;
```

---

## Implementation Checklist

### Phase 1: Investigation
- [ ] Find `aDurTestNew.c` line with `EVT_WAFEREND`
- [ ] Find `aLotEnd.c` lines with `ChkNewTestingEventLog()` and `apSaveNewTestingEventLog()`
- [ ] Understand the buffer management (clear/reset functions)
- [ ] Check if multiple wafers per lot → need buffer clear

### Phase 2: Coding
- [ ] Add event log flush code to WAFER_END (aDurTestNew.c)
- [ ] Modify LOT_END code (aLotEnd.c) - comment out OR add guard
- [ ] Add any necessary guard flags or declarations
- [ ] Add comments with job ID/date

### Phase 3: Compilation & Testing
- [ ] Compile: `make clean; make`
- [ ] Fix any compilation errors
- [ ] Unit test: Run single wafer lot
- [ ] Integration test: Run multiple wafer lot
- [ ] Verify: Check output log shows event at WAFER_END

### Phase 4: Validation
- [ ] Verify no events missing
- [ ] Verify no events duplicated (LOT_END double-flush)
- [ ] Verify LOT_END still works
- [ ] Check with customer if acceptable
- [ ] Run full regression test if available

---

## Code Change Summary

```c
// ═══════════════════════════════════════════════════════════════
// FILE: appli/aDurTestNew.c
// ═══════════════════════════════════════════════════════════════

// LOCATION: Find EVT_WAFEREND
EventLogMsgSend( EVT_WAFEREND, &WafData.waferID[0], &ContCountbyWafer, 0 );

// ADD AFTER THIS LINE:
/* @2025/04/04 your-name Event log flush moved from LOT_END to WAFER_END */
if( ChkNewTestingEventLog() == TRUE )
{
    apSaveNewTestingEventLog( outMsg );  // Adjust parameter as needed
    // TODO: Clear buffer if needed for next wafer
}


// ═══════════════════════════════════════════════════════════════
// FILE: appli/aLotEnd.c
// ═══════════════════════════════════════════════════════════════

// LOCATION: Find ChkNewTestingEventLog() in LOT_END section
// CHANGE: Comment out or add guard condition

// BEFORE:
if( ChkNewTestingEventLog() == TRUE )
{
    dNum = FlWtSGT( fp, NEWTESTINGLOG_STR, ( unsl )&NewTestingEventLog );
}

// AFTER (Option A - Remove):
/* @2025/04/04 your-name Removed: Event log now flushed at WAFER_END
if( ChkNewTestingEventLog() == TRUE )
{
    dNum = FlWtSGT( fp, NEWTESTINGLOG_STR, ( unsl )&NewTestingEventLog );
}
*/

// AFTER (Option B - Guard):
/* @2025/04/04 your-name Only flush if not already at WAFER_END */
if( (ChkNewTestingEventLog() == TRUE) && (WaferEndEventLogFlushed == FALSE) )
{
    dNum = FlWtSGT( fp, NEWTESTINGLOG_STR, ( unsl )&NewTestingEventLog );
}
```

---

## Testing Strategy

### Test Case 1: Single Wafer, Single Lot
```
Input: 1 wafer, 1 lot
Expected: Event appears in log at WAFER_END
Command: Run prober → 1 wafer → check log
Verification: grep "EVT_NDLE_MIC_MAC_DIFF" logfile → should appear AFTER WAFER_END
```

### Test Case 2: Multiple Wafers, Single Lot
```
Input: 5 wafers, 1 lot
Expected: 
  - Wafer 1: Event 1 visible at WAFER1_END
  - Wafer 2: Event 2 visible at WAFER2_END
  - ...
  - Wafer 5: Event 5 visible at WAFER5_END
Verification: No event mixing, each wafer's events separate
```

### Test Case 3: Backward Compatibility
```
Input: Run full lot, verify LOT_END still processes correctly
Expected: LOT_END report still generated, no missing data
Verification: Customer can still see lot summary at LOT_END
```

---

## Possible Issues & Solutions

### Issue 1: Buffer Overflow (Multiple Wafers)
**Problem**: If don't clear buffer after WAFER_END → next wafer's events mix with previous

**Solution**: Add buffer clear function
```c
// In aDurTestNew.c after apSaveNewTestingEventLog():
ClearEventLogBuffer();  // Or similar function name
// OR manually reset:
memset( &NewTestingEventLog, 0, sizeof(NewTestingEventLog) );
```

### Issue 2: Missing Events at LOT_END
**Problem**: If removed flush at LOT_END → LOT_END may not have access to events

**Solution**: Use Option B (guard flag) instead of complete removal
```c
// Keep LOT_END code but guard it
if( (ChkNewTestingEventLog() == TRUE) && (AlreadyFlushedAtWaferEnd == FALSE) )
```

### Issue 3: Double Flush
**Problem**: Events flushed twice → data duplication

**Solution**: Add flag to track flush status
```c
// In global or static:
BOOL bEventLogFlushedAtWaferEnd = FALSE;

// At WAFER_END flush:
bEventLogFlushedAtWaferEnd = TRUE;

// At LOT_END check:
if( bEventLogFlushedAtWaferEnd == FALSE ) { /* flush */ }

// At LOT_END reset for next lot:
bEventLogFlushedAtWaferEnd = FALSE;
```

---

## Why This Design Works

```
Current Problem:
┌──────────────────────────────────────┐
│ Event buffer = 1 per LOT              │
│ Contains events from ALL wafers       │
│ Flushed only at LOT_END              │
│ → Long delay for customer visibility │
└──────────────────────────────────────┘

New Design:
┌──────────────────────────────────────┐
│ Event buffer = cleared per WAFER      │
│ Contains events from SINGLE wafer     │
│ Flushed at WAFER_END                 │
│ → Immediate visibility for customer   │
│ → Clear separation between wafers     │
└──────────────────────────────────────┘
```

---

## Commands to Find Exact Locations

```bash
# Find WAFER_END location
grep -n "EVT_WAFEREND\|WAFER_END" code/appli/aDurTestNew.c

# Find LOT_END flush location
grep -n "apSaveNewTestingEventLog\|ChkNewTestingEventLog" code/appli/aLotEnd.c

# Find buffer definition
grep -n "NewTestingEventLog" code/appli/aApFile.c

# Find all files using event log
grep -r "ChkNewTestingEventLog\|apSaveNewTestingEventLog" code/appli/

# Find buffer structure
grep -n "tyNewTestingEventLog" code/h/*.h
```

---

## When You're Done

1. Update this document with actual line numbers you found
2. Cross-review changes with team member
3. Run full test suite
4. Get approval from customer before deploying
5. Document the change in changelog/release notes

**Template for changelog**:
```
[2025/04/04] v1.x.x
- FEATURE: Move event log flush from LOT_END to WAFER_END
  * Event EVT_NDLE_MIC_MAC_DIFF now visible at WAFER_END (previously LOT_END)
  * Customer can see diagnostic events immediately after wafer completes
  * Files modified: aDurTestNew.c, aLotEnd.c
  * Ticket: [Customer Issue ID]
```

