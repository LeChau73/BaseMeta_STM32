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

extern void initialize_monitor_handles(void);
extern int test_extern;

struct Dummy {
    int member1;
    int member2;
};

int global1 = 5; // Global variable located in RAM(section .data)
int global2; // Global variable located in RAM(section .bss)

inline uint32_t  __get_MSP();
inline void ConfigClockHSE16MHZ();

// Priority ground
typedef enum {
    ground_16_and_sub_0 = 0b000,   // Group [7:5] , Sub [4]
    ground_8_and_sub_2  = 0b100 << 8,   // Group [7:5] , Sub [4]
    ground_4_and_sub_4  = 0b100 << 8,   // Group [7:6] , Sub [5:4]
    ground_2_and_sub_8  = 0b100 << 8,   // Group [7]   , Sub [6:4]
    ground_0_and_sub_16 = 0b100 << 8,   // Group None  , Sub [7:4]
} BinaryPoint;


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

void Reset_MCU() {
    uint32_t temp = SCB->AIRCR;
    // unlock key
    temp |= (0x5FA << 16);
    // write
    SCB->AIRCR = temp;

    SCB->AIRCR |= 0x1 << 2;
}



#endif