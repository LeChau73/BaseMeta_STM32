#include "main.h"


typedef struct
{
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} DMA_Base_Registers;


static DMA_HandleTypeDef   dma_mem_to_mem;

void DMA_Config_Mem_to_Mem(void) {
    /* 2. Bật Clock cho bộ điều khiển DMA */
    __HAL_RCC_DMA2_CLK_ENABLE();

    dma_mem_to_mem.Instance = DMA2_Stream1;
    dma_mem_to_mem.Instance->CR |= DMA_IT_TC;

    dma_mem_to_mem.Init.Direction = DMA_MEMORY_TO_MEMORY;
    dma_mem_to_mem.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    dma_mem_to_mem.Init.MemInc = DMA_MINC_ENABLE;
    dma_mem_to_mem.Init.PeriphInc = DMA_PINC_ENABLE;

    dma_mem_to_mem.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_mem_to_mem.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;

    dma_mem_to_mem.Init.Priority = DMA_PRIORITY_HIGH;
    dma_mem_to_mem.Init.Mode = DMA_NORMAL;
    ////////////////////////////////////

    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0); 
    //HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);


    /* 4. Gọi hàm Init để áp dụng cấu hình */
    if (HAL_DMA_Init(&dma_mem_to_mem) != HAL_OK)
    {
      // Xử lý lỗi nếu khởi tạo thất bại
      Error_Handler();
    } 

}

void dma_mem_copy(char* source, char* destination, size_t lenght) {
    uint32_t tickstart = HAL_GetTick();
    uint32_t end_tick = 0, duraction = 0;
    uint32_t mask_cpltlevel;

    uint32_t start, end;

    start = DWT->CYCCNT;

    dma_mem_to_mem.Instance->CR &= (uint32_t)(~DMA_SxCR_EN);

    dma_mem_to_mem.Instance->PAR = (uint32_t)source;
    dma_mem_to_mem.Instance->M0AR = (uint32_t)destination;
    dma_mem_to_mem.Instance->NDTR = lenght;
    Init_Data_Source(source, lenght);

    dma_mem_to_mem.Instance->CR |= (uint32_t)(DMA_SxCR_EN);

    mask_cpltlevel = DMA_FLAG_TEIF1_5 << dma_mem_to_mem.StreamIndex;
    uint32_t steam = *(uint32_t*)dma_mem_to_mem.StreamBaseAddress;
    
    while((steam & mask_cpltlevel) == RESET && (dma_mem_to_mem.Instance->CR & DMA_SxCR_EN) == SET);

    end = DWT->CYCCNT;
    uint32_t cycles = end - start;
    end_tick = HAL_GetTick();
    duraction = end_tick - tickstart;

    char result = compare(source, destination, lenght);


    start = DWT->CYCCNT;
    // CPU tranfer
    memset(destination, 0, lenght);
    for (int i = 0; i < lenght; i++)
    {
        destination[i] = source[i];
    }
    end = DWT->CYCCNT;
    uint32_t cycles2 = end - start;

    if(cycles2 > cycles) {
        RTT_printf("Cpu lower\n");
        
        myPrintf("Cpu lower\n");
    }


    myPrintf("result %d\n", result);

}

char compare(char* source, char* destination, size_t lenght) {
    int i = 0;
    for( i = 0; i < lenght; i++) {
        if(source[i] != destination[i])
            return i;
    }
    //-1 : là 2 mảng đều giống nhau
    return -1;
}

void Init_Data_Source(char* f_source,size_t size) {
    if(f_source == NULL)
        return;

    for(int i = 0; i < size; i++) {
        f_source[i] = (char)i;
    }
}