#ifndef UART_H
#define UART_H
#include "core_m4.h"
#include "stm32f411xe.h"
#include <string.h>
#define USART2_BASE  (PERIPH_BASE + 0x4400UL)

#define USART2   ((Usart_Type*)USART2_BASE)

#define EMPTY 0
#define FULL 1
#define AVAILABLE 2
#define TX_BUF_SIZE 128

#define SET_BIT_UART(REG, BIT)     ((REG) |= (0x01U << BIT))
#define CLEAR_BIT_UART(REG, BIT)   ((REG) &= ~(0x01U << BIT))
#define READ_BIT_UART(REG, BIT)    ((REG) && (0x01U << BIT))

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
    DISNABLE_ALL = 0x00
    // more .. 
}config_interrupt;

/* ---- Descript about ring buffer -----
 * head : point to index avalable
 * tail : point to index is reading
 * ring_buffer full  when (head + 1) % SIZE == tail 
 * ring buffer empty when tail == head (tức là tail gặp head )
 */


typedef struct
{
  char buff[TX_BUF_SIZE];   //MAX 128 charate
  uint8_t head; // write index
  uint8_t tail; // read index
  uint8_t status;
} ring_buffer_uart;

typedef enum {
    IDLE,
    BUSY
}ty_status_engine;

typedef void (*uart_callback_t)(void);

typedef struct {
    ring_buffer_uart    cache_buffer;
    uart_callback_t     tranComplete_ptr;
    ty_status_engine    status_engine;
} uart_feature;

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
/*  Purpose : Ghi dữ liệu vào ring_buffer
    data : dữ liệu gửi đi
    len : size của data
    flush : muốn gửi ra luôn k,vì đang nằm trong bộ đệm
*/
void uart_write_it(char* data, int len, int flush);

/* Purpose : register callback when comple buffer tran */
//BUG: Tại sao inline thì k được
void register_callback_write_complete(uart_callback_t callback);

/* Control call ISR */
ty_status_engine control_engine_ISR(void);

uart_status uart_tran_hander_it(void);

void call_callback_uart(void);

void initStructure(uart_feature* config);

uint32_t get_pclk1_frequency(void);
void func_OverrunErrorHander(uart_log_level log_level);


/* ---------  API for Ring buffer ------------ */
/* Check trạng thái của ring hiện tại [ FULL | EMPTY | AVAILABLE ] */

static void ring_buffer_init(ring_buffer_uart* rb);

uint8_t status_ring_buffer(uart_feature* config);

/* Tính số lượng byte còn lại có sẵn(available) có trong ring */
static inline uint8_t ring_buffer_count_available(ring_buffer_uart* rb);

/* return về index của head hiện tại */
static uint8_t ring_buffer_push(ring_buffer_uart* rb,char data);

static uint8_t ring_buffer_push_mul(ring_buffer_uart* rb,char* data);

static char ring_buffer_pop(ring_buffer_uart* rb);

#endif