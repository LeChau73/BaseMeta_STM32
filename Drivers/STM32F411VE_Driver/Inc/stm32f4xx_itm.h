
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_ITM_H
#define __STM32F4xx_ITM_H

#include "core_m4.h"





#ifdef __cplusplus
extern "C"
{
#endif

// Bỏ từ khóa static để hàm có thể dùng chung
void Init_ITM(uint8_t port, uint8_t prescaler);
void ITM_SendChar(char c); 
void ITM_SendString(char* str);
void ITM_SendInt(int32_t num);




#ifdef __cplusplus
}
#endif


#endif