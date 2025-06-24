#ifndef __MAIN_H
#define __MAIN_H
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "../Src/semi_io.c"

extern void initialize_monitor_handles(void);



struct Dummy {
    int member1;
    int member2;
};

int global1 = 5; // Global variable located in RAM(section .data)
int global2; // Global variable located in RAM(section .bss)



#define RCC_BASE       0x40023800UL
#define GPIOD_BASE     0x40020C00UL
#define RCC_AHB1ENR    (*(volatile unsigned int *)(RCC_BASE + 0x30))
#define GPIOD_MODER    (*(volatile unsigned int *)(GPIOD_BASE + 0x00))
#define GPIOD_OTYPER   (*(volatile unsigned int *)(GPIOD_BASE + 0x04))
#define GPIOD_OSPEEDR  (*(volatile unsigned int *)(GPIOD_BASE + 0x08))
#define GPIOD_PUPDR    (*(volatile unsigned int *)(GPIOD_BASE + 0x0C))
#define GPIOD_ODR      (*(volatile unsigned int *)(GPIOD_BASE + 0x14))
#define GPIOD_BSRR     (*(volatile unsigned int *)(GPIOD_BASE + 0x18))

void led_on(uint8_t pin);
void led_off(uint8_t pin);
void gpio_init(void);
inline void ConfigClockHSE16MHZ();

#endif