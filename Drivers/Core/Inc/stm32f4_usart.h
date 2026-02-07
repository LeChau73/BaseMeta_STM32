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
    ERROR_UART = -1,
    SUCCESS_UART,
}uart_status;


typedef enum {
    MODE_SEND = 0x01,
    MODE_RECEVICE = 0x02,
    ALL = MODE_SEND | MODE_RECEVICE
} mode_config;

typedef enum {
    ENABLE_TRANS    =   (0x1 << 6), //TCIE = 1 bit 6 CR1
    DISNABLE_TRAN   =   ~(0x1 << 6), //TCIE = 0
    ENABLE_RECIVER  =   (0x1 << 5), //RXNEIE = 1 bit 6
    DISNABLE_RECIVER =  ~(0x1 << 5), 
    Overrun_Error   =   ENABLE_RECIVER, //RXNEIE = 1
    // more .. 
}config_interrupt;

//TODO: Refactor to optimize memmory
typedef struct
{
    uint32_t baudrate;
    uint8_t length;             //Set bit M to choose 8 or 9
    uint8_t stop_bit;           // 1/1.5/2 bit stop
    uint8_t break_frame;        // option để send one break frame
    mode_config config_mode;    // mode reciver/tran
    config_interrupt configISR;
}usart_config;

typedef enum {
    NORMAL_UART_LOG,
    ERROR_UART_LOG
}uart_log_level;

uart_status HAL_uart_Init(usart_config* config);
uart_status HAL_uart_tran1byte(uint8_t data);
uart_status HAL_uart_tranMul(uint8_t buffer[],int size);
uart_status HAL_send_break_frame(uint8_t number_Frame);
uart_status HAL_uart_receiver1byte(char* buffer);
uart_status HAL_uart_receiverMul(char* buffer);
uint32_t get_pclk1_frequency(void);
void func_OverrunErrorHander(uart_log_level log_level);

// Callback for Error handler
extern void (*func_OverrunError)(uart_log_level log_level);

// Callback for complete receiver
extern uart_status (*func_receiverComplete)(char *buff);

// Callback for Trans handler
extern uart_status (*func_transComplete)(uint8_t data);

#endif