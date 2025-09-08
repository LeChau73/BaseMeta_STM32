#ifndef __STM32F4xx_GPIO_H
#define __STM32F4xx_GPIO_H

#include "core_m4.h"

#ifdef __cplusplus
extern "C"
{
#endif






/* structure for register */
typedef struct
{
    uint32_t gpiox_MODER;
    uint32_t gpiox_OTYPER;
    uint32_t gpiox_OSPEEDR;
    uint32_t gpiox_PUPDR;
    uint32_t gpiox_IDR;
    uint32_t gpiox_ODR;
    uint32_t gpiox_BSRR;
    uint32_t gpiox_LCKR;
    uint32_t gpiox_AFRL;
    uint32_t gpiox_AFRH;
} GPIO_TypeDef;

/* structor for configuration */
typedef struct
{
    uint32_t mode;
    uint32_t pull;
    uint32_t speed;
    uint32_t alternate;
}GPIO_Config;

/* Facade + Data Object */
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
}GPIO_Pin_t;



#define PUSH_PULL    0x000
#define OPEN_DRAIN   0x100

#define GPIO_MODE_Pos                           0U
#define GPIO_MODE                               (0x3UL << GPIO_MODE_Pos)
#define MODE_INPUT                              (0x0UL << GPIO_MODE_Pos)
#define MODE_OUTPUT                             (0x1UL << GPIO_MODE_Pos)
#define MODE_AF                                 (0x2UL << GPIO_MODE_Pos)
#define MODE_ANALOG                             (0x3UL << GPIO_MODE_Pos)



#define OUTPUT_PP   MODE_OUTPUT | PUSH_PULL
#define OUTPUT_OD   MODE_OUTPUT | OPEN_DRAIN
#define INPUT_FLOATING  0x00
#define ALTERNATE   0x10

#define PULL_UP     0x01
#define PULL_DOWN   0x10


//Speed
#define LOW_SPEED       0x00000000U
#define MEDIUM_SPEED    0x00000001U
#define FAST_SPEED      0x00000002U
#define HIGH_SPEED      0x00000003U



#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */



void GPIO_Init(GPIO_Pin_t* gpiox, GPIO_Config* gpio_config);
void GPIO_Toogle(GPIO_Pin_t* gpiox);



// For RCC_AHB1ENR register
#define RCC_GPIOAEN                  ((uint16_t)0x0001)
#define RCC_GPIOBEN                  ((uint16_t)0x0002) 
#define RCC_GPIOCEN                  ((uint16_t)0x0004) 
#define RCC_GPIODEN                  ((uint16_t)0x0008) 



#ifdef __cplusplus
}
#endif

#endif
