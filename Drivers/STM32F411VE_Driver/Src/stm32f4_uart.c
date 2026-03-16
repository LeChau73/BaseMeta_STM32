#include "stm32f4_usart.h"


uart_feature uart_core;





uart_status HAL_uart_Init(usart_config* config) {

    //Enable clock mới ghi được
    //mọi thao tác ghi vào thanh ghi đó đều vô hiệu (giá trị vẫn giữ nguyên là 0).
    RCC->APB1ENR |= (1 << 17); // Bật USART2 clock (Bit 17)
    USART2->USART_DR = 0;
    USART2->USART_SR &= ~(1 << 6);

    LOG_REG_COLOR(USART2->USART_SR);
    uint8_t current_over;
    float USARTDIV;
    //For clock mà đi vào UART(APB1)
    uint32_t fCK = get_pclk1_frequency();
    RTT_printf("Clock uart = %d", fCK);
    uint8_t over[2] = {8, 16};

   //* Enable USART (UE bit) 
    // Read
    uint32_t temp = USART2->USART_CR1;
    // Clear
    temp = CLEAR_BIT_UART(temp, 13);
    // Modify
    temp |= SET_BIT_UART(temp, 13);

    USART2->USART_CR1 = temp;

    // ----------- BAUD RATE----------------
    //* for baud rate */
    temp = USART2->USART_BRR;
    // Clear
    temp = CLEAR_REG(temp);

    //get bit over
    if((USART2->USART_CR1 >> 15) & 0x01) {
        //oversampling by 8
        current_over = over[0];
    } else {
        current_over = over[1];
    }

    uint32_t result;
    //DEBUG FPU Lệnh Assembly đọc thanh ghi Floating-point Status and Control Register
    __asm volatile ("VMRS %0, fpscr" : "=r" (result));
    RTT_printf("FPU enable = %d\n", result);


    /*
     * CALCULATION LOGIC FOR USART_BRR:
     * -------------------------------------------------------------------------
     * Formula: USARTDIV = fCK / (8 * (2 - OVER8) * Baud)
     * * Example with fCK = 16MHz, Baud = 9600, OVER8 = 0 (divisor = 16):
     * 1. USARTDIV = 16,000,000 / (16 * 9600) = 104.1666...
     * 2. Mantissa (Phần nguyên) = 104 (0x68)
     * 3. Fraction (Phần thập phân): 
     * - Lấy 0.1666... nhân với 16 (vì phần Fraction trong BRR có 4 bit, max = 16)
     * - 0.1666 * 16 = 2.6656
     * - Làm tròn thành 3 (0x3)
     * 4. BRR = (Mantissa << 4) | (Fraction & 0x0F) = 0x683
     * -------------------------------------------------------------------------
     */

    USARTDIV = (float)fCK / (float)(current_over*config->baudrate);

    // Lấy phần nguyên trực tiếp bằng cách ép kiểu
    uint32_t Mantissa = (uint32_t)USARTDIV; 

    // Tính phần thập phân (Fraction)
    // Cách 1: Dùng trực tiếp số thực (Dễ hiểu nhất cho người mới đọc code)
    float fraction_float = USARTDIV - (float)Mantissa;
    uint16_t end_Fraction = (uint16_t)(fraction_float * 16.0f + 0.5f); // +0.5 để làm tròn (round) thay vì bị cắt cụt (truncate)

    RTT_printf("Mantissa = %d, Fraction (Hex) = %d\n", Mantissa, end_Fraction);

    // Nạp vào thanh ghi: BRR = [Mantissa][Fraction]
    temp = (Mantissa << 4) | (end_Fraction & 0x0F);
    USART2->USART_BRR = temp;

    // -----------END CONFIG BAUD RATE----------------

    // -----------START CONFIG LENGHT----------------
    switch(config->length) {
        case 8:
            CLEAR_BIT_UART(USART2->USART_CR1, 12);
            break;
        case 9:
            SET_BIT_UART(USART2->USART_CR1, 12);
            break;
        default:
            RTT_printf("ERROR : length cannot valid %d ", config->length);
            return ERROR_UART;
    }

    switch(config->config_mode) {
        case MODE_SEND:
            // Set TE bit ->  send idle frame
            SET_BIT_UART(USART2->USART_CR1, 3);
            break;
        case MODE_RECEVICE:
            // Set RE bit -> receiver frame
            SET_BIT_UART(USART2->USART_CR1, 2);
            break;
        case ALL:
        LOG_REG_COLOR(USART2->USART_SR);
            // Set TE bit ->  send idle frame
            SET_BIT_UART(USART2->USART_CR1, 3);
            // Set RE bit -> receiver frame
            SET_BIT_UART(USART2->USART_CR1, 2);
            //NOTE: Sau khi set TE thì idle frame tran thì
            //BIT TXE = 1 & TC = 1
            int count = 10000;
            while((USART2->USART_CR1 & (0x01 << 3)) == 0 && count-- != 0) {
                __asm volatile ("NOP");
            }
            if(count == 0) {
                return ERROR_UART;
            }
            LOG_REG_COLOR(USART2->USART_SR);
            break;
        default:
            RTT_printf("Cannot choose mode recevice or send \n");
            return ERROR_UART;
    }

    // Enable ISR for AURT
    //USART2->USART_CR1 |= config->configISR;

    //Enable DMA reception
    USART2->USART_CR3 |= config->enable_DMA << 6;

    LOG_REG_COLOR(USART2->USART_CR1);
    LOG_REG_COLOR(USART2->USART_SR);
    //TODO: các option khác
    __asm volatile ("dsb 0xF":::"memory");
    return SUCCESS_UART;
}

// Gửi 1 byte có đợi
uart_status HAL_uart_tran1byte(uint8_t data) {
    // wait until TXE=1 , BIT 7
    if( ((USART2->USART_SR & 0x01 << 7)) != 0 ) {
        USART2->USART_DR = data;
        //TEX cleared after write to DR
    }
    //Đợi cho đến khi nào TC(tran complete)
    while(!(USART2->USART_SR & (0x01 << 6)));
}

//NOTE:
    // Nếu gửi nhiều byte((TXE == 0)) thì TC k thể set lên 1 được
    // Bởi vì write vào DR thì TXE sẽ được clear
uart_status HAL_uart_tranMul(uint8_t buffer[], int size) {
    if(buffer == NULL || size <= 0) return ERROR_UART;
    
    // Gửi từng byte
    for(int i = 0; i < size; i++) {
        // Đợi TXE=1 (DR trống)
        uint16_t timeout = 10000;
        while(!(READ_BIT_UART(USART2->USART_SR, 7)) && timeout--) {
            __asm volatile("NOP");
        }
        if(timeout == 0) return ERROR_UART;
        
        USART2->USART_DR = buffer[i];
    }

    // Đợi TC=1 (tất cả byte gửi xong) - SỬA SAI BITWISE
    uint16_t timeout = 10000;
    while(!(USART2->USART_SR & (0x01 << 6)) && timeout--) {  // ✅ & thay vì &&
        __asm volatile("NOP");
    }
    
    return (timeout == 0) ? ERROR_UART : SUCCESS_UART;
}

/*  number_Frame : Số lượng frame break muốn gửi đi */
uart_status HAL_send_break_frame(uint8_t number_Frame) {
    // Wait for bit SBK = 0
    volatile uint32_t temp = USART2->USART_CR1;
    uint32_t timeout = 100000;
    while(number_Frame--) {
        SET_BIT_UART(USART2->USART_CR1, 0);
        
        // Flush memory buffer to pheripheral
        __asm volatile ("dsb 0xF":::"memory");
        LOG_REG(USART2->USART_CR1);
        while((USART2->USART_CR1 & 0x01) && timeout--) {
            __asm volatile ("NOP");
        }
        
        if(timeout == 0) return ERROR_UART;
        
        
    }
    return SUCCESS_UART;
}

uart_status HAL_uart_receiver1byte(char* buffer) {
    // Wait for until RXNE == 1 corresponds to data from shift register has tran to RDR
    // Can be read
    if(buffer == NULL)
        return ERROR_UART;
    while(READ_BIT_UART(USART2->USART_SR, 5));
    *buffer = (char)USART2->USART_DR;
    return SUCCESS_UART;
}

void func_OverrunErrorHander(uart_log_level log_level) {
    // Xử lý lỗi Overrun
    // Nhớ trình tự xóa ORE: Đọc SR rồi đọc DR
    uint32_t dummy = USART2->USART_SR;
    dummy = USART2->USART_DR;
    (void)dummy;
}

uint8_t status_ring_buffer(uart_feature* config) {
    uint8_t head = config->cache_buffer.head;
    uint8_t tail = config->cache_buffer.tail;
    if( (head + 1 ) % TX_BUF_SIZE == tail)
    {
        return FULL;
    } else if ( head == tail ) {
        return EMPTY;
    }
        
    return AVAILABLE;
}

void uart_write_it(char* data, int len, int flush) {
    if(data == NULL)
        return;

    // Chech ring buffer is avalable
    uint8_t status_ring = status_ring_buffer(&uart_core);
    // Check ring còn trống or là đang empty
    if( status_ring == AVAILABLE || status_ring == EMPTY ) {
        uint8_t index = 0;
        while ( data[index] != '\0' && len-- != 0 )
        {
            ring_buffer_push(&uart_core.cache_buffer, data[index++]);
        }
    }

    // Check có thể tran được k
    // Nếu mode flush thì đẩy ra luôn hoặc ring đang full
    // Nếu không thì ghi đến lúc nào đầy mới đẩy
    if ( flush == 1 || status_ring_buffer(&uart_core) == FULL ) {
        control_engine_ISR();
    }

}

ty_status_engine control_engine_ISR() {
    uint16_t timeout = 10000;
    // Wait for until TXE == 1
    while (READ_BIT_UART(USART2->USART_SR, 7) != 1 && timeout--) {
        __asm volatile("NOP");
    }

    if(timeout == 0) {
        return BUSY;
    }
    RTT_printf("status ring = %d\n", status_ring_buffer(&uart_core));
    // Get first byte
    if( status_ring_buffer(&uart_core) != EMPTY ) {
        USART2->USART_DR = ring_buffer_pop(&uart_core.cache_buffer);     // -> Triggle ISR
        USART2->USART_CR1 |= (0x1 << 6); // TCIE = 1 bit 6 CR1 -> Triggle ISR
        // XEm TC ở đây == 1 k?
    }

}

void initStructure(uart_feature* config) {
    
    for(uint8_t i = 0 ; i < TX_BUF_SIZE; i++ ) {
        config->cache_buffer.buff[i] = 0;
    }
    config->tranComplete_ptr = NULL;

}

void register_callback_write_complete(uart_callback_t callback) {
    initStructure(&uart_core);
    uart_core.tranComplete_ptr = callback;
}

void call_callback_uart() {
    if(uart_core.tranComplete_ptr != NULL)
    {
        uart_core.tranComplete_ptr();
    } else {
        RTT_printf("Cannot register callback\n");
    }

}


uart_status uart_tran_hander_it(void) {
    //RTT_printf("Send byte [%c]\n", );
     //BUG: lấy data chưa đúng
    USART2->USART_DR = ring_buffer_pop(&uart_core.cache_buffer);

    return SUCCESS_UART;
}


uint8_t ring_buffer_count_available(ring_buffer_uart* rb) {
    //count số lượng còn lại (TX_BUF_SIZE - 1) - head;
    //0 ---- tail(5) --- head(8) --- end(10)
    //trống từ 0 -> 5 + 8 -> 10 => 7
    uint8_t total = (TX_BUF_SIZE - 1) - rb->head + rb->tail;
    return total;
}

static uint8_t ring_buffer_push(ring_buffer_uart* rb,char data) {
    rb->buff[rb->head] = data;
    rb->head = (rb->head + 1) % TX_BUF_SIZE;
    return rb->head;
}

static char ring_buffer_pop(ring_buffer_uart* rb) {
    char data = 0;
    if(rb->tail != rb->head) {
        data = rb->buff[rb->tail]; // trả về dữ liệu hiện tại
        rb->tail = ( rb->tail + 1 ) % TX_BUF_SIZE;
    }
    RTT_printf("Send data [%c] \n", data);
    return data;
}


// AI viết: dùng để đọc clock cấp vào UART2(APB1)
uint32_t get_pclk1_frequency(void) {
    uint32_t sysclk = 0;
    uint32_t pllvco = 0, pllp = 0, pllsource = 0;
    
    // 1. Xác định nguồn SYSCLK hiện tại
    uint32_t sws = (RCC->CFGR & RCC_CFGR_SWS);
    
    switch (sws) {
        case 0x00: // HSI (Nội 16MHz)
            sysclk = 16000000;
            break;
        case 0x04: // HSE (Ngoại, thường là 8MHz hoặc 25MHz)
            sysclk = 25000000; // Thay đổi con số này theo thạch anh trên board bạn (thường Black Pill là 25MHz)
            break;
        case 0x08: // PLL
            // Tính toán SYSCLK từ cấu hình PLL
            pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
            uint32_t pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM);
            uint32_t plln = (RCC->PLLCFGR >> 6) & 0x1FF;
            pllp = (((RCC->PLLCFGR >> 16) & 0x03) + 1) * 2;
            
            if (pllsource == 0) // PLL từ HSI
                pllvco = (16000000 / pllm) * plln;
            else                // PLL từ HSE
                pllvco = (25000000 / pllm) * plln; // Thay 25MHz nếu cần
                
            sysclk = pllvco / pllp;
            break;
        default:
            sysclk = 16000000;
            break;
    }

    // 2. Tính toán AHB Prescaler
    uint32_t hpre_val = (RCC->CFGR & RCC_CFGR_HPRE) >> 4;
    uint8_t ahb_div_table[16] = {1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 8, 16, 64, 128, 256, 512};
    uint32_t hclk = sysclk / ahb_div_table[hpre_val];

    // 3. Tính toán APB1 Prescaler (PPRE1)
    uint32_t ppre1_val = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;
    uint8_t apb_div_table[8] = {1, 1, 1, 1, 2, 4, 8, 16};
    
    return hclk / apb_div_table[ppre1_val];
}