#include "stm32f4xx_gpio.h"
#include "SEGGER_RTT.h"



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


                // For input and output
                if ( ( gpio_config->mode & MODE_OUTPUT ) || ( gpio_config->mode & MODE_AF ) )
                {
                    uint32_t temp = gpiox->port->gpiox_OSPEEDR;
                    temp &= ~(0x03 << pin_num * 2);
                    temp |= (uint32_t)(gpio_config->speed << pin_num * 2);
                    gpiox->port->gpiox_OSPEEDR = temp;
                }

                /** Configuration output type : 0 : push-pull | 1 : open-drain **/
                uint32_t temp = gpiox->port->gpiox_OTYPER;
                temp &= ~(0x01 << pin_num);
                /* Conver value to mode : because : mode && output type the same a value*/                                                      // clear old value
                temp |= (uint32_t)(((gpio_config->mode & OUTPUT_TYPE) >> OUTPUT_TYPE_Pos) << pin_num);
                gpiox->port->gpiox_OTYPER = temp;

                /** Configuration pull-up/pull-down : 0 :  00: No pull-up, pull-down
                                                           01: Pull-up
                                                           10: Pull-down **/
                temp = gpiox->port->gpiox_PUPDR;
                temp &= ~(0x03 << pin_num * 2);                                                         // clear old value
                temp |= (uint32_t)(gpio_config->pull << pin_num * 2);
                gpiox->port->gpiox_PUPDR = temp;

                /** Configuration mode :  Input (reset state)
                                          01: General purpose output mode
                                          10: Alternate function mode
                                          11: Analog mode **/

                temp = gpiox->port->gpiox_MODER;
                temp &= ~(0x03 << pin_num * 2);                                                         // clear old value
                temp |= (uint32_t)( ((gpio_config->mode & GPIO_MODE ) >> GPIO_MODE_Pos) << pin_num * 2 );
                gpiox->port->gpiox_MODER = temp;
                
                //O alternate function low register 
                // 2 thanh ghi : 1 cái cho 8 pin | 1 cái cho 8 pin
                //              Mỗi pin sẽ mapping đến AF0 -> AF15 | AF0 -> sẽ là mode gì(chẳng hạn như UART)
                // User sẽ setting Pin và mode AF[0 -> 15] 0000 -> 1111
                // 0 - 7
                // 1111 : 0 -> 7 thì bit 4 luôn bằng 0, 8 -> 15 thì bit 4 sẽ bằng 1

                uint32_t* ptr = &gpiox->port->gpiox_AFRL;
                ptr  +=   ( pin_num >> 3) & 0x01;                                                   //xác định xem AFR L | H
                *ptr &=  ~( ( uint32_t ) ( 0x0F << ( pin_num % 8 ) * 4 ) );                          //clear old
                *ptr |=   ( uint32_t ) ( gpio_config->alternate  << ( pin_num % 8 ) * 4 ) ;          //Setting  [ pin_num % 8 ] vì thanh ghi đó chỉ setting 0 -> 8

                //TODO: Codding tiếp phần EXTI:
                    //2 mode: setting thanh ghi tương ứng
        
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
        gpiox->port->gpiox_ODR |= (gpiox->pin << pos);
    }
}