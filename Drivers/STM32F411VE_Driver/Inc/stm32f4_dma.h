#ifndef _STM32F411_DMA_
#define _STM32F411_DMA_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f411xe.h"
#include "stm32f4xx_hal_rcc.h"

#define CIRCULAR_MODE_ENABLE        1
#define CIRCULAR_MODE_DISNABLE      0

#define STREAM_DISNABLE(STREAM)                 \
    do {                                        \
        (DMA_Stream_TypeDef*)STREAM->CR |= 0;   \
    } while (0);                                \


typedef enum {
    Byte_1 = 0,
    Haft_Word = 0b01,
    Word = 0b10
}Width_Setting;

typedef enum {
    CHANNEL_0 = 0,   // 000
    CHANNEL_1 = 1,   // 001
    CHANNEL_2 = 2,   // 010
    CHANNEL_3 = 3,   // 011
    CHANNEL_4 = 4,   // 100
    CHANNEL_5 = 5,   // 101
    CHANNEL_6 = 6,   // 110
    CHANNEL_7 = 7    // 111
} channel_id;

typedef enum {
    LOW = 0,
    MEDIUM,
    HIGH,
    VERY_HIGH
}LEVEL_PRIORITY;


typedef enum {
    PHERIPHERAL_TO_MEM = 0,
    MEM_TO_PHERIPHERAL,
    MEM_TO_MEM
}Mode_tranfer;


typedef struct {
    uint32_t                    destination;
    uint32_t                    source_addr;
    Width_Setting               width_source;                       // Độ rộng của nguồn
    Width_Setting               width_destination;                  // Độ rộng dữ liệu của đích
    uint16_t                    number_data_items;                  // Số lượng data gửi đi
    channel_id                  channel_id_selection;                      // id channel
    uint8_t                     mode_struture;                      //FIFO or direct
    uint8_t                     mode_tranfer;                       // Circular mode or Normal mode
    LEVEL_PRIORITY              lev_priority;
    Mode_tranfer                mode_trans;                         // Data transfer direction
    uint8_t                     mem_increment_mode;                 //Địa chỉ base sẽ tăng lên mỗi lần trans dựa vào PSIZE
    uint8_t                     peripheral_increment_mode;
    uint8_t                        enable_ISR_Complete;                //Enable ISR sau khi truyền xong k
    uint8_t                        Half_transfer_ISR;
}DMA_configuration_Ty;


bool DMA_Init(DMA_configuration_Ty* Dma_config, DMA_Stream_TypeDef* dma_stream);


#endif