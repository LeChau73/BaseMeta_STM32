Dưới đây là **bảng tóm tắt nhanh, dễ nhìn** về **các thanh ghi Debug/Fault System** trong **ARM Cortex-M (ví dụ: M3/M4/M7)** — giúp bạn tra cứu nhanh khi debug lỗi:

---
 fault status registers 
### ⚙️ **FAULT STATUS REGISTERS OVERVIEW**

| **Register**                                  | **Address (offset)** | **Purpose**                                        | **Important Bits / Meaning**                                                            |
| --------------------------------------------- | -------------------- | -------------------------------------------------- | --------------------------------------------------------------------------------------- |
| **CFSR** (Configurable Fault Status Register) | 0xE000ED28           | Tổng hợp lỗi **MemManage + BusFault + UsageFault** | [31:16] UFSR – UsageFault<br>[15:8] BFSR – BusFault<br>[7:0] MMFSR – MemManage          |
| **HFSR** (Hard Fault Status Register)         | 0xE000ED2C           | Báo lỗi nghiêm trọng chuyển thành HardFault        | Bit 30 = FORCED (HardFault do lỗi con)<br>Bit 1 = VECTTBL (Lỗi khi lấy vector)          |
| **DFSR** (Debug Fault Status Register)        | 0xE000ED30           | Báo nguyên nhân liên quan đến **Debug Event**      | Bit 4 = EXTERNAL<br>Bit 3 = VCATCH<br>Bit 2 = DWTTRAP<br>Bit 1 = BKPT<br>Bit 0 = HALTED |
| **MMFAR** (MemManage Fault Address Register)  | 0xE000ED34           | Địa chỉ gây lỗi **MemManage**                      | Chứa địa chỉ fault nếu MMARVALID=1 (CFSR[7])                                            |
| **BFAR** (BusFault Address Register)          | 0xE000ED38           | Địa chỉ gây lỗi **BusFault**                       | Chứa địa chỉ fault nếu 
BFAR sẽ chứa địa chỉ mà CPU đang cố gắng đọc/ghi không hợp lệ.

 (BFARVALID=1 (CFSR[15])                                           |
| **AFSR** (Auxiliary Fault Status Register)    | 0xE000ED3C           | Báo lỗi bổ sung (phụ thuộc thiết bị)               | Dùng cho thông tin lỗi ngoài core (ví dụ bus ngoại vi)                                  |

---

### 🧠 **CHI TIẾT CFSR (32 bit)**

| **Fault Type**              | **Bit(s)** | **Flag Name** | **Ý nghĩa**                                     |
| --------------------------- | ---------- | ------------- | ----------------------------------------------- |
| **MemManage Fault (MMFSR)** | 0          | IACCVIOL      | Vi phạm khi fetch lệnh từ vùng cấm              |
|                             | 1          | DACCVIOL      | Vi phạm khi truy cập dữ liệu                    |
|                             | 3          | MUNSTKERR     | Lỗi khi unstack                                 |
|                             | 4          | MSTKERR       | Lỗi khi stacking                                |
|                             | 5          | MLSPERR       | Lỗi lazy FP                                     |
|                             | 7          | MMARVALID     | Địa chỉ hợp lệ trong MMFAR                      |
| **Bus Fault (BFSR)**        | 8          | IBUSERR       | Lỗi khi fetch lệnh từ bus                       |
|                             | 9          | PRECISERR     | Lỗi bus chính xác (địa chỉ hợp lệ trong BFAR)   |
|                             | 10         | IMPRECISERR   | Lỗi bus không chính xác                         |
|                             | 11         | UNSTKERR      | Lỗi bus khi unstack                             |
|                             | 12         | STKERR        | Lỗi bus khi stacking                            |
|                             | 13         | LSPERR        | Lỗi lazy FP                                     |
|                             | 15         | BFARVALID     | Địa chỉ hợp lệ trong BFAR                       |
| **Usage Fault (UFSR)**      | 16         | UNDEFINSTR    | Lệnh không hợp lệ                               |
|                             | 17         | INVSTATE      | Trạng thái không hợp lệ (ví dụ, EXC_RETURN sai) |
|                             | 18         | INVPC         | PC không hợp lệ                                 |
|                             | 19         | NOCP          | Thiếu coprocessor                               |
|                             | 24         | UNALIGNED     | Truy cập không thẳng hàng                       |
|                             | 25         | DIVBYZERO     | Chia cho 0                                      |

---

### 🛠 **DEBUG REGISTERS (liên quan tới debug trap)**

| **Register**                                        | **Purpose**                     | **Main Bits**                                                                 |
| --------------------------------------------------- | ------------------------------- | ----------------------------------------------------------------------------- |
| **DHCSR** (Debug Halting Control & Status)          | Điều khiển/tình trạng debug     | Bit 0 C_DEBUGEN (cho phép debug)<br>Bit 1 C_HALT<br>Bit 16 S_HALT             |
| **DEMCR** (Debug Exception and Monitor Control)     | Bật/tắt các loại debug và fault | Bit 24 = TRCENA (Trace enable)<br>Bit 16 = VC_HARDERR<br>Bit 0 = VC_CORERESET |
| **AIRCR** (Application Interrupt and Reset Control) | Reset & priority grouping       | Bit 31 VECTKEY, Bit 2 SYSRESETREQ                                             |




| **Thanh ghi** | **Tên đầy đủ**                                     | **Chức năng chính**                                                     |
| ------------- | -------------------------------------------------- | ----------------------------------------------------------------------- |
| **APSR**      | Application Program Status Register                | Chứa **các cờ điều kiện** (N, Z, C, V, Q) — kết quả của phép toán       |
| **IPSR**      | Interrupt Program Status Register                  | Chứa **số hiệu của interrupt đang chạy** (ví dụ 3 = HardFault)          |
| **EPSR**      | Execution Program Status Register                  | Chứa **bit T** (Thumb state = 1), và thông tin về pipeline              |
| **xPSR**      | Tổng hợp của 3 thanh ghi trên (**x = “combined”**) | Khi đọc qua debug, nó thể hiện tất cả (N, Z, C, V, T, ISR number, v.v.) |
