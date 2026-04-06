# Quick Debug Reference - TEL Prober

## Ví dụ Thực Tế: Trace Bug ở LOT_END

### Tình Huống:
```
LOG ERROR: "LOT_END: Failed to send report to tester"
Stage: LOT_END
```

### Tracing Steps:

**Step 1: Xác định Stage**
- Error message có "LOT_END" → Stage là **LOT_END**

**Step 2: Mở File Chính**
```
code/appli/aLotEnd.c  ← File chính xử lý LOT_END
```

**Step 3: Trace Error Message**
Tìm trong aLotEnd.c:
```c
// Tìm string "Failed to send report"
// → Locate hàm hiện error này
// → Xem điều kiện trước error
```

**Step 4: Check Root Cause**
Có thể là:
1. **GEM Protocol error** → Check `GEM/`
2. **Task communication error** → Check `tasksetup.h`, semaphore
3. **Data format error** → Check `aRecvResult.c`
4. **Config error** → Check `h/ppcTypes.h`, constant

**Step 5: Fix & Verify**
```
// Sửa trong aLotEnd.c
// Recompile
// Run test, check log output
```

---

## Ví Dụ 2: Alignment sai vị trí

### Tình Huống:
```
LOG: "Alignment offset too large: X=5mm, Y=3mm"
Stage: ALIGNMENT
```

### Tracing:
1. File chính: `appli/aAlignProc.c`
2. Tìm "offset" trong file này
3. Trace hàm:
   - `CalcOffset()` - tính toán offset
   - `FindMark()` - tìm mark trên camera
   - `AdjustPosition()` - điều chỉnh
4. Sub-files:
   - `target/` - camera control, stage move
   - `h/mgt5200/sdma.h` - hardware control
5. Fix: Kiểm tra calibration, camera threshold, stage accuracy

---

## Ví Dụ 3: Contact FAIL

### Tình Huống:
```
LOG: "Contact check failed at die (10, 15)"
Stage: BEFORE_TEST → Contact sub-step
```

### Tracing:
1. File chính: `appli/aBeforeTest.c` (BEFORE_TEST)
2. Tìm "contact" hoặc "force" trong aBeforeTest.c
3. Sub-file: `appli/aCFail.c` (Contact FAIL handling)
4. Check:
   - Contact force threshold
   - Needle height
   - Wafer surface condition
5. Fix: Điều chỉnh parameter, probe needle check

---

## Ví Dụ 4: TEST lỗi result

### Tình Huống:
```
LOG: "TEST: Tester returned invalid result format"
Stage: TEST
```

### Tracing:
1. File chính: `appli/aWafTest.c`
2. Trace GPIB/TTL command send:
   - `SendTestCommand()`
3. Trace result receive:
   - `aRecvResult.c` ← **QUAN TRỌNG**
4. Sub-files:
   - `GEM/` - GEM protocol format
   - `h/ppcTypes.h` - result data structure
5. Fix: Parse result format, verify protocol

---

## Ví Dụ 5: GMS Data file corrupt

### Tình Huống:
```
LOG: "GMSDataDiskWrite: File write error"
Stage: AFTER_TEST → Data write
```

### Tracing:
1. File chính: `appli/GMSDataDiskWrite.c`
2. Check:
   - File path validity
   - Disk space
   - Directory permission
   - Data buffer overflow
3. Sub-check:
   - `appli/aApDataRead.c` - data read/write
   - `appli/aApFile.c` - file operation
4. Fix: Check mount point, file descriptor, buffer size

---

## KEY FILES BY FEATURE

| Feature | Primary File | Support Files |
|---------|-------------|----------------|
| Lot management | aLotStart.c, aLotEnd.c | aInitData.c |
| Wafer loading | aLoadProcNew.c | aLoadTable.c, loader12/ |
| Alignment/Mark finding | aAlignProc.c | target/, mgt5200/ |
| Before test (contact/mark/heat) | aBeforeTest.c | aCFail.c, loader12/ |
| Test control | aWafTest.c | aRecvResult.c, GEM/, HSMS/ |
| After test (print/save) | aAfterTest.c | GMSDataDiskWrite.c |
| Probe control | probLib/ | target/ |
| Network/GEM | GEM/ | HSMS/, h/ |
| Task/Semaphore | masterTask.c | tasksetup.h |
| Hardware control | target/ | mgt5200/ |

---

## SUPER QUICK: When You Have Error Message

```
IF error contains:
  "LOT_START"     → aLotStart.c
  "LOAD"          → aLoadProcNew.c + loader12/
  "ALIGN"         → aAlignProc.c + target/
  "MARK"          → aBeforeTest.c + loader12/
  "CONTACT"       → aBeforeTest.c + aCFail.c
  "HEAT"          → aBeforeTest.c + target/
  "TEST"          → aWafTest.c + GEM/
  "RESULT"        → aRecvResult.c
  "PRINT"         → aAfterTest.c
  "GMS" or "DATA" → GMSDataDiskWrite.c + appli/aApFile.c
  "LOT_END"       → aLotEnd.c
  "GPIB" or "TTL" → GEM/ + target/
  "CONTACT FAIL"  → aCFail.c
  "CRASH"         → masterTask.c + tasksetup.h
ENDIF
```

---

## FILE DEPENDENCY

```
APPLTaskMain.c (main entry)
    ├─ masterTask.c (master control)
    ├─ aApplMainSeq.c (main sequence)
    └─ [process stage files]
            ├─ aLotStart.c
            ├─ aLoadProcNew.c
            ├─ aAlignProc.c
            ├─ aBeforeTest.c
            ├─ aWafTest.c
            ├─ aAfterTest.c
            ├─ aLotEnd.c
            │
            └─ [support]
                    ├─ aRecvResult.c
                    ├─ aInitData.c
                    ├─ loader12/
                    ├─ target/
                    ├─ GEM/
                    └─ HSMS/
```

---

## COMMON BUG PATTERNS

| Bug Pattern | Likely Cause | Check File |
|-------------|-------------|-----------|
| Hangs during wait | Semaphore deadlock | masterTask.c, tasksetup.h |
| Return value check fail | Type mismatch, pointer NULL | h/ppcTypes.h |
| Data corruption | Buffer overflow, pointer arithmetic | appli/*.c (each file) |
| Protocol fail | Message format, sequence | GEM/, HSMS/ |
| Hardware not responding | Timeout, command not sent | target/, mgt5200/ |
| Partial execution | Return early / exception | Find early return in function |

---

## HOW TO VERIFY FIX

1. **Read code before fix**: Understand logic
2. **Make minimal change**: Only fix the root cause
3. **Recompile**: `make clean; make`
4. **Run test**: Execute scenario again
5. **Check log**: Look for error message gone / new behavior correct
6. **Verify no regression**: Run full test suite if time permits

