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

extern void initialize_monitor_handles(void);



struct Dummy {
    int member1;
    int member2;
};

int global1 = 5; // Global variable located in RAM(section .data)
int global2; // Global variable located in RAM(section .bss)


inline void ConfigClockHSE16MHZ();

#endif