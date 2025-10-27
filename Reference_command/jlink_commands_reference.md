# J-Link Commands Reference

## 1️⃣ Device Control
| Command | Description |
|---------|-------------|
| device <name> | Chọn MCU để kết nối. |
| if SWD / if JTAG | Chọn giao tiếp debug (SWD hoặc JTAG). |
| speed <khz> | Đặt tốc độ giao tiếp. |
| reset / r | Reset MCU. |
| go / g | Cho MCU chạy từ PC hoặc từ địa chỉ hiện tại. |
| halt / h | Dừng MCU để debug. |
| step | Thực thi 1 instruction. |
| reg | Hiển thị giá trị các register CPU. |
| reg <name> <value> | Set giá trị register CPU. |

## 2️⃣ Memory Access
| Command | Description |
|---------|-------------|
| mem32 <addr> | Đọc 32-bit word tại địa chỉ. |
| mem16 <addr> | Đọc 16-bit half-word. |
| mem8 <addr> | Đọc 8-bit byte. |
| w32 <addr> <value> | Ghi 32-bit word. |
| w16 <addr> <value> | Ghi 16-bit half-word. |
| w8 <addr> <value> | Ghi 8-bit byte. |

## 3️⃣ Flash / File Operations
| Command | Description |
|---------|-------------|
| loadbin <file>, <addr> | Nạp file binary vào flash/memory. |
| loadfile <file> | Nạp file Intel HEX vào MCU. |
| erase | Xóa toàn bộ flash. |
| erase <addr> | Xóa sector flash chứa địa chỉ cụ thể. |
| savebin <file>, <addr>, <size> | Đọc memory và lưu ra file binary. |

## 4️⃣ SWD / JTAG / Trace
| Command | Description |
|---------|-------------|
| dap | Hiển thị thông tin Debug Access Port. |
| trace | Bật trace / ETM / ITM nếu MCU hỗ trợ. |

## 5️⃣ RTT (via Viewer / GDB)
| Command | Description |
|---------|-------------|
| rttstart | Khởi động RTT. |
| rttread | Đọc dữ liệu RTT từ MCU. |
| rttstop | Dừng RTT. |

## 6️⃣ Utilities / Info
| Command | Description |
|---------|-------------|
| version | Hiển thị version J-Link. |
| help / ? | Hiển thị danh sách lệnh. |
| save / load | Lưu / load thiết lập J-Link. |
| exit | Thoát J-Link Commander. |

---
💡 **Note:**
- Các lệnh nạp flash (`loadbin`, `erase`) thường đi kèm `r` và `g` để reset và chạy MCU.
- RTT commands (`rttstart`, `rttread`) thường dùng qua **RTT Viewer hoặc GDBServer**, không trực tiếp trong J-Link Commander.
- `speed` tối ưu tuỳ MCU và board, thường từ 4000–12000 kHz.
- `step` và `halt` dùng khi debug instruction-level.

