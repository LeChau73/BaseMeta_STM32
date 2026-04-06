# TEL Prober Project - Overview to Details Guide

**Mục đích**: Hiểu quy trình đo chip (Probe Process) và biết cách trace bug từ stage cụ thể xuống code/folder.

---

## 1️⃣ OVERVIEW - Quy Trình Chung (High-Level)

Đây là một **Wafer Probe System** - máy đo điểm tiếp xúc trên wafer chip.

**Main Flow - Quy trình chính**:
```
LOAD (Nạp wafer)
  ↓
ALIGNMENT (Căn chỉnh vị trí)
  ↓
TEST (Kiểm tra/đo chip)
  ↓
UNLOAD (Lấy wafer ra)
  ↓
LOT_END (Kết thúc Lot)
```

Ngoài ra có các bước phụ trợ:
- **BEFORE_TEST**: Chuẩn bị trước test (Mark, Heat, Contact, etc)
- **AFTER_TEST**: Xử lý sau test (In dấu, lưu dữ liệu, etc)

---

## 2️⃣ FOLDER STRUCTURE - Các Thư Mục Chính

| Folder | Mục Đích | Chi Tiết |
|--------|----------|----------|
| **appli/** | Application Logic - Logic ứng dụng | Chứa các hàm xử lý process stage |
| **loader/** | Loader Control | Điều khiển wafer loader |
| **loader12/** | Motor Control | Điều khiển motor (SIO-motor) |
| **target/** | Hardware/Target Control | Các điều khiển cụ thể cho hardware |
| **stage/** | Stage/Motion Control | Điều khiển stage đôi khi |
| **mmi/** | Man-Machine Interface | Giao diện người dùng, MMI |
| **GEM/** | GEM Protocol | Chuẩn giao thức GEM (FA Standard) |
| **HSMS/** | HSMS Protocol | Giao thức HSMS |
| **cmd/** | Command Files | Các file cấu hình lệnh cho platform |
| **h/** | Header Files | File .h dùng chung |
| **sys/** | System Level | Code system level, option setup |

---

## 3️⃣ MAIN FILES IN APPLI/ - Các File Chính

### **Master Control Files**:
- `APPLTaskMain.c` - Task chính của Application
- `masterTask.c` - Master task đồng bộ hóa các process
- `aApplMainSeq.c` - Main sequence logic

### **Process Stage Files** (Quan trọng nhất):

#### Lot Level:
- **aLotStart.c** - Xử lý **LOT_START**: Nạp batch lot
- **aLotEnd.c** - Xử lý **LOT_END**: Kết thúc lot

#### Wafer Level:
- **aBeforeTest.c** - **BEFORE_TEST**: Chuẩn bị trước test
  - Mark wafer
  - Heat (pre-heat)
  - Contact (tiếp xúc)
  - Polish needle (đánh bóng kim)

- **aAlignProc.c** - **ALIGNMENT**: Căn chỉnh vị trí wafer
  - Tìm mark trên wafer
  - Tính toán offset
  - Điều chỉnh position

- **aWafTest.c** - **TEST**: Kiểm tra chính
  - Điều khiển xét nghiệm
  - Ghi dữ liệu
  - Kiểm tra kết quả

- **aAfterTest.c** - **AFTER_TEST**: Xử lý sau test
  - In dấu PASS/FAIL
  - Lưu dữ liệu đo
  - Chuẩn bị cho wafer tiếp

#### Wafer Movement Files:
- **aLoadProcNew.c** - Load process (nạp từ loader vào stage)
- **aNextFreeDie.c** - Tìm die tiếp theo
- **aNextMarkDie.c** - Tìm die để mark
- **aSearchNext.c** - Tìm kiếm die tiếp theo

#### Output/Report Files:
- **GMSDataDiskWrite.c** - GMS data write
- **aRecvResult.c** - Nhận kết quả từ tester
- **aSendGpSrq.c** - Gửi lệnh GPIB

---

## 4️⃣ PROCESS STAGES - Các Giai Đoạn Chi Tiết

### **STAGE 1: LOT_START** (aLotStart.c)
```
1. Đọc setup file
2. Kiểm tra wafer map
3. Khởi tạo biến wafer 
4. Giao tiếp với tester (GEM/HSMS)
5. Khởi tạo dữ liệu
```
**Khi fix bug**: Nếu error ở LOG_START → Check aLotStart.c

---

### **STAGE 2: LOAD** (aLoadProcNew.c → loader12/ → loader/)
```
1. Lấy wafer từ cassette
2. Đặt wafer lên stage
3. Di chuyển loader về position initial
```
**Khi fix bug**: Nếu lỗi nạp wafer → Check aLoadProcNew.c + loader12/

---

### **STAGE 3: ALIGNMENT** (aAlignProc.c)
**Bước 1**: Tìm Mark trên wafer
- aAlignProc.c: Xử lý căn chỉnh
- Gửi lệnh điều khiển camera → target/
- Nhận ảnh từ camera → analyze

**Bước 2**: Tính toán offset
- So sánh vị trí mark thực tế vs dự kiến
- Tính delta XY
- Lưu vào offset table

**Bước 3**: Điều chỉnh vị trí wafer
- Gửi lệnh move stage
- Xác minh position mới

**Khi fix bug**: Nếu alignment sai → Check aAlignProc.c + stage/ + target/

---

### **STAGE 4: BEFORE_TEST** (aBeforeTest.c)
Nhiều sub-steps quan trọng:

**1. Mark**: In dấu trên chip trước đo
- Điều khiển marking head
- Kiểm tra chất lượng dấu

**2. Heat**: Sưởi chip trước đo
- Điều khiển heater temperature
- Chờ ổn định nhiệt độ

**3. Contact**: Tiếp xúc kim đo với pad
- Hạ kim chậm
- Kiểm tra contact force
- Xác minh điểm tiếp xúc

**4. Polish**: Đánh bóng kim (tuỳ chọn)
- Sử dụng wafer đánh bóng
- Tần số, áp lực, thời gian

**Khi fix bug**: Nếu lỗi trước test → Check aBeforeTest.c + aCFail.c (contact fail)

---

### **STAGE 5: TEST** (aWafTest.c)
```
1. Gửi điều kiện test đến tester (GPIB/TTL)
2. Tester chạy đo (Agilent, LTX, v.v.)
3. Nhận kết quả: PASS, FAIL, ERROR
4. Ghi dữ liệu đo vào file/GMS
```
**Khi fix bug**: Lỗi đo → Check aWafTest.c + aRecvResult.c + GEM/

---

### **STAGE 6: AFTER_TEST** (aAfterTest.c)
```
1. In dấu PASS/FAIL (nếu PASS mới in)
2. Lưu dữ liệu đo (GMSDataDiskWrite.c)
3. Tính toán yield, failbin
4. Chuẩn bị die/wafer tiếp theo
```
**Khi fix bug**: Lỗi sau test → Check aAfterTest.c

---

### **STAGE 7: UNLOAD** (aLoadProcNew.c khác mode)
```
1. Nâng kim, tháo wafer khỏi stage
2. Di chuyển wafer về cassette
3. Đặt wafer vào vị trí đúng
```

---

### **STAGE 8: LOT_END** (aLotEnd.c)
```
1. Tính tổng thống kê (PASS/FAIL/ERROR)
2. Gửi report cuối cùng tới tester
3. Lưu dữ liệu lot
4. Reset các biến cho lot tiếp theo
```
**Khi fix bug**: Nếu LOT_END fail → Check aLotEnd.c + aRecvResult.c

---

## 5️⃣ QUICK DEBUG MAPPING

**Bạn được giao fix bug ở stage nào → Trace ở đâu?**

| Stage | Primary File | Secondary Files | Folder |
|-------|-------------|-----------------|--------|
| LOT_START | aLotStart.c | aInitData.c | appli/ |
| LOAD | aLoadProcNew.c | aLoadTable.c | appli/ + loader12/ |
| ALIGNMENT | aAlignProc.c | - | appli/ + target/ |
| BEFORE_TEST | aBeforeTest.c | aCFail.c | appli/ |
| TEST | aWafTest.c | aRecvResult.c | appli/ + GEM/ |
| AFTER_TEST | aAfterTest.c | GMSDataDiskWrite.c | appli/ |
| UNLOAD | aLoadProcNew.c | aLoadTable.c | appli/ + loader12/ |
| LOT_END | aLotEnd.c | aRecvResult.c | appli/ |

---

## 6️⃣ KEY SUPPORT FILES

| File | Mục Đích |
|------|----------|
| **ppcTypes.h** | Type definition, constant |
| **tasksetup.h** | Task setup, semaphore |
| **bestcomm_api.h** | API control |
| **task_api/ | Task-related APIs |
| **mgt5200/sdma.h** | DMA, hardware control |
| **GEM/**, **HSMS/** | Protocol handling |

---

## 7️⃣ TASK & PROCESS MAPPING

**Main Tasks**:
- `MasterTask` - Master control, đồng bộ
- `APPLTask` - Application main task
- `portTask` - Port communication
- `lamptask` - Lamp control (status indicator)
- `ftpTask` - FTP file transfer
- `masterTask` - Low-level master control

Khi trace: Tìm `extern intl MasterTask;` để thấy nó được gọi từ đâu.

---

## 📌 SUMMARY

**High-Level** → Các stage: LOT_START → LOAD → ALIGNMENT → TEST → LOT_END
**Mid-Level** → Các file chính: aLotStart.c, aAlignProc.c, aWafTest.c, etc.
**Low-Level** → Sub-functions, hardware control, GEM/HSMS protocol

**Khi fix bug**:
1. Xác định stage có issue
2. Mở file chính của stage đó
3. Trace logic chi tiết
4. Check support files (protocol, hardware, etc.)
5. Sửa code → Test → Log verify

