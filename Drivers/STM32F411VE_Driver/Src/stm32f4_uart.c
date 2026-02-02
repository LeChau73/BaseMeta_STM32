#include "stm32f4_usart.h"


uart_status HAL_uart_Init(usart_config* config) {

    //Enable clock mới ghi được
    //mọi thao tác ghi vào thanh ghi đó đều vô hiệu (giá trị vẫn giữ nguyên là 0).
    RCC->APB1ENR |= (1 << 17); // Bật USART2 clock (Bit 17)

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
    temp = CLEAR_BIT(temp, 13);
    // Modify
    temp = SET_BIT(temp, 13);

    USART2->USART_CR1 = temp;

    // ----------- BAUD RATE----------------
    //* for baud rate */
    temp = USART2->USART_BRR;
    // Clear
    temp = CLEAR_REG(temp);

    // Modify

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
            CLEAR_BIT(USART2->USART_CR1, 12);
            break;
        case 9:
            SET_BIT(USART2->USART_CR1, 12);
            break;
        default:
            RTT_printf("ERROR : length cannot valid %d ", config->length);
            return ERROR_UART;
    }
    // Set TE bit ->  send idle frame
    SET_BIT(USART2->USART_CR1, 3);


    LOG_REG_COLOR(USART2->USART_SR);
    //TODO: các option khác

    return SUCCESS_UART;
}

// Gửi 1 byte có đợi
uart_status HAL_uart_tran1byte(uint8_t data) {
    LOG_REG_COLOR(USART2->USART_SR);
    // wait until TXE=1 , BIT 7
    uint32_t bit = READ_BIT(USART2->USART_SR, 7);
    RTT_printf("bit = %d\n",bit);
    if(READ_BIT(USART2->USART_SR, 7) == 1) {
        USART2->USART_DR = data;
        //TEX cleared after write to DR
        LOG_REG_COLOR(USART2->USART_SR);
    }
    //Đợi cho đến khi nào TC(tran complete)
    while(!(USART2->USART_SR & (0x01 << 6)));
    LOG_REG_COLOR(USART2->USART_SR);
}

//NOTE:
    // Nếu gửi nhiều byte((TXE == 0)) thì TC k thể set lên 1 được
    // Bởi vì write vào DR thì TXE sẽ được clear
uart_status HAL_uart_tranMul(uint8_t buffer[], int size) {
    LOG_REG_COLOR(USART2->USART_SR);
    if(READ_BIT(USART2->USART_SR, 7) == 1) {
        for(int i = 0; i < size; i++) {
            USART2->USART_DR = buffer[i];
            // Đợi TXE=1 ,DR empty
            LOG_REG_COLOR(USART2->USART_SR);
            while(!READ_BIT(USART2->USART_SR, 7));
        }
    }
    LOG_REG_COLOR(USART2->USART_SR);
    while(!(USART2->USART_SR && (0x01 << 6)));
    
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