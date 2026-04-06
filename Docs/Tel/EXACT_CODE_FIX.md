# EXACT CODE FIX - Customer Event Log Timing Issue

## Summary
Move event log flush from **LOT_END → WAFER_END** so customer sees log immediately after each wafer.

---

## STEP 1: Add Call to WAFER_END

### File: `appli/aDurTestNew.c`

**Search for:** `EventLogMsgSend( EVT_WAFEREND`

**Location:** Around line 3110

**Current code:**
```c
    /* Advanced Log Send ( 20008 Wafer End ) */
    ContCountbyWafer =  SramGet4( PrbContactCount ) - ContCountbyWafer;
    EventLogMsgSend( EVT_WAFEREND, &WafData.waferID[0], &ContCountbyWafer, 0 );
```

**ADD THIS AFTER (after EventLogMsgSend):**
```c
    /* @20260404 your-name Flush event log at WAFER_END for immediate customer visibility */
    apSaveNewTestingEventLog( NULL );  // Trigger disk write + FTP send
```

**Result after change:**
```c
    /* Advanced Log Send ( 20008 Wafer End ) */
    ContCountbyWafer =  SramGet4( PrbContactCount ) - ContCountbyWafer;
    EventLogMsgSend( EVT_WAFEREND, &WafData.waferID[0], &ContCountbyWafer, 0 );
    
    /* @20260404 your-name Flush event log at WAFER_END for immediate customer visibility */
    apSaveNewTestingEventLog( NULL );  // ← NEW LINE: Trigger disk write + FTP send
```

---

## STEP 2: Add Declaration (if needed)

### File: `appli/aDurTestNew.c`

**Search for:** `#include` statements at top of file (around line 500-700)

**Check if already declared:**
```c
extern STATUS  apSaveNewTestingEventLog( unsb * );
```

If **NOT present**, add to includes section:
```c
/* @20260404 your-name Event log flush function */
extern STATUS  apSaveNewTestingEventLog( unsb * );
```

---

## STEP 3: (OPTIONAL) Check LOT_END for Duplicate Flush

### File: `appli/aLotEnd.c`

**Search for:** Where event log is saved/flushed in LOT_END

**Check the current code** - if LOT_END also calls `apSaveNewTestingEventLog()`:

```c
// Look for pattern like:
if( ChkNewTestingEventLog() == TRUE )
{
    apSaveNewTestingEventLog( ... );  // Remove or comment out
}
```

**If found, ADD a GUARD to prevent double-flush:**

```c
// ORIGINAL (may cause double-flush):
if( ChkNewTestingEventLog() == TRUE )
{
    apSaveNewTestingEventLog( outMsg );
}

// CHANGED (prevent double at LOT_END if already at WAFER_END):
/* @20260404 your-name Already flushed at WAFER_END, skip LOT_END flush to avoid duplicate
if( ChkNewTestingEventLog() == TRUE )
{
    apSaveNewTestingEventLog( outMsg );
}
*/
```

---

## STEP 4: Compile & Test

```bash
cd code
make clean
make
```

If errors about `apSaveNewTestingEventLog` not found:
- Check that you added the declaration in Step 2
- Or the function may have different name/location

---

## How It Works (After Fix)

```
WAFER_END (aDurTestNew.c, line ~3110):
└─ EventLogMsgSend()           ← Events sent to RAM buffer
└─ apSaveNewTestingEventLog()   ← NEW! Set flag: "Write to disk NOW"
   └─ ChkNewTestingEventLog() becomes TRUE
   └─ aApFile.c detects this flag
   └─ aApFile.c calls FlWtSGT() → Write to /data/logs/NewTestingEventLog.csv
   └─ File now on disk!
   └─ ftpTask auto-detects new file
   └─ ftpTask transfers via FTP to customer
   └─ Customer SEES LOG immediately! ✅

LOT_END (if needed):
└─ Skip or guard the call (prevent double-flush)
```

---

## Complete Example - FINAL CODE

### File: `appli/aDurTestNew.c` (Around line 3105-3115)

**BEFORE:**
```c
    M5539AssistFlg = 0;

    stat = EndTest();

    /* Dry Air Function OFF */
    ApDryAirCtrl( 0 );

    /* Advanced Log Send ( 20008 Wafer End ) */
    ContCountbyWafer =  SramGet4( PrbContactCount ) - ContCountbyWafer;
    EventLogMsgSend( EVT_WAFEREND, &WafData.waferID[0], &ContCountbyWafer, 0 );

    SetFinsMachineStatus2( 0 );
```

**AFTER:**
```c
    M5539AssistFlg = 0;

    stat = EndTest();

    /* Dry Air Function OFF */
    ApDryAirCtrl( 0 );

    /* Advanced Log Send ( 20008 Wafer End ) */
    ContCountbyWafer =  SramGet4( PrbContactCount ) - ContCountbyWafer;
    EventLogMsgSend( EVT_WAFEREND, &WafData.waferID[0], &ContCountbyWafer, 0 );
    
    /* @20260404 your-name-here Flush event log at WAFER_END instead of LOT_END */
    /* This triggers: RAM → Disk → FTP → Customer sees immediately */
    apSaveNewTestingEventLog( NULL );

    SetFinsMachineStatus2( 0 );
```

---

## Verification Checklist

- [ ] Find line with `EventLogMsgSend( EVT_WAFEREND, ...` around line 3110 in aDurTestNew.c
- [ ] Add `apSaveNewTestingEventLog( NULL );` after it
- [ ] Add `extern STATUS apSaveNewTestingEventLog( unsb * );` if not already declared
- [ ] Compile: `make clean; make`
- [ ] No compilation errors
- [ ] Test: Run 1 wafer, check log appears at WAFER_END (not LOT_END)
- [ ] Test: Run 3+ wafers, verify no event mixing
- [ ] Verify: Customer can see event log immediately in their interface

---

## Troubleshooting

### Error: "apSaveNewTestingEventLog not found"
→ Add declaration to aDurTestNew.c top:
```c
extern STATUS  apSaveNewTestingEventLog( unsb * );
```

### Error: "parameter type mismatch"
→ Change to match signature:
```c
apSaveNewTestingEventLog( NULL );         // Simple version
// or
apSaveNewTestingEventLog( (unsb *)NULL ); // Explicit cast
```

### Event log still only at LOT_END
→ Check if LOT_END also calls it
→ In aLotEnd.c, comment out duplicate call
→ Recompile and test again

---

## Chain Reaction (Automatic)

```
1. apSaveNewTestingEventLog() called at WAFER_END ✅
   ↓
2. Sets flag in NewTestingEventLog struct
   ↓
3. aApFile.c detects ChkNewTestingEventLog() == TRUE ✅
   ↓
4. Writes to disk: FlWtSGT(fp, "NEWTESTING", ...) ✅
   ↓
5. ftpTask detects NEW FILE on disk ✅
   ↓
6. FTP transfers to customer automatically ✅
   ↓
7. Customer sees log on their PC immediately! ✅

→ NO additional FTP code needed!
→ NO additional file handling needed!
→ Everything automatic!
```

---

## Files Modified Summary

| File | Change | Lines |
|------|--------|-------|
| aDurTestNew.c | ADD `apSaveNewTestingEventLog()` | ~3112 |
| aDurTestNew.c | ADD declaration | ~750-800 |
| aLotEnd.c | OPTIONAL: Guard duplicate call | TBD |

---

## Total Changes: **2-3 lines of code!**

The beauty of this fix: You don't modify complex logic, just:
1. Call existing function at earlier point (WAFER_END instead of LOT_END)
2. System auto-handles: write to disk → FTP send → customer sees

