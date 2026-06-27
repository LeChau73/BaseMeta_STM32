#include"lib_log_dma_uart.h"

// [SUB SYSTEM] : [LEVEL] : [DATA] > ver1
// [SUB SYSTEM] : [TIME] : [LEVEL] : [DATA] > ver1


extern UART_HandleTypeDef huart2;
static sLogData* logData = NULL;

uint8_t Adapter_Hardware_UART_DMA(char* src, int size) {
    HAL_UART_Transmit_DMA(&huart2, src, size);
}

sLogData* Log_Init() {

    register_interface(Adapter_Hardware_UART_DMA);

    // single intances
    if(logData != NULL) {
        return logData;
    }

    logData = (sLogData*)malloc(sizeof(sLogData));
    if (logData == NULL)
    {
        return NULL;
    }

    logData->subSytem = &tableSub[0]; // mapping table init subsytem
    logData->global_enable_log = 1;

    return logData;
}


LOG_STATUS log_Str(ty_SubSystem_Log subsystem, const char* string) {

    if(subsystem < MAIN || subsystem > MAX_SUBSYSTEM)
        return Write_Fail;

    Log_Init();

    int len = 0, lenghtLevel = 0;

    if(logData->global_enable_log == 0)
        return Write_Fail;
    
    char message[MAX_BUFF];
    
    memset(message, 0x00, MAX_BUFF);

    // Find id để tìm ra sub system => tìm ra được tên sub string và level log của nó
    if( logData->subSytem[subsystem].id_sub < MAX_SUBSYSTEM ) {

        const tyTableSub* sub_s = &logData->subSytem[subsystem];
        if(sub_s == NULL)
            return Write_Fail;

        // Tên của module đó dài bao nhiêu

        len = strlenSub(sub_s->nameSub) - 1;
        
        // Coppy tên module vào message -> message = [SUB SYSTEM]
        strncpy(message, sub_s->nameSub, len ); //[SUB SYSTEM]

        // message = [SUB SYSTEM] | 
        strncpy(message + len, " | ", 3);
        len += 3;

        // message = [SUB SYSTEM] | [Level log]
        lenghtLevel = strlen(tableLevelStr[sub_s->levelLog]);
        strncpy(message + len, tableLevelStr[sub_s->levelLog], lenghtLevel);
        len += lenghtLevel;

        // message = [SUB SYSTEM] | [Level log] | 
        strncpy(message + len, " | ", 3);
        len += 3;
    }

    // message = [SUB SYSTEM] | [Level log] | [DATA] \n
    strncpy(message + len, string, strlen(string));
    len += strlen(string);

    push(message, len);
}


static inline uint8_t strlenSub(const char* strSub) {
    uint8_t count;
    while(strSub[count++] != '\0');
    return count;
}