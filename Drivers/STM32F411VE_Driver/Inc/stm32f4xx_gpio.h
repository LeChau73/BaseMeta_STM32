#ifndef __STM32F4xx_GPIO_H
#define __STM32F4xx_GPIO_H


#ifdef __cplusplus
extern "C"
{
#endif


#include "core_m4.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal_rcc.h"



/* structure for register */
typedef struct
{
    __IO uint32_t MODER;    // 0x00
    __IO uint32_t OTYPER;   // 0x04
    __IO uint32_t OSPEEDR;  // 0x08
    __IO uint32_t PUPDR;    // 0x0C
    __IO uint32_t IDR;      // 0x10
    __IO uint32_t ODR;      // 0x14
    __IO uint32_t BSRR;     // 0x18
    __IO uint32_t LCKR;     // 0x1C
    __IO uint32_t AFRL;     // 0x20
    __IO uint32_t AFRH;     // 0x24
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

#define OUTPUT_TYPE_Pos                         4U
#define OUTPUT_TYPE                             (0x1UL << OUTPUT_TYPE_Pos)
#define OUTPUT_PP                               (0x0UL << OUTPUT_TYPE_Pos)
#define OUTPUT_OD                               (0x1UL << OUTPUT_TYPE_Pos)


// EXTI : External Interrupt
//E → External (ngoài)
//XTI → Interrupt (ngắt)

#define EXTI_MODE_Pos                           16U                                 //0xFFF0FFFF
#define EXTI_MODE                               (0x3UL << EXTI_MODE_Pos)            //                0b0000 0000 0000 0011 0000 0000 0000 0000
#define EXTI_IT                                 (0x1UL << EXTI_MODE_Pos)            //interupt :      0b0000 0000 0000 0001 0000 0000 0000 0000
#define EXTI_EVT                                (0x2UL << EXTI_MODE_Pos)            //sự kiện ở ngoài 0b0000 0000 0000 0010 0000 0000 0000 0000
//                   **
//



#define TRIGGER_MODE_Pos                         20U
#define TRIGGER_MODE                            (0x7UL << TRIGGER_MODE_Pos)       // 0b0000 0000 0111 0000 0000 0000 0000 0000
#define TRIGGER_RISING                          (0x1UL << TRIGGER_MODE_Pos)       //Mode lấy rising nếu cấu hình ngắt ngoài
#define TRIGGER_FALLING                         (0x2UL << TRIGGER_MODE_Pos)       //Mode lấy falling nếu cấu hình ngắt ngoài



#define  GPIO_MODE_IT_RISING                    (MODE_INPUT | EXTI_IT | TRIGGER_RISING)                     /*!< External Interrupt Mode with Rising edge trigger detection          */
#define  GPIO_MODE_IT_FALLING                   (MODE_INPUT | EXTI_IT | TRIGGER_FALLING)                    /*!< External Interrupt Mode with Falling edge trigger detection         */
#define  GPIO_MODE_IT_RISING_FALLING            (MODE_INPUT | EXTI_IT | TRIGGER_RISING | TRIGGER_FALLING)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection  */
 
#define  GPIO_MODE_EVT_RISING                   (MODE_INPUT | EXTI_EVT | TRIGGER_RISING)                     /*!< External Event Mode with Rising edge trigger detection             */
#define  GPIO_MODE_EVT_FALLING                  (MODE_INPUT | EXTI_EVT | TRIGGER_FALLING)                    /*!< External Event Mode with Falling edge trigger detection            */
#define  GPIO_MODE_EVT_RISING_FALLING           (MODE_INPUT | EXTI_EVT | TRIGGER_RISING | TRIGGER_FALLING)   /*!< External Event Mode with Rising/Falling edge trigger detection     */





#define OUTPUT_PP   MODE_OUTPUT | PUSH_PULL
#define OUTPUT_OD   MODE_OUTPUT | OPEN_DRAIN
#define INPUT_FLOATING  0x00
#define ALTERNATE   0x10

#define PULL_UP         0x01
#define PULL_DOWN       0x10
#define GPIO_NOPULL     0x00




/** @defgroup GPIO_mode_define GPIO mode define
  * @brief GPIO Configuration Mode
  *        Elements values convention: 0x00WX00YZ
  *           - W  : EXTI trigger detection on 3 bits
  *           - X  : EXTI mode (IT or Event) on 2 bits
  *           - Y  : Output type (Push Pull or Open Drain) on 1 bit
  *           - Z  : GPIO mode (Input, Output, Alternate or Analog) on 2 bits
  *         - Mode IT là đi qua NVIC gọi ngắt phần mềm
  *         - Mode Event tức là tạo ra pulse genetenor
  * @{
  */ 



//GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//GPIO_InitStruct.Pin = GPIO_PIN_0;                  // Chọn pin PA0
//GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;        // Ngắt cạnh lên
//GPIO_InitStruct.Pull = GPIO_NOPULL;                // Không kéo lên/xuống
//HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
                         

#define  GPIO_MODE_INPUT                        MODE_INPUT                                                  /*!< Input Floating Mode                   */
#define  GPIO_MODE_OUTPUT_PP                    (MODE_OUTPUT | OUTPUT_PP)                                   /*!< Output Push Pull Mode                 */
#define  GPIO_MODE_OUTPUT_OD                    (MODE_OUTPUT | OUTPUT_OD)                                   /*!< Output Open Drain Mode                */
#define  GPIO_MODE_AF_PP                        (MODE_AF | OUTPUT_PP)                                       /*!< Alternate Function Push Pull Mode     */
#define  GPIO_MODE_AF_OD                        (MODE_AF | OUTPUT_OD)                                       /*!< Alternate Function Open Drain Mode    */

#define  GPIO_MODE_ANALOG                       MODE_ANALOG                                                 /*!< Analog Mode  */
    
#define  GPIO_MODE_IT_RISING                    (MODE_INPUT | EXTI_IT | TRIGGER_RISING)                     /*!< External Interrupt Mode with Rising edge trigger detection          */
#define  GPIO_MODE_IT_FALLING                   (MODE_INPUT | EXTI_IT | TRIGGER_FALLING)                    /*!< External Interrupt Mode with Falling edge trigger detection         */
#define  GPIO_MODE_IT_RISING_FALLING            (MODE_INPUT | EXTI_IT | TRIGGER_RISING | TRIGGER_FALLING)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection  */
 
#define  GPIO_MODE_EVT_RISING                   (MODE_INPUT | EXTI_EVT | TRIGGER_RISING)                     /*!< External Event Mode with Rising edge trigger detection             */
#define  GPIO_MODE_EVT_FALLING                  (MODE_INPUT | EXTI_EVT | TRIGGER_FALLING)                    /*!< External Event Mode with Falling edge trigger detection            */
#define  GPIO_MODE_EVT_RISING_FALLING           (MODE_INPUT | EXTI_EVT | TRIGGER_RISING | TRIGGER_FALLING)   /*!< External Event Mode with Rising/Falling edge trigger detection     */



#define AF7             0x0111U


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

//return về giá trị tương ứng của thanh ghi SYSCFG_EXTICR để setting port
#define GPIO_GET_INDEX(__GPIO__)  (uint8_t)( __GPIO__ == GPIOA ? 0U : \
                                             __GPIO__ == GPIOB ? 1U : \
                                             __GPIO__ == GPIOC ? 2U : \
                                             __GPIO__ == GPIOD ? 3U : \
                                             __GPIO__ == GPIOH ? 0U : 0xFF )

#ifdef __cplusplus
}
#endif

#endif
