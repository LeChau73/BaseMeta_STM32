#ifndef __MAIN_H
#define __MAIN_H
#include "stdio.h"
#include "stdlib.h"
#include "stm32f4xx_itm.h"

extern void initialize_monitor_handles(void);

struct Dummy {
    int member1;
    int member2;
};

int global1 = 5; // Global variable located in RAM(section .data)
int global2; // Global variable located in RAM(section .bss)







#endif