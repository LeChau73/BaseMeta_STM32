#ifndef __MAIN_H
#define __MAIN_H
#include "stdio.h"
#include "stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_itm.h"
#include "../Src/semi_io.c"
#include "log.h"
#include "debugFunc.h"
#include "SEGGER_RTT.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx.h"
#include "BST.h"
#include "myRTOS.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_dma.h"
#include "Ring_Buffer.h"

#define RTT_printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)
#define LOG_REG(name) SEGGER_RTT_printf(0, #name " = 0x%08X\n", (unsigned int)(name))  //for register
//%08X là một định dạng cho printf:
//%X : in số nguyên không dấu dưới dạng thập lục phân chữ IN HOA (A..F).
//8 : chiều rộng tối thiểu là 8 ký tự.
//0 : đệm bằng ký tự 0 (nếu độ dài thực tế nhỏ hơn 8).

extern UART_HandleTypeDef huart2;




// Priority ground
typedef enum {
    ground_16_and_sub_0 = 0b000,   // Group [7:5] , Sub [4]
    ground_8_and_sub_2  = 0b100 << 8,   // Group [7:5] , Sub [4]
    ground_4_and_sub_4  = 0b100 << 8,   // Group [7:6] , Sub [5:4]
    ground_2_and_sub_8  = 0b100 << 8,   // Group [7]   , Sub [6:4]
    ground_0_and_sub_16 = 0b100 << 8,   // Group None  , Sub [7:4]
} BinaryPoint;

inline uint32_t  __get_MSP();
inline void ConfigClockHSE16MHZ();

void USART2_IRQHandler(void);
void HardFault_Handler();
void HardFault_Decoder(uint32_t *pStack);
void EXTI9_5_IRQHandler(void);
void SVC_Handler(void);
void EXTI0_IRQHandler(void);
void configGpio();
// NVIC priority grouping is handled by core_cm4.h through NVIC_SetPriorityGrouping macro

void Error_Handler(void);
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void Start_UART_DMA_Receive(void);
extern HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
void dma_handler_callback( struct __UART_HandleTypeDef * hdma);

extern void dma_mem_copy(char* source, char* destination, size_t lenght);
extern void DMA_Config_Mem_to_Mem(void);
extern char compare(char* source, char* destination, size_t lenght);
extern void Init_Data_Source(char* f_source,size_t size);

void SysTick_Handler(void) ;
void HardFault_Decoder(uint32_t *pStack);
#endif