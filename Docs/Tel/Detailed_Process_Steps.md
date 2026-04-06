# DETAILED PROCESS STEPS - Chi Tiết Từng Bước

## 1. LOT_START - Khởi Tạo Lot

**File chính**: `appli/aLotStart.c`

### Sub-steps:

#### Step 1.1: Đọc cấu hình Lot
```c
// aLotStart.c
Function: ReadLotSetup()
  ├─ Đọc file setup từ đĩa
  ├─ Parse lot number, wafer count, test condition
  ├─ Load wafer map (vị trí chip trên wafer)
  ├─ Load test parameter từ recipe
  └─ Validate: Lot name != empty, wafer count > 0
```

#### Step 1.2: Kiểm tra dữ liệu
```c
// aLotStart.c
Function: ValidateLotData()
  ├─ Check lot name, ID
  ├─ Check wafer map file exists
  ├─ Check test recipe compatibility
  └─ Return: OK or ERROR_CODE
```

#### Step 1.3: Khởi tạo biến Global
```c
// aInitData.c hoặc aLotStart.c
Function: InitializeGlobalVar()
  ├─ Reset wafer counter = 0
  ├─ Reset die counter = 0
  ├─ Reset PASS/FAIL/ERROR count = 0
  ├─ Reset yield = 0%
  ├─ Clear GMS data buffer
  └─ Clear result array
```

#### Step 1.4: Giao tiếp với Tester (GEM/HSMS)
```c
// aLotStart.c + GEM/*.c
Function: ConnectWithTester()
  ├─ Gửi message "LOT_START" hoặc equivalent SEMI B protocol
  ├─ Wait response từ tester
  ├─ If response OK → continue
  ├─ If timeout/error → Report LOT_START fail → LOT_END
  └─ Store tester handle/state
```

#### Step 1.5: Setup MMI (Man-Machine Interface)
```c
// mmi/*
Function: UpdateMMI_StartLot()
  ├─ Display lot number
  ├─ Display wafer count
  ├─ Display test condition
  └─ Ready for start signal
```

**如果在这步出现错误**:
- Error reading setup → Check disk, file permissions
- Tester communication fail → Check GEM/ protocol files
- Invalid data → Check aInitData.c validation logic

---

## 2. LOAD - Nạp Wafer từ Loader

**File chính**: `appli/aLoadProcNew.c`

### Sub-steps:

#### Step 2.1: Kiểm tra Loader Status
```c
// aLoadProcNew.c + loader12/*
Function: CheckLoaderReady()
  ├─ Poll loader12/ motor status
  ├─ Verify loader position = "READY_TO_PICK"
  ├─ Check cassette presence
  ├─ If not ready → Send "GET_READY" command → Wait
  └─ Return: READY or TIMEOUT_ERROR
```

#### Step 2.2: Lấy Wafer từ Cassette
```c
// aLoadProcNew.c + loader12/*
Function: PickWaferFromCassette()
  ├─ Calculate cassette slot position (từ wafer counter)
  ├─ Send motor command to loader12/
  ├─ motor moves to slot
  ├─ Gripper closes (nắm wafer)
  ├─ Motor lifts wafer
  ├─ Wait completion signal
  └─ Verify: Sensor detect wafer picked up
```

#### Step 2.3: Di chuyển Wafer lên Stage
```c
// aLoadProcNew.c + loader12/* + target/*
Function: MoveWaferToStage()
  ├─ Send motor extend command
  ├─ Wafer moves over stage
  ├─ Wafer lowers onto stage center
  ├─ Verify stage surface sensor (wafer on contact)
  ├─ Release gripper
  ├─ Check: Wafer stable on stage
  └─ Motor returns to ready position
```

#### Step 2.4: Verify Wafer Position on Stage
```c
// aLoadProcNew.c + target/*
Function: VerifyWaferPosition()
  ├─ Check stage sensor: Wafer centered
  ├─ Optional: Use camera to verify position if available
  └─ If not centered → Report warning, may need realign
```

**如果在这步出现错误**:
- Loader timeout → Check loader12/, motor control
- Wafer sensor not detected → Check stage sensor, connector
- Position verify fail → May need calibration

---

## 3. ALIGNMENT - Căn chỉnh Wafer

**File chính**: `appli/aAlignProc.c`

### Sub-steps:

#### Step 3.1: Tìm Mark trên Wafer
```c
// aAlignProc.c + target/*
Function: FindWaferMark()
  ├─ Activate camera (từ target/)
  ├─ Move stage/camera to approximate mark area
  ├─ Capture image
  ├─ Image processing: Detect mark pattern
  │   ├─ Threshold image
  │   ├─ Find edge/contrast
  │   └─ Extract mark center position
  ├─ Repeat if mark not clear:
  │   ├─ Adjust stage slightly
  │   ├─ Re-capture
  │   └─ Re-process
  └─ Return: Mark_X, Mark_Y position (pixel or mm)
```

#### Step 3.2: Tính toán Offset
```c
// aAlignProc.c
Function: CalculateOffset()
  ├─ Read expected mark position (từ recipe/map)
  ├─ Compare với actual mark position từ bước 3.1
  ├─ Calculate:
  │   ├─ Offset_X = Actual_X - Expected_X
  │   └─ Offset_Y = Actual_Y - Expected_Y
  ├─ Validate: offset < Max_Allow (thường ±5mm)
  │   ├─ If OK → Continue
  │   └─ If NOK → Report ALIGNMENT_FAIL
  └─ Store offset in memory (dùng cho lần tính tiếp)
```

#### Step 3.3: Điều chỉnh Stage Position
```c
// aAlignProc.c + target/*
Function: AdjustStagePosition()
  ├─ Send move command: Move stage by (-Offset_X, -Offset_Y)
  ├─ Stage motor moves wafer
  ├─ Wait reach target position
  ├─ Verify: Current position = Target position
  │   ├─ If error > tolerance → Retry
  │   └─ If OK → Continue
  └─ Store new position for test
```

#### Step 3.4: Verify Position (Optional 2nd Pass)
```c
// aAlignProc.c
Function: VerifyAlignment()
  ├─ Optional: Capture image again at new position
  ├─ Check mark is now centered
  └─ If yes → Alignment complete
```

**如果在这步出现错误**:
- Mark not found → Check camera, wafer mark quality, lighting
- Offset too large → Wafer may be misloaded, check LOAD step
- Position verification fail → Stage motor issue, check calibration

---

## 4. BEFORE_TEST - Chuẩn Bị Trước Test

**File chính**: `appli/aBeforeTest.c`
**Sub-file**: `appli/aCFail.c` (contact fail handling)

### 4.1 MARK - In Dấu trên Chip

```c
// aBeforeTest.c
Function: PerformMarking()
  ├─ Read mark data từ wafer map (die location, lot ID, date)
  ├─ Activate marker/engraver
  ├─ Move stage to mark die position
  ├─ Execute mark pattern
  ├─ Wait mark complete (timeout check)
  ├─ Deactivate marker
  ├─ Verify mark quality (optional image check)
  └─ If fail → Increment mark_fail_count
```

### 4.2 HEAT - Sưởi Chip

```c
// aBeforeTest.c + target/*
Function: PreHeatChip()
  ├─ Read test temperature từ recipe
  ├─ Activate thermal stage/heater
  ├─ Ramp temperature to setpoint
  ├─ Monitor temperature (target ± tolerance)
  ├─ Wait settle time (usually 30-60 sec)
  ├─ Verify temperature stable
  └─ Ready for contact
```

### 4.3 CONTACT - Tiếp Xúc Kim Đo

**QUAN TRỌNG**: Đây là bước có nhiều sub-step nhất

```c
// aBeforeTest.c + aCFail.c + target/*
Function: MakeContact()
  ├─ Sub 4.3.1: Hạ Kim Chậm
  │   ├─ Activate needle drive (từ target/)
  │   ├─ Move needle down slowly (contact speed setting)
  │   ├─ Monitor approach (có thể dùng capacitive sensor)
  │   └─ Wait contact signal
  │
  ├─ Sub 4.3.2: Kiểm Tra Contact Force
  │   ├─ Read force sensor
  │   ├─ Compare với setpoint (e.g., 100-200g)
  │   ├─ If force OK → Continue
  │   └─ If force NOK → aCFail.c handle, retry hoặc fail
  │
  ├─ Sub 4.3.3: Verify Electrical Contact
  │   ├─ Send small test signal
  │   ├─ Check response
  │   ├─ If OK → contact established ✓
  │   └─ If fail → aCFail.c handle (needle clean? pad broken?)
  │
  └─ Sub 4.3.4: Contact Fail Handling (aCFail.c)
      ├─ IF contact fail:
      ├─   Retry contact (up to N times)
      ├─   Clean needle (if needle polish wafer available)
      ├─   Move to different pad (if multi-pad option)
      ├─   If all retry fail → Mark die as CONTACT_FAIL
      └─   Move to next die (skip this one)
```

### 4.4 POLISH (Optional) - Đánh Bóng Kim

```c
// aBeforeTest.c + target/*
Function: NeedlePolish()
  ├─ Check: Polish wafer available?
  ├─ IF no polish wafer in system → Skip
  ├─ Move stage to polish wafer
  ├─ Contact needle to polish wafer surface
  ├─ Perform polish motion (frequency, force, duration)
  ├─ Lift needle from polish wafer
  ├─ Move back to original die position
  └─ Resume contact (4.3)
```

**如果在这步出现错误**:
- Mark fail → Marker issue, check marker control
- Heat timeout → Heater not working, check temperature sensor
- Contact fail → Needle issue, pad broken, check aCFail.c logic
- Polish issue → Polish wafer missing or worn

---

## 5. TEST - Kiểm Tra Chính

**File chính**: `appli/aWafTest.c`

### Sub-steps:

#### Step 5.1: Chuẩn bị Test Condition
```c
// aWafTest.c
Function: SetupTestCondition()
  ├─ Load test program/pattern từ recipe
  ├─ Set test parameter (Vdd, frequency, timeout, etc.)
  ├─ Configure tester equipment
  └─ Ready state: Tester waiting for data
```

#### Step 5.2: Gửi Lệnh Test cho Tester
```c
// aWafTest.c + GEM/* hoặc HSMS/*
Function: SendTestCommand()
  ├─ Format GPIB/TTL command, include:
  │   ├─ Test program ID
  │   ├─ Pin list
  │   ├─ Pad coordinates (X, Y từ alignment)
  │   └─ Test condition (V, I, freq)
  ├─ Send command to tester (via GPIB/TTL/Ethernet)
  ├─ Wait acknowledgement (ACK)
  └─ If error → Retry or report TEST_SEND_FAIL
```

#### Step 5.3: Tester Chạy Test
```
// Tester (external equipment) runs autonomous
Tester performs:
  1. Apply power/signal according to test program
  2. Measure responses
  3. Compare với spec (PASS/FAIL/range check)
  4. Accumulate results
  5. Wait for all pin complete
  6. Format result data
```

#### Step 5.4: Nhận Kết Quả từ Tester
```c
// aWafTest.c + aRecvResult.c
Function: ReceiveTestResult()
  ├─ Wait result from tester (polling hoặc interrupt)
  ├─ Timeout: Nếu no result trong X sec → TEST_TIMEOUT
  ├─ Receive raw result data (byte array)
  ├─ Parse result format (SEMI standard):
  │   ├─ Status: PASS / FAIL / SKIP / ERROR
  │   ├─ Fail bits: Indicate which test/pin failed
  │   ├─ Measurement values (nếu có)
  │   └─ Timestamp
  ├─ Validate: Format valid? CRC correct?
  ├─ Store result in memory
  └─ Return: Result_Status
```

**Note**: aRecvResult.c là file riêng xử lý parse result

**如果在这步出现错误**:
- Test command not accepted → Check GPIB/TTL command format, GEM protocol
- Tester timeout → Tester not responding, check connection, tester status
- Invalid result format → Check aRecvResult.c parser, SEMI protocol spec
- CRC/checksum error → Data corruption in transmission

---

## 6. AFTER_TEST - Xử Lý Sau Test

**File chính**: `appli/aAfterTest.c`

### Sub-steps:

#### Step 6.1: Quyết Định PASS/FAIL
```c
// aAfterTest.c
Function: DeterminePASS_or_FAIL()
  ├─ Read test result từ bước 5.4
  ├─ If status = PASS:
  │   ├─ increment pass_count
  │   └─ Set die_result = PASS
  ├─ If status = FAIL:
  │   ├─ increment fail_count
  │   ├─ Extract fail_bits
  │   ├─ Map fail_bits → fail_bin (vd: BIN_01, BIN_02, etc.)
  │   └─ Set die_result = FAIL
  └─ If status = ERROR:
      ├─ increment error_count
      └─ Mark need retry or skip
```

#### Step 6.2: In Dấu PASS/FAIL (Nếu PASS)
```c
// aAfterTest.c + target/*
Function: PrintPASScode()
  ├─ If result = FAIL → Skip print (FAIL chips không in dấu)
  ├─ If result = PASS:
  │   ├─ Move stage marker to mark area
  │   ├─ Engrave "PASS" code (hoặc date/lot code)
  │   ├─ Verify mark quality
  │   └─ Increment pass_marked_count
```

#### Step 6.3: Ghi Data vào File GMS
```c
// aAfterTest.c + GMSDataDiskWrite.c
Function: SaveToGMSFile()
  ├─ Format GMS record (SEMI standard CSV-like):
  │   ├─ Lot ID, Wafer number
  │   ├─ Die coordinates (X, Y)
  │   ├─ Test result (PASS/FAIL)
  │   ├─ Fail bin (nếu FAIL)
  │   ├─ Measurement values
  │   └─ Timestamp
  ├─ Open GMS file để append
  ├─ Write record
  ├─ Close file
  └─ If I/O error → Log error, may retry
```

**Note**: GMSDataDiskWrite.c là chuyên trách ghi GMS

#### Step 6.4: Chuẩn bị Die/Wafer Tiếp Theo
```c
// aAfterTest.c + aNextDieNew.c hoặc aSearchNext.c
Function: PrepareNextDie()
  ├─ Increment die counter
  ├─ Read next die position từ wafer map:
  │   ├─ Search next untested die
  │   ├─ Calculate stage move (ΔX, ΔY)
  │   └─ Return: Next_Die_Position
  ├─ If more die available:
  │   ├─ Loop back to ALIGNMENT (step 3)
  │   └─ Process next die
  ├─ If no more die on wafer:
  │   ├─ Mark wafer complete
  │   └─ Proceed to UNLOAD (step 7)
```

**如果在这步出现错误**:
- Print fail → Marker issue
- File write fail → Disk full, permission, bad sector
- Next die calculation fail → Wafer map corrupted

---

## 7. UNLOAD - Lấy Wafer Ra

**File chính**: `appli/aLoadProcNew.c` (mode UNLOAD)

### Sub-steps:

#### Step 7.1: Nâng Kim Khỏi Wafer
```c
// aLoadProcNew.c + target/*
Function: LiftNeedle()
  ├─ Activate needle up command
  ├─ Needle retracts from pad
  ├─ Verify: Needle fully up (position sensor)
  └─ Ready for wafer removal
```

#### Step 7.2: Lấy Wafer Khỏi Stage
```c
// aLoadProcNew.c + loader12/*
Function: RemoveWaferFromStage()
  ├─ Send loader motor ready command
  ├─ Motor extend to stage
  ├─ Open gripper (không có wafer yet)
  ├─ Move stage up/lift wafer slightly
  ├─ Gripper close (nắm wafer)
  ├─ Verify: Sensor detect wafer gripped
  ├─ Motor withdraw wafer from stage
  └─ Verify: Stage sensor no wafer
```

#### Step 7.3: Đặt Wafer Về Cassette
```c
// aLoadProcNew.c + loader12/*
Function: PlaceWaferToCassette()
  ├─ Calculate return slot (từ wafer counter)
  ├─ Move wafer to cassette slot
  ├─ Lower wafer into slot
  ├─ Release gripper
  ├─ Verify: Wafer in cassette (sensor)
  ├─ Motor returns to standby
  └─ Cassette now has wafer back
```

**如果在这步出现错误**:
- Wafer drop → Motor issue, gripper weak, check loader12/
- Wrong slot → Counter/calculation error in sub 7.3
- Wafer not sensed → Sensor failure, check stage/cassette sensors

---

## 8. LOT_END - Kết Thúc Lot

**File chính**: `appli/aLotEnd.c`

### Sub-steps:

#### Step 8.1: Tính Tổng Thống Kê
```c
// aLotEnd.c
Function: CalculateLotStatistics()
  ├─ Total wafer count (từ config)
  ├─ Total die tested = Σ all die
  ├─ Total PASS = pass_count
  ├─ Total FAIL = fail_count
  ├─ Calculate Yield = PASS / (PASS + FAIL) * 100%
  ├─ Fail bin distribution:
  │   ├─ BIN_01: N chips
  │   ├─ BIN_02: N chips
  │   └─ ...
  └─ Generate summary report
```

#### Step 8.2: Gửi Report Tới Tester
```c
// aLotEnd.c + GEM/* hoặc HSMS/*
Function: SendLotEndReport()
  ├─ Format report message:
  │   ├─ Lot ID
  │   ├─ Status: COMPLETE / ERROR
  │   ├─ Yield
  │   ├─ PASS/FAIL/ERROR counts
  │   ├─ Fail bin list
  │   └─ Timestamp
  ├─ Send to tester (GPIB/TTL/Network)
  ├─ Wait acknowledgement
  └─ If error → Retry hoặc log warning
```

#### Step 8.3: Lưu GMS Summary (Lot-level)
```c
// aLotEnd.c + GMSDataDiskWrite.c
Function: SaveLotSummary()
  ├─ Write GMS lot summary:
  │   ├─ [LOT_SUMMARY]
  │   ├─ Lot ID, Date, Time
  │   ├─ Equipment ID
  │   ├─ Yield
  │   ├─ Total die, PASS, FAIL, ERROR
  │   └─ Fail bin summary
  ├─ Close GMS file
  └─ Optional: Archive GMS file
```

#### Step 8.4: Reset Config Cho Lot Tiếp Theo
```c
// aLotEnd.c
Function: ResetForNextLot()
  ├─ Clear all lot-level counters
  ├─ Clear wafer map buffer
  ├─ Clear test parameter cache
  ├─ Close GMS file
  ├─ Reset status flags
  ├─ Ready state: IDLE → Wait LOT_START
  └─ Update MMI: "Ready for next lot"
```

**如果在这步出现错误**:
- Tester report send fail → Check GEM protocol, tester connection
- GMS file write fail → Disk issue, check GMSDataDiskWrite.c
- Statistics calculation error → Overflow, counter issue, check data type

---

## SUMMARY TABLE - Mapping Stage → Function → File

| Stage | Main Function | File | Sub-functions |
|-------|---------------|------|----------------|
| LOT_START | LotStartProc() | aLotStart.c | ReadLotSetup, ValidateData, InitGlobal, ConnectTester |
| LOAD | LoadWaferProc() | aLoadProcNew.c | CheckLoader, PickWafer, MoveToStage, VerifyPosition |
| ALIGNMENT | AlignProc() | aAlignProc.c | FindMark, CalcOffset, AdjustPosition, VerifyAlign |
| MARK | MarkChip() | aBeforeTest.c | ActivateMarker, Execute, VerifyQuality |
| HEAT | HeatChip() | aBeforeTest.c | RampTemp, Monitor, SettleWait |
| CONTACT | MakeContact() | aBeforeTest.c, aCFail.c | LowerNeedle, ForceCheck, VerifyElec, FailHandle |
| POLISH | PolishNeedle() | aBeforeTest.c | MoveToPolish, Execute, MoveBack |
| TEST | RunTest() | aWafTest.c, aRecvResult.c | SendCommand, WaitResult, ParseResult |
| PRINT | PrintPASS() | aAfterTest.c | CheckResult, MoveMarker, Engrave |
| GMS_WRITE | SaveGMS() | GMSDataDiskWrite.c, aAfterTest.c | FormatRecord, OpenFile, Write, Close |
| NEXT_DIE | PrepareNext() | aAfterTest.c, aNextDieNew.c | IncrementCounter, CalcPosition, LoopCheck |
| UNLOAD | UnloadWaferProc() | aLoadProcNew.c | LiftNeedle, RemoveWafer, PlaceInCassette |
| LOT_END | LotEndProc() | aLotEnd.c | CalcStats, SendReport, SaveSummary, Reset |

