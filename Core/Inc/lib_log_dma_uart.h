#ifndef _LIB_LOG_DMA_UART_H__
#define _LIB_LOG_DMA_UART_H__
#include "lib_engine_manager.h"
#include "log.h"

typedef enum {
    Write_Success,
    Write_Fail
}LOG_STATUS;

typedef enum {
    DMA_TRAN_FAIL,
    DMA_FIFO_ERROR,
    DMA_UNALIGH_MENT,
    UART_TRAN_FAIL,
    BUFFER_ERROR,
    TIMEOUT
}ERROR_TYPE;


typedef enum {
    BUFFER_FULL,
    BUFFER_EMPTY,
    BUFFER_AVAILABLE
}STATUS_BUFFER;

struct status_machine {
    ERROR_TYPE          type_error;
    STATUS_BUFFER       status_buffer;
};


typedef void (*callbackStatus)(ERROR_TYPE error_type);  //for error
typedef void (*callbackComplete)();

LOG_STATUS Log_Write(const char* fmt, ...);   // Write log to buffer ring
LOG_STATUS log_flush(void);
void register_callback(callbackStatus callback_error,callbackComplete callback_compelte );
struct status_machine GetStatus(void);          //Status of buffer and machine\




#endif