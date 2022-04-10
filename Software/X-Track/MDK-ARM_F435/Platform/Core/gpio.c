/*
 * MIT License
 * Copyright (c) 2017 - 2022 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "gpio.h"

const PinInfo_TypeDef PIN_MAP[PIN_MAX] =
{
    /*GPIO_Type* GPIOx;  //对应GPIOx地址
    TIM_Type* TIMx;      //对应TIMx地址
    ADC_Type* ADCx;      //对应ADCx地址

    uint16_t GPIO_Pin_x;    //对应GPIO_Pin位
    uint8_t TimerChannel;   //对应定时器通道
    uint8_t ADC_CHANNEL;*/  //对应ADC通道
    {GPIOA, TIM2, ADC1,  GPIO_Pin_0, 1, ADC_CHANNEL_0}, /* PA0 */
    {GPIOA, TIM2, ADC1,  GPIO_Pin_1, 2, ADC_CHANNEL_1}, /* PA1 */
    {GPIOA, TIM2, ADC1,  GPIO_Pin_2, 3, ADC_CHANNEL_2}, /* PA2 */
    {GPIOA, TIM2, ADC1,  GPIO_Pin_3, 4, ADC_CHANNEL_3}, /* PA3 */
    {GPIOA, NULL, ADC1,  GPIO_Pin_4, 0, ADC_CHANNEL_4}, /* PA4 */
    {GPIOA, TIM2, ADC1,  GPIO_Pin_5, 1, ADC_CHANNEL_5}, /* PA5 */
    {GPIOA, TIM3, ADC1,  GPIO_Pin_6, 1, ADC_CHANNEL_6}, /* PA6 */
    {GPIOA, TIM3, ADC1,  GPIO_Pin_7, 2, ADC_CHANNEL_7}, /* PA7 */
    {GPIOA, TIM1, NULL,  GPIO_Pin_8, 1, ADC_CHANNEL_X}, /* PA8 */
    {GPIOA, TIM1, NULL,  GPIO_Pin_9, 2, ADC_CHANNEL_X}, /* PA9 */
    {GPIOA, TIM1, NULL, GPIO_Pin_10, 3, ADC_CHANNEL_X}, /* PA10 */
    {GPIOA, TIM1, NULL, GPIO_Pin_11, 4, ADC_CHANNEL_X}, /* PA11 */
    {GPIOA, NULL, NULL, GPIO_Pin_12, 0, ADC_CHANNEL_X}, /* PA12 */
    {GPIOA, NULL, NULL, GPIO_Pin_13, 0, ADC_CHANNEL_X}, /* PA13 */
    {GPIOA, NULL, NULL, GPIO_Pin_14, 0, ADC_CHANNEL_X}, /* PA14 */
    {GPIOA, TIM2, NULL, GPIO_Pin_15, 1, ADC_CHANNEL_X}, /* PA15 */

    {GPIOB, TIM3, ADC1,  GPIO_Pin_0, 3, ADC_CHANNEL_8}, /* PB0 */
    {GPIOB, TIM3, ADC1,  GPIO_Pin_1, 4, ADC_CHANNEL_9}, /* PB1 */
    {GPIOB, TIM2, NULL,  GPIO_Pin_2, 4, ADC_CHANNEL_X}, /* PB2 */
    {GPIOB, TIM2, NULL,  GPIO_Pin_3, 2, ADC_CHANNEL_X}, /* PB3 */
    {GPIOB, TIM3, NULL,  GPIO_Pin_4, 1, ADC_CHANNEL_X}, /* PB4 */
    {GPIOB, TIM3, NULL,  GPIO_Pin_5, 2, ADC_CHANNEL_X}, /* PB5 */
    {GPIOB, TIM4, NULL,  GPIO_Pin_6, 1, ADC_CHANNEL_X}, /* PB6 */
    {GPIOB, TIM4, NULL,  GPIO_Pin_7, 2, ADC_CHANNEL_X}, /* PB7 */
    {GPIOB, TIM4, NULL,  GPIO_Pin_8, 3, ADC_CHANNEL_X}, /* PB8 */
    {GPIOB, TIM4, NULL,  GPIO_Pin_9, 4, ADC_CHANNEL_X}, /* PB9 */
    {GPIOB, TIM2, NULL, GPIO_Pin_10, 3, ADC_CHANNEL_X}, /* PB10 */
    {GPIOB, TIM2, NULL, GPIO_Pin_11, 4, ADC_CHANNEL_X}, /* PB11 */
    {GPIOB, TIM5, NULL, GPIO_Pin_12, 1, ADC_CHANNEL_X}, /* PB12 */
    {GPIOB, NULL, NULL, GPIO_Pin_13, 0, ADC_CHANNEL_X}, /* PB13 */
    {GPIOB, TIM12,NULL, GPIO_Pin_14, 1, ADC_CHANNEL_X},/* PB14 */
    {GPIOB, TIM12,NULL, GPIO_Pin_15, 2, ADC_CHANNEL_X}, /* PB15 */

    {GPIOC, NULL, ADC1,  GPIO_Pin_0, 0, ADC_CHANNEL_10},/* PC0 */
    {GPIOC, NULL, ADC1,  GPIO_Pin_1, 0, ADC_CHANNEL_11},/* PC1 */
    {GPIOC, TIM20,ADC1,  GPIO_Pin_2, 2, ADC_CHANNEL_12},/* PC2 */
    {GPIOC, NULL, ADC1,  GPIO_Pin_3, 0, ADC_CHANNEL_13},/* PC3 */
    {GPIOC, TIM9, ADC1,  GPIO_Pin_4, 1, ADC_CHANNEL_14},/* PC4 */
    {GPIOC, TIM9, ADC1,  GPIO_Pin_5, 2, ADC_CHANNEL_15},/* PC5 */
    {GPIOC, TIM3, NULL,  GPIO_Pin_6, 1, ADC_CHANNEL_X}, /* PC6 */
    {GPIOC, TIM3, NULL,  GPIO_Pin_7, 2, ADC_CHANNEL_X}, /* PC7 */
    {GPIOC, TIM3, NULL,  GPIO_Pin_8, 3, ADC_CHANNEL_X}, /* PC8 */
    {GPIOC, TIM3, NULL,  GPIO_Pin_9, 4, ADC_CHANNEL_X}, /* PC9 */
    {GPIOC, TIM5, NULL, GPIO_Pin_10, 2, ADC_CHANNEL_X}, /* PC10 */
    {GPIOC, TIM5, NULL, GPIO_Pin_11, 3, ADC_CHANNEL_X}, /* PC11 */
    {GPIOC, NULL, NULL, GPIO_Pin_12, 0, ADC_CHANNEL_X}, /* PC12 */
    {GPIOC, NULL, NULL, GPIO_Pin_13, 0, ADC_CHANNEL_X}, /* PC13 */
    {GPIOC, NULL, NULL, GPIO_Pin_14, 0, ADC_CHANNEL_X}, /* PC14 */
    {GPIOC, NULL, NULL, GPIO_Pin_15, 0, ADC_CHANNEL_X}, /* PC15 */

    {GPIOD, NULL, NULL,  GPIO_Pin_0, 0, ADC_CHANNEL_X}, /* PD0 */
    {GPIOD, NULL, NULL,  GPIO_Pin_1, 0, ADC_CHANNEL_X}, /* PD1 */
    {GPIOD, NULL, NULL,  GPIO_Pin_2, 0, ADC_CHANNEL_X}, /* PD2 */
    {GPIOD, NULL, NULL,  GPIO_Pin_3, 0, ADC_CHANNEL_X}, /* PD3 */
    {GPIOD, NULL, NULL,  GPIO_Pin_4, 0, ADC_CHANNEL_X}, /* PD4 */
    {GPIOD, NULL, NULL,  GPIO_Pin_5, 0, ADC_CHANNEL_X}, /* PD5 */
    {GPIOD, NULL, NULL,  GPIO_Pin_6, 0, ADC_CHANNEL_X}, /* PD6 */
    {GPIOD, NULL, NULL,  GPIO_Pin_7, 0, ADC_CHANNEL_X}, /* PD7 */
    {GPIOD, NULL, NULL,  GPIO_Pin_8, 0, ADC_CHANNEL_X}, /* PD8 */
    {GPIOD, NULL, NULL,  GPIO_Pin_9, 0, ADC_CHANNEL_X}, /* PD9 */
    {GPIOD, NULL, NULL, GPIO_Pin_10, 0, ADC_CHANNEL_X}, /* PD10 */
    {GPIOD, NULL, NULL, GPIO_Pin_11, 0, ADC_CHANNEL_X}, /* PD11 */
    {GPIOD, TIM4, NULL, GPIO_Pin_12, 1, ADC_CHANNEL_X}, /* PD12 */
    {GPIOD, TIM4, NULL, GPIO_Pin_13, 2, ADC_CHANNEL_X}, /* PD13 */
    {GPIOD, TIM4, NULL, GPIO_Pin_14, 3, ADC_CHANNEL_X}, /* PD14 */
    {GPIOD, TIM4, NULL, GPIO_Pin_15, 4, ADC_CHANNEL_X}, /* PD15 */

    {GPIOE, NULL, NULL,  GPIO_Pin_0, 0, ADC_CHANNEL_X}, /* PE0 */
    {GPIOE, TIM20,NULL,  GPIO_Pin_1, 4, ADC_CHANNEL_X}, /* PE1 */
    {GPIOE, TIM20,NULL,  GPIO_Pin_2, 1, ADC_CHANNEL_X}, /* PE2 */
    {GPIOE, TIM20,NULL,  GPIO_Pin_3, 2, ADC_CHANNEL_X}, /* PE3 */
    {GPIOE, TIM3, NULL,  GPIO_Pin_4, 2, ADC_CHANNEL_X}, /* PE4 */
    {GPIOE, TIM9, NULL,  GPIO_Pin_5, 1, ADC_CHANNEL_X}, /* PE5 */
    {GPIOE, TIM9, NULL,  GPIO_Pin_6, 2, ADC_CHANNEL_X}, /* PE6 */
    {GPIOE, NULL, NULL,  GPIO_Pin_7, 0, ADC_CHANNEL_X}, /* PE7 */
    {GPIOE, NULL, NULL,  GPIO_Pin_8, 0, ADC_CHANNEL_X}, /* PE8 */
    {GPIOE, TIM1, NULL,  GPIO_Pin_9, 1, ADC_CHANNEL_X}, /* PE9 */
    {GPIOE, NULL, NULL, GPIO_Pin_10, 0, ADC_CHANNEL_X}, /* PE10 */
    {GPIOE, TIM1, NULL, GPIO_Pin_11, 2, ADC_CHANNEL_X}, /* PE11 */
    {GPIOE, NULL, NULL, GPIO_Pin_12, 0, ADC_CHANNEL_X}, /* PE12 */
    {GPIOE, TIM1, NULL, GPIO_Pin_13, 3, ADC_CHANNEL_X}, /* PE13 */
    {GPIOE, TIM1, NULL, GPIO_Pin_14, 4, ADC_CHANNEL_X}, /* PE14 */
    {GPIOE, NULL, NULL, GPIO_Pin_15, 0, ADC_CHANNEL_X}, /* PE15 */

    {GPIOF, NULL, NULL,  GPIO_Pin_0, 0, ADC_CHANNEL_X}, /* PF0 */
    {GPIOF, NULL, NULL,  GPIO_Pin_1, 0, ADC_CHANNEL_X}, /* PF1 */
    {GPIOF, TIM20,NULL,  GPIO_Pin_2, 3, ADC_CHANNEL_X}, /* PF2 */
    {GPIOF, TIM20,ADC3,  GPIO_Pin_3, 4, ADC_CHANNEL_9}, /* PF3 */
    {GPIOF, NULL, ADC3,  GPIO_Pin_4, 0, ADC_CHANNEL_14},/* PF4 */
    {GPIOF, NULL, ADC3,  GPIO_Pin_5, 0, ADC_CHANNEL_15},/* PF5 */
    {GPIOF, TIM10,ADC3,  GPIO_Pin_6, 1, ADC_CHANNEL_4}, /* PF6 */
    {GPIOF, TIM11,ADC3,  GPIO_Pin_7, 1, ADC_CHANNEL_5}, /* PF7 */
    {GPIOF, TIM13,ADC3,  GPIO_Pin_8, 1, ADC_CHANNEL_6}, /* PF8 */
    {GPIOF, TIM14,ADC3,  GPIO_Pin_9, 1, ADC_CHANNEL_7}, /* PF9 */
    {GPIOF, NULL, ADC3, GPIO_Pin_10, 0, ADC_CHANNEL_8}, /* PF10 */
    {GPIOF, NULL, NULL, GPIO_Pin_11, 0, ADC_CHANNEL_X}, /* PF11 */
    {GPIOF, TIM20,NULL, GPIO_Pin_12, 1, ADC_CHANNEL_X}, /* PF12 */
    {GPIOF, TIM20,NULL, GPIO_Pin_13, 2, ADC_CHANNEL_X}, /* PF13 */
    {GPIOF, TIM20,NULL, GPIO_Pin_14, 3, ADC_CHANNEL_X}, /* PF14 */
    {GPIOF, TIM20,NULL, GPIO_Pin_15, 4, ADC_CHANNEL_X}, /* PF15 */

    {GPIOG, NULL, NULL,  GPIO_Pin_0, 0, ADC_CHANNEL_X}, /* PG0 */
    {GPIOG, NULL, NULL,  GPIO_Pin_1, 0, ADC_CHANNEL_X}, /* PG1 */
    {GPIOG, NULL, NULL,  GPIO_Pin_2, 0, ADC_CHANNEL_X}, /* PG2 */
    {GPIOG, NULL, NULL,  GPIO_Pin_3, 0, ADC_CHANNEL_X}, /* PG3 */
    {GPIOG, NULL, NULL,  GPIO_Pin_4, 0, ADC_CHANNEL_X}, /* PG4 */
    {GPIOG, NULL, NULL,  GPIO_Pin_5, 0, ADC_CHANNEL_X}, /* PG5 */
    {GPIOG, NULL, NULL,  GPIO_Pin_6, 0, ADC_CHANNEL_X}, /* PG6 */
    {GPIOG, NULL, NULL,  GPIO_Pin_7, 0, ADC_CHANNEL_X}, /* PG7 */
    {GPIOG, NULL, NULL,  GPIO_Pin_8, 0, ADC_CHANNEL_X}, /* PG8 */
    {GPIOG, NULL, NULL,  GPIO_Pin_9, 0, ADC_CHANNEL_X}, /* PG9 */
    {GPIOG, NULL, NULL, GPIO_Pin_10, 0, ADC_CHANNEL_X}, /* PG10 */
    {GPIOG, NULL, NULL, GPIO_Pin_11, 0, ADC_CHANNEL_X}, /* PG11 */
    {GPIOG, NULL, NULL, GPIO_Pin_12, 0, ADC_CHANNEL_X}, /* PG12 */
    {GPIOG, NULL, NULL, GPIO_Pin_13, 0, ADC_CHANNEL_X}, /* PG13 */
    {GPIOG, NULL, NULL, GPIO_Pin_14, 0, ADC_CHANNEL_X}, /* PG14 */
    {GPIOG, NULL, NULL, GPIO_Pin_15, 0, ADC_CHANNEL_X}, /* PG15 */

    {GPIOH, NULL, NULL,  GPIO_Pin_0, 0, ADC_CHANNEL_X}, /* PH0 */
    {GPIOH, NULL, NULL,  GPIO_Pin_1, 0, ADC_CHANNEL_X}, /* PH1 */
    {GPIOH, NULL, NULL,  GPIO_Pin_2, 0, ADC_CHANNEL_X}, /* PH2 */
    {GPIOH, NULL, NULL,  GPIO_Pin_3, 0, ADC_CHANNEL_X}, /* PH3 */
    {GPIOH, NULL, NULL,  GPIO_Pin_4, 0, ADC_CHANNEL_X}, /* PH4 */
    {GPIOH, NULL, NULL,  GPIO_Pin_5, 0, ADC_CHANNEL_X}, /* PH5 */
    {GPIOH, NULL, NULL,  GPIO_Pin_6, 0, ADC_CHANNEL_X}, /* PH6 */
    {GPIOH, NULL, NULL,  GPIO_Pin_7, 0, ADC_CHANNEL_X}, /* PH7 */
    {GPIOH, NULL, NULL,  GPIO_Pin_8, 0, ADC_CHANNEL_X}, /* PH8 */
    {GPIOH, NULL, NULL,  GPIO_Pin_9, 0, ADC_CHANNEL_X}, /* PH9 */
    {GPIOH, NULL, NULL, GPIO_Pin_10, 0, ADC_CHANNEL_X}, /* PH10 */
    {GPIOH, NULL, NULL, GPIO_Pin_11, 0, ADC_CHANNEL_X}, /* PH11 */
    {GPIOH, NULL, NULL, GPIO_Pin_12, 0, ADC_CHANNEL_X}, /* PH12 */
    {GPIOH, NULL, NULL, GPIO_Pin_13, 0, ADC_CHANNEL_X}, /* PH13 */
    {GPIOH, NULL, NULL, GPIO_Pin_14, 0, ADC_CHANNEL_X}, /* PH14 */
    {GPIOH, NULL, NULL, GPIO_Pin_15, 0, ADC_CHANNEL_X}, /* PH15 */
};

/**
  * @brief  GPIO初始化
  * @param  GPIOx: GPIO地址
  * @param  GPIO_Pin_x: GPIO对应位
  * @param  GPIO_Mode_x: GPIO模式
  * @param  GPIO_Drive_x: GPIO速度
  * @retval 无
  */
void GPIOx_Init(
    gpio_type* GPIOx,
    uint16_t GPIO_Pin_x,
    PinMode_TypeDef Mode,
    gpio_drive_type GPIO_Drive_x
)
{
    gpio_init_type gpio_init_struct;
    crm_periph_clock_type CRM_GPIOx_PERIPH_CLOCK;

    if(GPIOx == GPIOA)     CRM_GPIOx_PERIPH_CLOCK = CRM_GPIOA_PERIPH_CLOCK;
    else if(GPIOx == GPIOB)CRM_GPIOx_PERIPH_CLOCK = CRM_GPIOB_PERIPH_CLOCK;
    else if(GPIOx == GPIOC)CRM_GPIOx_PERIPH_CLOCK = CRM_GPIOC_PERIPH_CLOCK;
    else if(GPIOx == GPIOD)CRM_GPIOx_PERIPH_CLOCK = CRM_GPIOD_PERIPH_CLOCK;
    else if(GPIOx == GPIOE)CRM_GPIOx_PERIPH_CLOCK = CRM_GPIOE_PERIPH_CLOCK;
    else if(GPIOx == GPIOF)CRM_GPIOx_PERIPH_CLOCK = CRM_GPIOF_PERIPH_CLOCK;
    else if(GPIOx == GPIOG)CRM_GPIOx_PERIPH_CLOCK = CRM_GPIOG_PERIPH_CLOCK;
    else if(GPIOx == GPIOH)CRM_GPIOx_PERIPH_CLOCK = CRM_GPIOH_PERIPH_CLOCK;
    else return;

    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_pins = GPIO_Pin_x;
    gpio_init_struct.gpio_drive_strength = GPIO_Drive_x;

    if(Mode == INPUT)
    {
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    }
    else if(Mode == INPUT_PULLUP)
    {
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    }
    else if(Mode == INPUT_PULLDOWN)
    {
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
    }
    else if(Mode == INPUT_ANALOG)
    {
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    }
    else if(Mode == OUTPUT)
    {
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    }
    else if(Mode == OUTPUT_OPEN_DRAIN)
    {
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
    }
    else if(Mode == OUTPUT_AF_PP)
    {
        gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    }
    else if(Mode == OUTPUT_AF_OD)
    {
        gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
    }
    else
    {
        return;
    }

    crm_periph_clock_enable(CRM_GPIOx_PERIPH_CLOCK, TRUE);
    gpio_init(GPIOx, &gpio_init_struct);
}


/**
  * @brief  获取当前引脚对应的GPIOx编号
  * @param  Pin: 引脚编号
  * @retval 无
  */
scfg_port_source_type GPIO_GetPortNum(uint8_t Pin)
{
    uint8_t retval = 0xFF;
    uint8_t index;
    gpio_type* GPIOx = PIN_MAP[Pin].GPIOx;

    static const gpio_type* GPIO_Map[] =
    {
        GPIOA,
        GPIOB,
        GPIOC,
        GPIOD,
        GPIOE,
        GPIOF,
        GPIOG,
        GPIOH
    };

    for(index = 0; index < sizeof(GPIO_Map) / sizeof(GPIO_Map[0]); index++)
    {
        if(GPIOx == GPIO_Map[index])
        {
            retval = index;
            break;
        }
    }

    return (scfg_port_source_type)retval;
}

/**
  * @brief  获取当前引脚对应的 PinSource
  * @param  GPIO_Pin_x: GPIO对应位
  * @retval 无
  */
gpio_pins_source_type GPIO_GetPinSource(uint16_t GPIO_Pin_x)
{
    uint8_t PinSource = 0;
    while(GPIO_Pin_x > 1)
    {
        GPIO_Pin_x >>= 1;
        PinSource++;
    }
    return (gpio_pins_source_type)PinSource;
}

/**
  * @brief  获取当前引脚对应的编号
  * @param  Pin: 引脚编号
  * @retval 无
  */
uint8_t GPIO_GetPinNum(uint8_t Pin)
{
    return GPIO_GetPinSource(PIN_MAP[Pin].GPIO_Pin_x);
}
