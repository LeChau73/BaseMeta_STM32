Searched for text `ChkLotEndMsg_NEC`, 10 results

Searched for files matching `**/ERROR_MAIN.c`, 1 match

Read [](file:///d%3A/STM32_WORKSPACE/VxWork/TEL_Prober/code/probLib/ERROR_MAIN.c#8970-8970), lines 8970 to 9030

Searched for text `ChkLotEndMsg_All` (`TEL_Prober` · `code/probLib/ERROR_MAIN.c`), 5 results

Read [](file:///d%3A/STM32_WORKSPACE/VxWork/TEL_Prober/code/probLib/ERROR_MAIN.c#8900-8900), lines 8900 to 8960

Searched for regex `sendAssist.*LOT_END|errFmt.*LOT_END`, 15 results

Searched for text `sendAssist`, 5 results

Searched for text `errFmt`, 10 results

Searched for regex `^VOID.*errFmt\s*\(` (`TEL_Prober` · `code/probLib/ERROR_MAIN.c`), no results

Searched for text `AdvEventFtpPut`, 5 results

Read [](file:///d%3A/STM32_WORKSPACE/VxWork/TEL_Prober/code/probLib/eventLogControl.c#679-679), lines 679 to 750

# Kiểm Tra LOT_END GỬI ĐẾN XEM

Bây giờ tôi sẽ truy vết toàn bộ flow LOT_END → FTP:

## 📍 **1. LOT_END Message Được Gửi Từ Đâu:**

### Task gửi **LOT_END** (file khác nhau):
```c
// ✓ aLotEnd.c (dòng 2443)
sendAssist( outMsg , APPL_ASSIST+LOT_END ,ERR_APPLI_ACK);

// ✓ masterTask.c (dòng 7154)
errFmt( &palnBuf[0], APPL_ASSIST+LOT_END, ERR_APPLI_ACK, ApplTask );

// ✓ portTask.c (dòng 1135)
sendAssist(buf,APPL_ASSIST + LOT_END,ERR_APPLI_ACK);

// ✓ APPLTaskMain.c (dòng 1747)
sendAssist( &testEndBuff[0], APPL_ASSIST+LOT_END, ERR_APPLI_ACK );
```

**Gửi đến:** EventLogControl task (thông qua Message Queue)

---

## 📍 **2. EventLogControl Task Nhận LOT_END:**

### **eventLogControl.c** (dòng 454-462)
```c
// Nhận tin từ Message Queue
pNo = MQGetNo( rxBuff, &assNo, 3 );  // ← Trích ID

// Kiểm tra nếu là LOT_END
if( ChkLotEndMsg_NEC( assNo ) == OK )  // ← Kiểm tra ID = APPL_ASSIST + LOT_END
{
    pNo = MQGetNo( rxBuff, &assNo, 1 );
    if ( assNo == EVT_MSGCLEAR )  // ← Kiểm tra nó có phải "Message Clear"
    {
        setEventLogFileName (fileName, today, 1);  // ← Tạo tên file
        AdvEventFtpPut ( dirName, fileName );      // ← GỤI FTP! 📤
    }
}
```

---

## 📍 **3. AdvEventFtpPut Gửi File Qua FTP:**

```c
// eventLogControl.c (dòng 686)
STATUS AdvEventFtpPut (unsb *dirName, unsb *fileName )
{
    STATUS sts;

    if (SPOPTCHK( SPO_ADVANCED_LOG ) == 0)  // Check option
        return OK;

    if ( (FtpEvtLogFileHostIp[0] != 0) && 
         (FtpEvtLogFileUserId[0] != 0) &&
         (FtpEvtLogFilePasswd[0] != 0) && 
         (FtpEvtLogFileUpDir[0] != 0) )
    {
        sts = FtpPutEventLogFile ( dirName, fileName );  // ← Gửi FTP!
        //         ↑
        //  Gửi đến FTP Server
    }
    return (sts);
}
```

---

## 🎯 **Tóm Lại Flow:**

```
Task (aLotEnd, masterTask, v.v...)
        │
        ├─→ sendAssist(APPL_ASSIST + LOT_END)
        │
        ▼
Message Queue
        │
        ▼
EventLogControl Task
        │
        ├─→ MQGetNo() → Trích ID
        │
        ├─→ ChkLotEndMsg_NEC() → Kiểm tra = LOT_END?
        │
        ├─→ ✓ OK!
        │
        ├─→ AdvEventFtpPut()
        │
        ▼
FtpPutEventLogFile()
        │
        ▼
FTP SERVER 📤
        (FtpEvtLogFileHostIp)
        (FtpEvtLogFileUpDir)
```

---

## **Key Destination:**

| Thành Phần | Giá Trị |
|-----------|--------|
| **Gửi đến Task** | **EventLogControl** |
| **Gửi FTP qua** | **FtpPutEventLogFile()** |
| **Đến Server** | **FTP Host IP** (từ config) |
| **Thư Mục FTP** | **FtpEvtLogFileUpDir** |

**Kết luận:** LOT_END message gọi trigger `AdvEventFtpPut()` → gửi FTP file qua `FtpPutEventLogFile()` đến FTP server.