
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_ITM_H
#define __STM32F4xx_ITM_H

#include "core_m4.h"
#include <stdbool.h>




#ifdef __cplusplus
extern "C"
{
#endif

// Bỏ từ khóa static để hàm có thể dùng chung
void ITM_Init(bool enable_timestamp);
int _write(int file, char *ptr, int len);
void ITM_SendChar(uint32_t c); 
void ITM_SendString(const char* str);
void ITM_SendInt(int32_t num, uint8_t port);
void ITM_SendBuffer(const uint8_t* buffer, uint32_t size, uint8_t port);
void ITM_EnablePort(uint8_t port);
void ITM_DisablePort(uint8_t port);
uint8_t ITM_IsPortEnabled(uint8_t port);
void ITM_SendHalfWord(uint16_t data);
void ITM_SendWord(uint32_t data);







#ifdef __cplusplus
}
#endif


#endif