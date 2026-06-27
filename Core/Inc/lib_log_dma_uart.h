#ifndef _LIB_LOG_DMA_UART_H__
#define _LIB_LOG_DMA_UART_H__
#include "lib_engine_manager.h"
#include "log.h"
#include <stdlib.h>


#define MAX_NAME_SUB 24
#define MAX_BUFF 128


typedef enum {
    NO_LOGGING,
    NORMAL,
    DEBUG,
    WARNING,
    ERROR_LOG,
    MAX_LEVEL
}tyLogLevel;

typedef enum {
    DMA_TRAN_FAIL,
    DMA_FIFO_ERROR,
    DMA_UNALIGH_MENT,
    UART_TRAN_FAIL,
    BUFFER_ERROR,
    TIMEOUT
}ERROR_TYPE;

static char* tableLevelStr[MAX_LEVEL] = {
    "NO_LOGGING",
    "NORMAL",
    "DEBUG", 
    "WARNING",
    "ERROR",
};


typedef enum {
    MAIN,
    DMA,
    UART,
    RING_BUFFER,
    MAX_SUBSYSTEM
}ty_SubSystem_Log;

typedef struct {
    ty_SubSystem_Log id_sub;
    char nameSub[MAX_NAME_SUB];
    tyLogLevel levelLog;
}tyTableSub;

// table config data
static tyTableSub tableSub[MAX_SUBSYSTEM] = {
    MAIN,        "main",        NORMAL,
    DMA,         "dma",         NO_LOGGING ,
    DMA,         "dma",         NO_LOGGING,
    UART,        "uart",        NO_LOGGING,
    RING_BUFFER, "ring_buffer", NO_LOGGING
};

typedef uint8_t (*fn_send_to_hardware)(char* data, int lenghth);
extern void register_interface(fn_send_to_hardware fn);
static inline uint8_t strlenSub(const char* strSub);

//typedef void (*callbackStatus)(ERROR_TYPE error_type);  //for error
typedef void (*callbackComplete)();

typedef struct {
    tyTableSub* subSytem;
    uint8_t global_enable_log;
}sLogData;

typedef enum {
    Write_Success,
    Write_Fail
}LOG_STATUS;


typedef enum {
    BUFFER_FULL,
    BUFFER_EMPTY,
    BUFFER_AVAILABLE
}STATUS_BUFFER;

struct status_machine {
    ERROR_TYPE          type_error;
    STATUS_BUFFER       status_buffer;
};


LOG_STATUS Log_Write(const char* fmt, ...);   // Write log to buffer ring
LOG_STATUS log_flush(void);
LOG_STATUS log_Str(ty_SubSystem_Log subsystem, const char* string);
//void register_callback(callbackStatus callback_error,callbackComplete callback_compelte );
struct status_machine GetStatus(void);          //Status of buffer and machine

sLogData* log_Init();

uint8_t Adapter_Hardware_UART_DMA(char* src, int size);



#endif