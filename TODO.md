# 🚀 UART DMA Driver Project Tracking

## 1. Ring Buffer Layer (Data)
- [x] Định nghĩa Struct RingBuffer (Head, Tail, Capacity)
- [x] Viết hàm `RB_Push` (Xử lý wrap-around)
- [x] Implement Atomic Access cho Head/Tail (LDREX/STREX)
- [x] Unit test: Push dữ liệu vượt ngưỡng store

## 2. Engine Manager (Middle)
- [ ] Thiết kế State Machine (IDLE, BUSY, ERROR)
- [ ] Viết hàm `UART_Engine_TriggerTx`
- [ ] Xử lý Callback cho Upper Layer
- [ ] Cơ chế Timeout/Health check trong vòng `while(1)`

## 3. Hardware Layer (Low-level)
- [x] Cấu hình UART Register (Baudrate, Mode)
- [ ] Cấu hình DMA Stream & Channel
- [ ] Viết ISR cho DMA Transfer Complete
- [ ] Viết ISR cho UART Error (ORE, FE, NE)

## 4. Application Layer
- [ ] Hoàn thiện hàm `Log_Write` (vsnprintf)
- [ ] Tích hợp cơ chế Retry/Timeout khi Buffer full