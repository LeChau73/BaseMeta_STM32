# APPLTask - Thằng Trung Gian (Dispatcher/Router) Của Hệ Thống

## Câu Trả Lời Ngắn
**Có**, APPLTask là thằng trung gian đứng giữa để nối kết/điều phối tất cả các phần (stage handlers) trong hệ thống.

---

## Cấu Trúc Tổng Thể

```
┌─────────────────────────────────────────────────────────────┐
│                   APPLMainTask (main loop)                  │
│           (trong APPLTaskMain.c - line 1234)                │
│                                                             │
│  1. Chờ message queue (MsgGetPNo)                          │
│  2. Parse message type (pNo) & sender (fId)                │
│  3. Dispatch đến handler phù hợp (switch/case)            │
│  4. Gọi handler function (APPLLotStartProc, etc)          │
│  5. Nhận response từ handler                               │
│  6. Gửi response trở lại message queue (MQSend)           │
│  7. Loop lại                                               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
        ↑                                   ↓
        │                                   │
   ┌----┴----┬─────────┬──────┬─────────┬──┴──────┐
   │         │         │      │         │         │
   │         ↓         ↓      ↓         ↓         ↓
   │    LOT_START  LOAD   ALIGN    TEST   UNLOAD  LOT_END
   │      (PL000) (PP000) (PP001) (PP002)(others)(others)
   │
   └── Từ: masterTask, GpibTask, portTask, MMITask, etc.
```

---

## APPLMainTask - Main Loop (APPLTaskMain.c @ line 1234)

```c
VOID APPLMainTask( ... )
{
    intl pNo;      // Message number (process type)
    intl fId;      // From Task ID (sender)
    intl tot;      // To Task ID (receiver)
    intb readBuf[...];   // Buffer nhận message
    intb sendBuf[...];   // Buffer gửi response
    
    // === MAIN LOOP ===
    FOREVER
    {
        // Step 1: RECEIVE message
        pNo = MsgGetPNo( &fId, &tot, &msgNum, &readBuf[0], ... );
        
        tot = fId;  // Response sẽ gửi lại cho người gửi
        
        if( pNo != ERROR )
        {
            // Step 2: DISPATCH dựa trên message type
            switch( pNo )
            {
            case 1:  // PL000 = LOT_START
                stat = APPLLotStartProc( fId, &readBuf[0], &tot, &sendBuf[0] );
                break;
                
            case 2:  // PP000 = LOAD
                stat = APPLLoadProc( fId, &readBuf[0], &tot, &sendBuf[0] );
                break;
                
            case 3:  // PP001 = ALIGNMENT (main process)
                stat = APPLMainProc( fId, &readBuf[0], &tot, &sendBuf[0] );
                break;
                
            case 4:  // PP002 = OTHER (TEST, UNLOAD, etc)
                stat = RPSuspend( fId, &readBuf[0], &tot, &sendBuf[0] );
                break;
                
            // ... nhiều case khác ...
            }
        }
        
        // Step 3: SEND response
        // (xử lý response và gửi lại thông qua message queue)
    }
}
```

---

## Nói Rõ: APPLTask Là "Intermediary" Như Thế Nào?

| Thành Phần | Vai Trò | Ví Dụ |
|-----------|---------|--------|
| **Sender** | Gửi message đến APPLTask | MasterTask gửi "PP001" (ALIGNMENT) |
| **APPLTask** | **Nhận** message, **parse**, **dispatch** | Nhận "PP001", gọi APPLMainProc() |
| **Handler** | Xử lý logic thực tế | APPLMainProc() xử lý alignment |
| **Sender** | **Nhận** response từ APPLTask | MasterTask nhận kết quả từ APPLTask |

---

## Message Flow Chi Tiết

```
1. MasterTask:
   MQSend(ApplTask, "PP001", ...);    // Gửi PP001 (ALIGNMENT)
                                       
2. APPLMainTask Receives:
   pNo = MsgGetPNo() → pNo = 3        // case 3 = PP001
   fId = MasterTask ID                 // Ghi nhớ ai gửi
                                       
3. APPLMainTask Dispatches:
   switch(3) → case 3:
   stat = APPLMainProc(fId, buf...);  // Gọi handler chính
                                       
4. APPLMainProc() Processes:
   → Xử lý alignment logic
   → Xác định next stage
   → Return status (SEND_MSG, NON_MSG, etc)
                                       
5. APPLMainTask Response:
   if( stat == SEND_MSG )
       MQSend(tot, sendBuf, ...);      // Gửi response về MasterTask
```

---

## APPLTask Nối Kết Những Gì?

### **Incoming Messages (từ các task khác)**
- `MasterTask` → Điều khiển master, synchronize
- `portTask` → Giao tiếp với bên ngoài
- `MMITask` → Giao tiếp từ LCD/UI
- `GpibTask` → Giao tiếp từ GPIB/tester
- `ftpTask` → FTP file transfer requests

### **Outgoing Handlers (APPLTask gọi)**
- `APPLLotStartProc()` → Xử lý LOT_START
- `APPLLoadProc()` → Xử lý LOAD wafer
- `APPLMainProc()` → Xử lý ALIGNMENT/TEST/UNLOAD (main logic!)
- `RPSuspend()` → Xử lý suspend/resume
- Và nhiều others...

### **Backend Support**
- `aLotStart.c` → Detailed LOT_START logic
- `aAlignProc.c` → Detailed ALIGNMENT logic
- `aWafTest.c` → Detailed TEST logic
- `aLoadProcNew.c` → Detailed LOAD logic

---

## Message Types (từ switch pNo)

| pNo | Message | File Xử Lý | Ý Nghĩa |
|-----|---------|-----------|---------|
| 1 | PL000 | APPLLotStartProc() | LOT_START |
| 2 | PP000 | APPLLoadProc() / TableLoadProc() | LOAD |
| 3 | PP001 | APPLMainProc() | ALIGNMENT / TEST / UNLOAD |
| 4 | PP002 | RPSuspend() | SUSPEND / RESUME |
| 5+ | Others | Various | Various operations |

---

## Kiến Trúc Tổng (Architecture Diagram)

```
┌─────────────────────────────────┐
│   EXTERNAL INPUTS               │
│  (Tester, UI, Network, Loader)  │
└────────────┬────────────────────┘
             │ Message
             ↓
┌─────────────────────────────────┐
│  APPLI_TASK (APPLMainTask)      │  ← **INTERMEDIARY/ROUTER**
│  [Message Dispatcher Loop]      │
│                                 │
│  - Receive: MsgGetPNo()        │
│  - Dispatch: switch(pNo)        │
│  - Process: Call handlers       │
│  - Respond: MQSend()           │
└────────┬────────────────────────┘
         │
    ┌────┴────┬────────┬─────┬──────┐
    │          │        │     │      │
    ↓          ↓        ↓     ↓      ↓
┌──────┐  ┌────────┐ ┌─────┐ ┌────┐ ┌────┐
│LOT   │  │  LOAD  │ │ALIGN│ │TEST│ │OTHR│
│START │  │ WAFER  │ │    │ │    │ │    │
└──────┘  └────────┘ └─────┘ └────┘ └────┘
   │         │        │       │       │
   ↓         ↓        ↓       ↓       ↓
 aDur    Load      Align   Test   After
 Test    Table     Proc    Proc   Test
 .c      .c       .c      .c     .c
 
 (Detailed implementations)
```

---

## Tại Sao APPLTask Là Trung Gian?

1. **Centralized Control**: Tất cả messages đi qua APPLTask
2. **Message Queue Management**: APPLTask quản lý message queue (FIFO)
3. **Dispatch Logic**: Quyết định pesan nào → handler nào
4. **Synchronization**: Đồng bộ hóa giữa các tasks (thông qua MasterTask)
5. **Error Handling**: APPLTask xử lý error responses
6. **State Management**: APPLTask theo dõi state của machine

---

## So Sánh

| Concept | Ví Dụ | Trong Code |
|---------|-------|-----------|
| **Trung Gian** | Receptionist tại công ty | APPLMainTask |
| **Gửi message** | Khách hàng gọi đến | MasterTask gửi "PP001" |
| **Parse** | Receptionist nghe hiểu | pNo = MsgGetPNo() |
| **Dispatch** | Chuyển đến phòng đúng | switch(pNo) case 3 |
| **Handler** | Nhân viên xử lý vấn đề | APPLMainProc() |
| **Response** | Feedback lại khách | MQSend(response) |

---

## Kết Luận

**APPLTask = Thằng Trung Gian/Router/Dispatcher**

- Nhận messages từ tất cả tasks khác
- Parse message type → xác định task nào gửi, cần xử lý gì
- Dispatch đến handler phù hợp (LOT_START, ALARM, TEST, etc)
- Nhận response từ handlers
- Gửi response lại cho requester
- Loop vô tận chờ messages mới

**Không có APPLTask**:
- Không có tập trung điều khiển
- Không có message routing
- Từng task tự quản lý logic (dẫn đến chaos)

**Có APPLTask**:
- Tập trung điều khiển tất cả
- Messages được **serialized** (xử lý lần lượt)
- Logic có trật tự, dễ debug
- Dễ thêm features (thêm case trong switch)
