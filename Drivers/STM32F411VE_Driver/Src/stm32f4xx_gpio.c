#include "stm32f4xx_gpio.h"




void GPIO_Init(GPIO_Pin_t* gpiox, GPIO_Config* gpio_config)
{
    //Check condition exception
    if (gpiox == 0 || gpio_config == 0 )
        return ;

    uint16_t pos = 0;

    //Mỗi config tương ứng với các thanh ghi trong gpio

    //Mà có 16 pin ,mỗi pin có mode riêng

    //thanh ghi index i = 0, pin i = 1 => convert 2 giá trị này tương đương nhau
    for( uint16_t i = 0 ; i < 16; i++ )
    {
        if (gpiox->pin != 0)
        {
            pos = (gpiox->pin & (0x01 << i));
        }

        // add config mode for corresponding pin
        if ( gpio_config->alternate == 0 )
        {
            // For input and output
            if ( gpio_config->mode |= MODE_OUTPUT )
            {
                gpiox->port->gpiox_MODER |= (uint16_t)(gpio_config->mode << pos);
                uint16_t outType = (gpio_config->mode >> 16);
                gpiox->port->gpiox_OTYPER |= outType << pos;
                gpiox->port->gpiox_OSPEEDR |= (gpio_config->speed << pos);
            } else {
                // mode input
                gpiox->port->gpiox_MODER |= (uint16_t)(gpio_config->mode << pos);
                gpiox->port->gpiox_PUPDR |= (gpio_config->pull << pos);
            }


        } else {
            // For mode alternate

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