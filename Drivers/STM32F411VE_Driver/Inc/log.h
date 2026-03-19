#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f411xe.h"
#include "stm32f4xx_itm.h"
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

typedef enum
{
    INT = 0,
    HEX,
    STRING
}type_log;

#define MAX_SIZE_BUFF 64

void led_on(uint8_t pin);
void led_off(uint8_t pin);
void debug_by_led(void);
void gpio_init(void);
char *int_to_string(int num, char *buffer);
void myPrintf(const char* fmt, ...);
char* convert_hex_to_string(uint32_t value, char* buffer);
void hex_to_ascii_bytes(uint32_t value, char* buffer);
// Function prototypes
void PrintfLog_Init(void);
void print_int( int value );

#define GPIOD_BASE     0x40020C00UL
#define GPIOD_BSRR     (*(volatile unsigned int *)(GPIOD_BASE + 0x18))


#ifdef __cplusplus
}
#endif

#endif // PRINTFLOG_H