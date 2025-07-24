#ifndef PRINTF_LOG_H
#define PRINTF_LOG_H

#ifdef __cplusplus
extern "C" {
#endif



typedef enum
{
    INT = 0,
    HEX,
    STRING
}type_log;

#define MAX_SIZE_BUFF 64


char *int_to_string(int num, char *buffer, int* size);
// Function prototypes
void PrintfLog_Init(void);
void Printf_Int( int value );




#ifdef __cplusplus
}
#endif

#endif // PRINTFLOG_H