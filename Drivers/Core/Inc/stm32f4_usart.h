#ifndef UART_H
#define UART_H
#include "core_m4.h"
#include "stm32f411xe.h"

#define USART2_BASE  (PERIPH_BASE + 0x4400UL)

#define USART2   ((Usart_Type*)USART2_BASE)


typedef struct {
    __IOM uint32_t USART_SR;
    __IOM uint32_t USART_DR;
    __IOM uint32_t USART_BRR;
    __IOM uint32_t USART_CR1;
    __IOM uint32_t USART_CR2;
    __IOM uint32_t USART_CR3;
    __IOM uint32_t USART_GTPR;
} Usart_Type;

typedef enum {
    Bit_s_9600,
    //more
}option_baurate;

typedef enum {
    ERROR_UART,
    SUCCESS_UART,
}uart_status;

typedef struct 
{
    uint32_t baudrate;
    uint8_t length;             //Set bit M to choose 8 or 9
    uint8_t stop_bit;           // 1/1.5/2 bit stop
}usart_config;


uart_status HAL_uart_Init(usart_config* config);
uart_status HAL_uart_tran1byte(uint8_t data);
uart_status HAL_uart_tranMul(uint8_t buffer[],int size);
uint32_t get_pclk1_frequency(void);




#endif