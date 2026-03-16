#include"stm32f4_dma.h"


bool DMA_Init(DMA_configuration_Ty* Dma_config, DMA_Stream_TypeDef* dma_stream) {
    if(Dma_config == NULL)
        return false;


    volatile uint32_t temp;


    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    // Data transfer direction
    temp = dma_stream->CR;
    temp &= ~(0x3 << 6);        //clear
    temp |= Dma_config->mode_trans << 6;

    // Circular mode
    temp |= (Dma_config->mode_struture << 8);

    // Peripheral increment mode
    temp |= (Dma_config->peripheral_increment_mode << 9);

    // Memory increment mode
    temp |= (Dma_config->mem_increment_mode << 10);

    if( Dma_config->mode_trans != MEM_TO_MEM ) {
        // Peripheral data size
        temp &= ~(0x3 << 11);        //clear
        temp |= Dma_config->width_source << 11;

        // Memory data size
        temp &= ~(0x3 << 13);        //clear
        temp |= Dma_config->width_destination << 13;
    }


    //Priority level
    temp &= ~(0x3 << 16);        //clear
    temp |= Dma_config->lev_priority << 16;

    //: Channel selection
    temp &= ~(0x3 << 25);        //clear
    temp |= Dma_config->channel_id_selection << 25;

    //: TCIE: Transfer complete interrupt enable
    temp &= ~(0x1 << 4);        //clear
    temp |= Dma_config->enable_ISR_Complete << 4;

    //: HTIE: Half transfer interrupt enable
    temp &= ~(0x1 << 3);        //clear
    temp |= Dma_config->Half_transfer_ISR << 3;

    dma_stream->CR = temp;

    dma_stream->PAR = 0; //clear
    dma_stream->M0AR = 0;
    // Destination address
    switch (Dma_config->mode_trans)
    {
    case PHERIPHERAL_TO_MEM:
        dma_stream->PAR = Dma_config->source_addr; //
        dma_stream->M0AR = Dma_config->destination; //  TODO: nếu dùng 2 mem
        break;
    case MEM_TO_PHERIPHERAL:
        dma_stream->PAR = Dma_config->destination; //
        dma_stream->M0AR = Dma_config->source_addr; //  TODO: nếu dùng 2 mem
        break;
    case MEM_TO_MEM:
            dma_stream->PAR = Dma_config->destination; //TODO: Check
        dma_stream->M0AR = Dma_config->source_addr;
        break;
    default:
        break;
    }

    dma_stream->NDTR = 0;
    dma_stream->NDTR = Dma_config->number_data_items;

    // Stream enabled
    dma_stream->CR |= 0x01;


    LOG_REG(dma_stream->CR);
    LOG_REG(dma_stream->NDTR);

    return 1;
}