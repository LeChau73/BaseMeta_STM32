#include "app_dma.h"


char buffer[100];

void DMA_UART_to_Mem() {
    
    usart_config config;
    config.length = 8;
    config.baudrate = 9600;
    config.config_mode = ALL;
    config.enable_DMA = 0x1;
    
    int status;

    status = HAL_uart_Init(&config);
    
    DMA_configuration_Ty dma_config;
    dma_config.destination = (uint32_t)buffer;
    dma_config.source_addr = (uint32_t)&USART2->USART_DR;
    dma_config.width_source = Byte_1;
    dma_config.width_destination = Byte_1;
    dma_config.number_data_items = 10;
    dma_config.channel_id_selection = CHANNEL_4;
    dma_config.mode_struture = CIRCULAR_MODE_DISNABLE;
    dma_config.mode_tranfer = PHERIPHERAL_TO_MEM;
    dma_config.lev_priority = MEDIUM;
    dma_config.mem_increment_mode = 1;
    dma_config.peripheral_increment_mode = 0;
    dma_config.enable_ISR_Complete = 1;
    dma_config.Half_transfer_ISR = 1;

    RTT_printf("Destination %x\n", dma_config.destination);
    RTT_printf("Source %x\n", dma_config.source_addr);

    DMA_Init(&dma_config, DMA1_Stream5);

    NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    if(status == -1)
        RTT_printf("Errorr\n");
}


