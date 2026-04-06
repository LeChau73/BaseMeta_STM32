# TEL PROBER - CHEAT SHEET 📋

## PROCESS FLOW - Quy Trình Chính

```
LOT_START (aLotStart.c)
    ↓
LOAD (aLoadProcNew.c + loader12/)
    ↓
[FOR EACH WAFER]
  ├─ ALIGNMENT (aAlignProc.c)
  ├─ [FOR EACH DIE]
  │   ├─ MARK (aBeforeTest.c)
  │   ├─ HEAT (aBeforeTest.c)
  │   ├─ CONTACT (aBeforeTest.c + aCFail.c)
  │   ├─ [POLISH] (aBeforeTest.c - optional)
  │   ├─ TEST (aWafTest.c + aRecvResult.c)
  │   ├─ PRINT (aAfterTest.c)
  │   ├─ GMS_WRITE (GMSDataDiskWrite.c)
  │   └─ NEXT_DIE (aNextDieNew.c)
  └─ UNLOAD (aLoadProcNew.c)
    ↓
LOT_END (aLotEnd.c)
```

---

## QUICK LOOKUP TABLE

| Keywords in Error | Primary File | Key Function |
|-------------------|--------------|------------------|
| LOT_START | aLotStart.c | LotStartProc() |
| LOAD | aLoadProcNew.c | LoadWaferProc() |
| ALIGN | aAlignProc.c | AlignProc() |
| MARK | aBeforeTest.c | MarkChip() |
| HEAT | aBeforeTest.c | HeatChip() |
| CONTACT | aBeforeTest.c + aCFail.c | MakeContact() |
| POLISH | aBeforeTest.c | PolishNeedle() |
| TEST | aWafTest.c | RunTest() |
| RESULT | aRecvResult.c | ReceiveTestResult() |
| PRINT\|PASS | aAfterTest.c | PrintPASScode() |
| GMS\|DATA | GMSDataDiskWrite.c | SaveToGMSFile() |
| NEXT | aNextDieNew.c | PrepareNextDie() |
| UNLOAD | aLoadProcNew.c | UnloadWaferProc() |
| LOT_END | aLotEnd.c | LotEndProc() |

---

## FOLDER REFERENCE

```
appli/          ← Application process logic (main)
target/         ← Hardware control (stage, marker, camera)
loader12/       ← Motor/loader control
loader/         ← Loader mechanical
GEM/            ← GEM protocol (tester communication)
HSMS/           ← HSMS protocol (alternative)
mmi/            ← Man-Machine Interface
h/              ← Header files (definitions)
stage/          ← Stage/motion control
sys/            ← System level
```

---

## DEBUGGING WORKFLOW (5-STEP QUICK)

```
1. READ ERROR MESSAGE
   → Extract stage name (LOT_START, ALIGNMENT, TEST, etc.)

2. OPEN PRIMARY FILE
   → appli/<stage>.c
   → Exception: Use aCFail.c for CONTACT FAIL

3. SEARCH ERROR STRING
   → Find exact error message in code
   → Understand context (function, return code)

4. CHECK DEPENDENCIES
   → Follow function calls to support files
   → loader12/, target/, GEM/ if needed

5. FIX & VERIFY
   → Make minimal change
   → Recompile: make clean; make
   → Re-run test, check log
```

---

## COMMON ERROR PATTERNS

```
Error: "Connection timeout"
  → Check GEM/ or HSMS/ files, network setup

Error: "Wafer not detected"
  → Check loader12/, sensor in target/

Error: "Offset too large"
  → Check aAlignProc.c, camera calibration, stage accuracy

Error: "Contact fail"
  → Check aCFail.c, probe needle condition

Error: "Invalid result format"
  → Check aRecvResult.c, SEMI protocol definition

Error: "File write error"
  → Check GMSDataDiskWrite.c, disk space, permission

Error: "Crash / Hang"
  → Check masterTask.c, semaphore deadlock, tasksetup.h
```

---

## FILE INCLUSION MAP (Who calls whom)

```
APPLTaskMain.c (entry point)
    ├─ masterTask.c (master loop)
    │   └─ [process stage files]
    │
    ├─ aLotStart.c
    │   ├─ aInitData.c (init variables)
    │   ├─ GEM/*.c (send LOT_START message)
    │   └─ mmi/* (update UI)
    │
    ├─ aLoadProcNew.c
    │   ├─ aLoadTable.c (position table)
    │   ├─ loader12/* (motor control)
    │   └─ target/* (sensor, solenoid)
    │
    ├─ aAlignProc.c
    │   ├─ target/* (camera, stage)
    │   └─ [imaging lib]
    │
    ├─ aBeforeTest.c
    │   ├─ aCFail.c (contact fail handling)
    │   ├─ loader12/* (mark drive, gripper)
    │   └─ target/* (heater, probe needle, motor)
    │
    ├─ aWafTest.c
    │   ├─ aRecvResult.c (parse GPIB response)
    │   ├─ GEM/*.c (GPIB command)
    │   └─ HSMS/*.c (TTL alternative)
    │
    ├─ aAfterTest.c
    │   ├─ aNextDieNew.c (calculate next die)
    │   ├─ GMSDataDiskWrite.c (save data)
    │   └─ aApFile.c (file operations)
    │
    └─ aLotEnd.c
        ├─ aRecvResult.c (final message handling)
        ├─ GMSDataDiskWrite.c (save summary)
        └─ GEM/*.c (send LOT_END message)
```

---

## FUNCTION TRACING EXAMPLE

**Error**: "TEST: Tester returned SRQ=51H"

**Step 1-2**: Error = TEST stage → File = aWafTest.c

**Step 3**: Search "SRQ" in aWafTest.c
```
Found: if (result == SRQ_51H) { error_handler(); }
```

**Step 4a**: Trace result source
```
result ← aRecvResult.c: ReceiveTestResult()
```

**Step 4b**: Check aRecvResult.c
```
ReceiveTestResult() calls:
  ├─ ParseGPIB() [from GEM/]
  └─ ValidateChecksum()
```

**Step 4c**: Check GEM/ for SRQ_51H meaning
```
GEM/define.h: #define SRQ_51H 0x51  // Tester error condition
```

**Fix options**:
1. Tester command format wrong → Fix aWafTest.c SendCommand
2. Tester not ready → Add delay/retry in aWafTest.c
3. GPIB parsing wrong → Fix aRecvResult.c parser

---

## CRITICAL SECTIONS

These files are frequently involved in bugs:

1. **aBeforeTest.c** - Complex multi-step process (MARK/HEAT/CONTACT)
2. **aRecvResult.c** - Protocol parsing, data corruption
3. **aCFail.c** - Retry logic, edge cases
4. **aAlignProc.c** - Image processing, calibration
5. **aLotStart.c** - Initial state, configuration
6. **masterTask.c** - Timing, semaphore issues
7. **GEM/ + HSMS/** - Protocol compliance

---

## GIT IGNORE (Files NOT in git)

```
obj/          ← Compiled object files
stage/        ← Generated by build
cmd/          ← Platform-specific configs
LdMachineData*/  ← Test data
mmiMenu*/     ← UI assets
```

---

## BUILD COMMAND (Typical VxWorks)

```bash
cd code
make clean
make          # Rebuild all
make -j 4     # Parallel build (faster)
```

Check results in `obj/` directory (if using separate build)

---

## DEBUG LOG KEYWORDS (Search These)

```
"ERROR"        ← Error occurred
"FAIL"         ← Operation failed
"TIMEOUT"      ← Wait timeout
"NOT FOUND"    ← Expected data/device not found
"INVALID"      ← Bad data/format
"CRASH"        ← System crash
"ASSERT"       ← Assertion failed (debug mode)
```

---

## DO's AND DON'Ts

✅ DO:
- Start with error message → identify stage
- Open primary file for that stage
- Trace function calls sequentially
- Check function return values
- Verify data types match (int vs intl)
- Test single die first
- Verify no regression

❌ DON'T:
- Jump to fix without understanding flow
- Modify many files at once
- Change magic numbers without comment
- Ignore function return codes
- Assume typo without reading spec
- Skip testing after fix

---

## QUICK FILE SIZES OVERVIEW

| Category | Est. Lines | Complexity |
|----------|-----------|-----------|
| aLotStart.c | 500-1000 | Medium |
| aLoadProcNew.c | 1000-2000 | High |
| aAlignProc.c | 500-1000 | High |
| aBeforeTest.c | 2000-3000 | VERY HIGH |
| aWafTest.c | 500-1000 | Medium |
| aAfterTest.c | 500-1000 | Medium |
| aLotEnd.c | 300-500 | Low |
| aCFail.c | 500-1000 | High |
| aRecvResult.c | 300-500 | Medium |
| GMSDataDiskWrite.c | 300-500 | Low |

---

## KEY STRUCTS/DEFINES (In h/ppcTypes.h)

```c
// Typical defines you'll see
#define MAX_DIE_PER_WAFER    1000
#define MAX_WAFER_PER_LOT    500
#define LOT_NAME_LEN         32
#define MAX_OFFSET_ALLOW_MM  5.0

// Return codes
#define SUCCESS              0
#define ERROR_TIMEOUT        1
#define ERROR_INVALID_DATA   2
#define ERROR_HW_NOT_READY   3
// ... etc
```

---

## EMERGENCY: IF YOU'RE STUCK

```
1. Check if it's a HW issue
   → Test with manual command if possible

2. Re-read the stage description (this doc)
   → Maybe you misunderstood flow

3. Add debug print statements
   → printf() or log_message()
   → Recompile and watch output

4. Check git history (if git available)
   → See what changed recently
   → Git blame to find who modified

5. Ask for spec sheet if available
   → SEMI standards, protocol docs

6. Check similar code in other stages
   → Pattern matching for solution
```

---

## LAST RESORT: Search Strings

Use these grep commands to find what you need:

```bash
# Find where function is defined
grep -r "void FunctionName()" code/

# Find where function is called
grep -r "FunctionName(" code/

# Find all error handling
grep -r "ERROR_" code/appli/

# Find timeout handling
grep -r "TIMEOUT" code/appli/

# Find semaphore operations
grep -r "semTake\|semGive" code/

# Find return value checks
grep -r "if.*== ERROR\|if.*!= SUCCESS" code/

# Find TODO/FIXME
grep -r "TODO\|FIXME\|BUG\|HACK" code/appli/*.c
```

---

## CONTACT REFERENCE in OLD SESSIONS

If you have previous bug fixes for:
- CONTACT issues → Look at aCFail.c changes
- ALIGNMENT issues → Look at aAlignProc.c changes
- TEST issues → Look at aWafTest.c or aRecvResult.c changes

Pattern is usually: File with stage name prefix = primary handler

