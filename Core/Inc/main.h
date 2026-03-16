#ifndef __MAIN_H
#define __MAIN_H
#include "stdio.h"
#include "stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_itm.h"
#include "stm32f4xx_gpio.h"
#include "core_m4.h"
#include "../Src/semi_io.c"
#include "log.h"
#include "debugFunc.h"
#include "SEGGER_RTT.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f411xe.h"
#include "stm32f4xx.h"
#include "BST.h"
#include "myRTOS.h"
#include "stm32f4_usart.h"
#include "stm32f4_dma.h"
#include "app_dma.h"
// Configuage clock
#define RCC_CR              (*(volatile uint32_t*)0x40023800)
#define RCC_PLLCFGR         (*(volatile uint32_t*)0x40023804)
#define RCC_CFGR            (*(volatile uint32_t*)0x40023808)

#define RTT_printf(...)  SEGGER_RTT_printf(0,__VA_ARGS__)
#define LOG_REG(name) SEGGER_RTT_printf(0, #name " = 0x%08X\n", (unsigned int)(name))  //for register
//%08X là một định dạng cho printf:
//%X : in số nguyên không dấu dưới dạng thập lục phân chữ IN HOA (A..F).
//8 : chiều rộng tối thiểu là 8 ký tự.
//0 : đệm bằng ký tự 0 (nếu độ dài thực tế nhỏ hơn 8).


//TODO: check xem đang dùng clock nào
//RCC_CFGR 0x08



#define ROM_M4_PID4   (*(volatile uint32_t*)0xE00FFFD0) // Peripheral ID4 in ROM table (Cortex-M4)

#define ROM_M4_CPM3   (*(volatile uint32_t*)0xE00FFFFC) // Peripheral ID4 in ROM table (Cortex-M4)

#define ROM_M4_SCS    (*(volatile uint32_t*)0xE00FF000) // Peripheral ID4 in ROM table (Cortex-M4)



struct Dummy {
    int member1;
    int member2;
};


// Priority ground
typedef enum {
    ground_16_and_sub_0 = 0b000,   // Group [7:5] , Sub [4]
    ground_8_and_sub_2  = 0b100 << 8,   // Group [7:5] , Sub [4]
    ground_4_and_sub_4  = 0b100 << 8,   // Group [7:6] , Sub [5:4]
    ground_2_and_sub_8  = 0b100 << 8,   // Group [7]   , Sub [6:4]
    ground_0_and_sub_16 = 0b100 << 8,   // Group None  , Sub [7:4]
} BinaryPoint;




int global1 = 5; // Global variable located in RAM(section .data)
int global2; // Global variable located in RAM(section .bss)

inline uint32_t  __get_MSP();
inline void ConfigClockHSE16MHZ();
extern uart_feature uart_core;

// Callback for Error handler
void (*func_OverrunError)(uart_log_level log_level) = NULL;

// Callback for complete receiver
uart_status (*func_receiverComplete)(char *buff)= NULL;

// Callback for Trans handler
uart_status (*func_transHandler)(void) = NULL;

extern void initialize_monitor_handles(void);
extern int test_extern;
void register_callback_write_complete(uart_callback_t callback);


void USART2_IRQHandler(void);
void HardFault_Handler(uint32_t *pStack);
void EXTI9_5_IRQHandler(void);
void SVC_Handler(void);
void EXTI0_IRQHandler(void);
void configGpio();
// Set the priority grouping
void NVIC_SetPriorityGrouping(uint32_t priority_grouping) {

}

void NVIC_SetSubPriority(uint32_t priority_grouping) {
    
}

//Testting
void NVIC_ConfigPriority(BinaryPoint config) {
    uint32_t temp = SCB->AIRCR;
    // unlock key
    temp |= (0x5FA << 16);
    // write
    SCB->AIRCR = temp;

    // Read
    if ( ((temp >> 8) & 0x07) & config ) {
        //the same confg
        return;
    } else {
        SCB->AIRCR |= config;
    }
    
}

inline void Reset_MCU() {
    uint32_t temp = SCB->AIRCR;
    // unlock key
    temp |= (0x5FA << 16);
    // write
    SCB->AIRCR = temp;

    SCB->AIRCR |= 0x1 << 2;
}

#endif