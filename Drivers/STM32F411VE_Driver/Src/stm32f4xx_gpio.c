#include "stm32f4xx_gpio.h"
#include "SEGGER_RTT.h"

int test_extern = 10;

void GPIO_Init(GPIO_Pin_t* gpiox, GPIO_Config* gpio_config)
{
    uint32_t ioposition = 0;
    uint32_t iocurrent = 0;
    
    //myPrintf("%x", gpiox->pin);
    //Check condition exception
    if (gpiox == NULL || gpio_config == NULL )
        return;

    uint16_t pos = 0;

    //Mỗi config tương ứng với các thanh ghi trong gpio

    //Mà có 16 pin ,mỗi pin có mode riêng

    //thanh ghi index i = 0, pin i = 1 => convert 2 giá trị này tương đương nhau
    //DEBUG: gpiox->pin = PIN3 | PIN4 | PIN5
    // gpiox->pin = 0x0000 0000 0011 1000
    for(uint16_t pin_num = 0; pin_num < 16; pin_num++)
    {
        ioposition = 0x01 << pin_num;
        iocurrent = (uint32_t)(gpiox->pin) & ioposition;
        //check whether that pin i is configurated ?
        if ( ioposition & iocurrent )
        {
                RTT_printf("=====Befor PinNum = %d====\n", pin_num);
                // For input and output
                if ( ( gpio_config->mode & MODE_OUTPUT ) || ( gpio_config->mode & MODE_AF ) )
                {
                    uint32_t temp = gpiox->port->OSPEEDR;
                    temp &= ~(0x03 << pin_num * 2);
                    temp |= (uint32_t)(gpio_config->speed << pin_num * 2);
                    gpiox->port->OSPEEDR = temp;
                }

                /** Configuration output type : 0 : push-pull | 1 : open-drain **/
                uint32_t temp = gpiox->port->OTYPER;
                temp &= ~(0x01 << pin_num);
                /* Conver value to mode : because : mode && output type the same a value*/                                                      // clear old value
                temp |= (uint32_t)(((gpio_config->mode & OUTPUT_TYPE) >> OUTPUT_TYPE_Pos) << pin_num);
                gpiox->port->OTYPER = temp;

                /** Configuration pull-up/pull-down : 0 :  00: No pull-up, pull-down
                                                           01: Pull-up
                                                           10: Pull-down **/
                temp = gpiox->port->PUPDR;
                temp &= ~(0x03 << pin_num * 2);                                                         // clear old value
                temp |= (uint32_t)(gpio_config->pull << pin_num * 2);
                gpiox->port->PUPDR = temp;

                /** Configuration mode :  Input (reset state)
                                          01: General purpose output mode
                                          10: Alternate function mode
                                          11: Analog mode **/

                temp = gpiox->port->MODER;
                temp &= ~(0x03 << pin_num * 2);                                                         // clear old value
                temp |= (uint32_t)( ((gpio_config->mode & GPIO_MODE ) >> GPIO_MODE_Pos) << pin_num * 2 );
                gpiox->port->MODER = temp;
                
                //O alternate function low register 
                // 2 thanh ghi : 1 cái cho 8 pin | 1 cái cho 8 pin
                //              Mỗi pin sẽ mapping đến AF0 -> AF15 | AF0 -> sẽ là mode gì(chẳng hạn như UART)
                // User sẽ setting Pin và mode AF[0 -> 15] 0000 -> 1111
                // 0 - 7
                // 1111 : 0 -> 7 thì bit 4 luôn bằng 0, 8 -> 15 thì bit 4 sẽ bằng 1

                uint32_t* ptr = &gpiox->port->AFR[0];
                ptr  +=   ( pin_num >> 3) & 0x01;                                                   //xác định xem AFR L | H
                *ptr &=  ~( ( uint32_t ) ( 0x0F << ( pin_num % 8 ) * 4 ) );                          //clear old
                *ptr |=   ( uint32_t ) ( gpio_config->alternate  << ( pin_num % 8 ) * 4 ) ;          //Setting  [ pin_num % 8 ] vì thanh ghi đó chỉ setting 0 -> 8

                //TODO: Codding tiếp phần EXTI:
                    //2 mode: setting thanh ghi tương ứng
                if ( ( gpio_config->mode & EXTI_MODE ) != 0 )
                {

                     //TODO: Codding tiếp phần EXTI:
                    //2 mode: setting thanh ghi tương ứng
                if ( ( gpio_config->mode & EXTI_MODE ) != 0 )
                {
                    __HAL_RCC_SYSCFG_CLK_ENABLE();//enable clock for SYSCFG
                    // Read
                    temp = SYSCFG->EXTICR[ pin_num >> 2 ]; //dịch 2 chia cho 2^2
                    LOG_REG(temp);
                    LOG_REG_COLOR(EXTI->EMR);
                    LOG_REG_COLOR(EXTI->IMR);
                    LOG_REG_COLOR(EXTI->RTSR);
                    LOG_REG_COLOR(EXTI->FTSR);
                    // hiểu backgroud đã
                    // bởi vì: 0 - 3 : là 1 thanh ghi, 1 pin sở hữu 4 bit
                    // [pin: 0 1111 ][pin: 1 1111 ][pin: 2 1111 ][pin: 3 1111 ]
                    // [pin: 12 1111 ][pin: 13 1111 ][pin: 14 1111 ][pin: 15 1111 ]
                    // Ví dụ pin 13 ==> expected : 1 * 4
                    // Clear : tìm index của pin đó trong thanh ghi (nibble)
                    // Ví dụ : 13( 1101 & 11) =>> 1 << 2 => 1 * 4 = 4
                    // Như kiểu : 13 % 4 = 1 [ tức là phần dư chỉ 0 1 2 3 tương ứng với nibble ]
                    temp &= ~(0xFU << ( (pin_num & 0x3) << 2) );            //HACK: Hiểu phép chia này sau
                    //Modify
                    //💡 Tip: Trên ARM embedded, tránh % với hằng số là 2^n, luôn dùng & để giảm lệnh và tăng tốc.
                    temp |= ( GPIO_GET_INDEX(gpiox->port) << ( (pin_num & 0x03) << 2) );
                    //Writing
                    SYSCFG->EXTICR[ pin_num >> 2 ] = temp;
                    
                    //* Event mask register
                    temp = EXTI->EMR;
                    temp &= ~(iocurrent);
                    if (gpio_config->mode & EXTI_EVT)
                    {
                        temp |= iocurrent;
                    }
                    EXTI->EMR = temp;
                    //* Interrupt mask register 
                    temp = EXTI->IMR;
                    temp &= ~(iocurrent);
                    if (gpio_config->mode & EXTI_IT)
                    {
                        temp |= iocurrent;
                    }
                    EXTI->IMR = temp;

                    //* Rising trigger selection register
                    temp = EXTI->RTSR;
                    temp &= ~(iocurrent);
                    if (gpio_config->mode & TRIGGER_RISING)
                    {
                        temp |= iocurrent;
                    }
                    EXTI->RTSR = temp;

                    //* Falling trigger selection register
                    temp = EXTI->FTSR;
                    temp &= ~(iocurrent);
                    if (gpio_config->mode & TRIGGER_FALLING)
                    {
                        temp |= iocurrent;
                    }
                    EXTI->FTSR = temp;

                    RTT_printf("=====After=====\n");
                    LOG_REG_COLOR1(EXTI->EMR);
                    LOG_REG_COLOR1(EXTI->IMR);
                    LOG_REG_COLOR1(EXTI->RTSR);
                    LOG_REG_COLOR1(EXTI->FTSR);
                    LOG_REG_COLOR1(gpiox->port->MODER);

                    // THIẾU : k mapping pin đó vào line nào
                    // một line EXTI có thể được cấu hình đồng thời cho cả hai cạnh: Rising và Falling.
                    // một line EXTI có thể được cấu hình đồng thời cả .
                    // IMR (Interrupt Mask Register) và EMR (Event Mask Register) hoàn toàn có thể được bật đồng thời cho cùng một EXTI line.

                }
            }

                
        
        } else {
            continue; // Skip if pin not selected
        }
    }

}

void GPIO_Toogle(GPIO_Pin_t* gpiox) 
{
    uint16_t pos = 0;
    for(uint8_t i = 0 ; i < 16; i++)
    {
        pos = (gpiox->pin & (0x01 << i));
        gpiox->port->ODR |= (gpiox->pin << pos);
    }
}