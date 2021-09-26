/*
 * MIT License
 * Copyright (c) 2019 _VIFEXTech
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
    /*GPIO_TypeDef* GPIOx;  //对应GPIOx地址
    TIM_TypeDef* TIMx;      //对应TIMx地址
    ADC_TypeDef* ADCx;      //对应ADCx地址

    uint16_t GPIO_Pin_x;    //对应GPIO_Pin位
    uint8_t TimerChannel;   //对应定时器通道
    uint8_t ADC_Channel;*/  //对应ADC通道
    {GPIOA, TIM2, ADC1,  GPIO_Pin_0, 1, ADC_Channel_0}, /* PA0 */
    {GPIOA, TIM2, ADC1,  GPIO_Pin_1, 2, ADC_Channel_1}, /* PA1 */
    {GPIOA, TIM2, ADC1,  GPIO_Pin_2, 3, ADC_Channel_2}, /* PA2 */
    {GPIOA, TIM2, ADC1,  GPIO_Pin_3, 4, ADC_Channel_3}, /* PA3 */
    {GPIOA, NULL, ADC1,  GPIO_Pin_4, 0, ADC_Channel_4}, /* PA4 */
    {GPIOA, NULL, ADC1,  GPIO_Pin_5, 0, ADC_Channel_5}, /* PA5 */
    {GPIOA, TIM3, ADC1,  GPIO_Pin_6, 1, ADC_Channel_6}, /* PA6 */
    {GPIOA, TIM3, ADC1,  GPIO_Pin_7, 2, ADC_Channel_7}, /* PA7 */
    {GPIOA, TIM1, NULL,  GPIO_Pin_8, 1, ADC_Channel_X}, /* PA8 */
    {GPIOA, TIM1, NULL,  GPIO_Pin_9, 2, ADC_Channel_X}, /* PA9 */
    {GPIOA, TIM1, NULL, GPIO_Pin_10, 3, ADC_Channel_X}, /* PA10 */
    {GPIOA, TIM1, NULL, GPIO_Pin_11, 4, ADC_Channel_X}, /* PA11 */
    {GPIOA, NULL, NULL, GPIO_Pin_12, 0, ADC_Channel_X}, /* PA12 */
    {GPIOA, NULL, NULL, GPIO_Pin_13, 0, ADC_Channel_X}, /* PA13 */
    {GPIOA, NULL, NULL, GPIO_Pin_14, 0, ADC_Channel_X}, /* PA14 */
    {GPIOA, TIM2, NULL, GPIO_Pin_15, 1, ADC_Channel_X}, /* PA15 */

    {GPIOB, TIM3, ADC1,  GPIO_Pin_0, 3, ADC_Channel_8}, /* PB0 */
    {GPIOB, TIM3, ADC1,  GPIO_Pin_1, 4, ADC_Channel_9}, /* PB1 */
    {GPIOB, NULL, NULL,  GPIO_Pin_2, 0, ADC_Channel_X}, /* PB2 */
    {GPIOB, TIM2, NULL,  GPIO_Pin_3, 2, ADC_Channel_X}, /* PB3 */
    {GPIOB, TIM3, NULL,  GPIO_Pin_4, 1, ADC_Channel_X}, /* PB4 */
    {GPIOB, TIM3, NULL,  GPIO_Pin_5, 2, ADC_Channel_X}, /* PB5 */
    {GPIOB, TIM4, NULL,  GPIO_Pin_6, 1, ADC_Channel_X}, /* PB6 */
    {GPIOB, TIM4, NULL,  GPIO_Pin_7, 2, ADC_Channel_X}, /* PB7 */
    {GPIOB, TIM4, NULL,  GPIO_Pin_8, 3, ADC_Channel_X}, /* PB8 */
    {GPIOB, TIM4, NULL,  GPIO_Pin_9, 4, ADC_Channel_X}, /* PB9 */
    {GPIOB, TIM2, NULL, GPIO_Pin_10, 3, ADC_Channel_X}, /* PB10 */
    {GPIOB, TIM2, NULL, GPIO_Pin_11, 4, ADC_Channel_X}, /* PB11 */
    {GPIOB, NULL, NULL, GPIO_Pin_12, 0, ADC_Channel_X}, /* PB12 */
    {GPIOB, NULL, NULL, GPIO_Pin_13, 0, ADC_Channel_X}, /* PB13 */
#ifdef TIM12
    {GPIOB, TIM12, NULL, GPIO_Pin_14, 1, ADC_Channel_X},/* PB14 */
#else
    {GPIOB, NULL, NULL, GPIO_Pin_14, 0, ADC_Channel_X},/* PB14 */
#endif
    {GPIOB, NULL, NULL, GPIO_Pin_15, 0, ADC_Channel_X}, /* PB15 */

    {GPIOC, NULL, ADC1,  GPIO_Pin_0, 0, ADC_Channel_10},/* PC0 */
    {GPIOC, NULL, ADC1,  GPIO_Pin_1, 0, ADC_Channel_11},/* PC1 */
    {GPIOC, NULL, ADC1,  GPIO_Pin_2, 0, ADC_Channel_12},/* PC2 */
    {GPIOC, NULL, ADC1,  GPIO_Pin_3, 0, ADC_Channel_13},/* PC3 */
    {GPIOC, NULL, ADC1,  GPIO_Pin_4, 0, ADC_Channel_14},/* PC4 */
    {GPIOC, NULL, ADC1,  GPIO_Pin_5, 0, ADC_Channel_15},/* PC5 */
    {GPIOC, TIM3, NULL,  GPIO_Pin_6, 1, ADC_Channel_X}, /* PC6 */
    {GPIOC, TIM3, NULL,  GPIO_Pin_7, 2, ADC_Channel_X}, /* PC7 */
    {GPIOC, TIM3, NULL,  GPIO_Pin_8, 3, ADC_Channel_X}, /* PC8 */
    {GPIOC, TIM3, NULL,  GPIO_Pin_9, 4, ADC_Channel_X}, /* PC9 */
    {GPIOC, NULL, NULL, GPIO_Pin_10, 0, ADC_Channel_X}, /* PC10 */
    {GPIOC, NULL, NULL, GPIO_Pin_11, 0, ADC_Channel_X}, /* PC11 */
    {GPIOC, NULL, NULL, GPIO_Pin_12, 0, ADC_Channel_X}, /* PC12 */
    {GPIOC, NULL, NULL, GPIO_Pin_13, 0, ADC_Channel_X}, /* PC13 */
    {GPIOC, NULL, NULL, GPIO_Pin_14, 0, ADC_Channel_X}, /* PC14 */
    {GPIOC, NULL, NULL, GPIO_Pin_15, 0, ADC_Channel_X}, /* PC15 */

    {GPIOD, NULL, NULL,  GPIO_Pin_0, 0, ADC_Channel_X}, /* PD0 */
    {GPIOD, NULL, NULL,  GPIO_Pin_1, 0, ADC_Channel_X}, /* PD1 */
    {GPIOD, NULL, NULL,  GPIO_Pin_2, 0, ADC_Channel_X}, /* PD2 */
    {GPIOD, NULL, NULL,  GPIO_Pin_3, 0, ADC_Channel_X}, /* PD3 */
    {GPIOD, NULL, NULL,  GPIO_Pin_4, 0, ADC_Channel_X}, /* PD4 */
    {GPIOD, NULL, NULL,  GPIO_Pin_5, 0, ADC_Channel_X}, /* PD5 */
    {GPIOD, NULL, NULL,  GPIO_Pin_6, 0, ADC_Channel_X}, /* PD6 */
    {GPIOD, NULL, NULL,  GPIO_Pin_7, 0, ADC_Channel_X}, /* PD7 */
    {GPIOD, NULL, NULL,  GPIO_Pin_8, 0, ADC_Channel_X}, /* PD8 */
    {GPIOD, NULL, NULL,  GPIO_Pin_9, 0, ADC_Channel_X}, /* PD9 */
    {GPIOD, NULL, NULL, GPIO_Pin_10, 0, ADC_Channel_X}, /* PD10 */
    {GPIOD, NULL, NULL, GPIO_Pin_11, 0, ADC_Channel_X}, /* PD11 */
    {GPIOD, TIM4, NULL, GPIO_Pin_12, 1, ADC_Channel_X}, /* PD12 */
    {GPIOD, TIM4, NULL, GPIO_Pin_13, 2, ADC_Channel_X}, /* PD13 */
    {GPIOD, TIM4, NULL, GPIO_Pin_14, 3, ADC_Channel_X}, /* PD14 */
    {GPIOD, TIM4, NULL, GPIO_Pin_15, 4, ADC_Channel_X}, /* PD15 */

#ifdef GPIOE
    {GPIOE, NULL, NULL,  GPIO_Pin_0, 0, ADC_Channel_X}, /* PE0 */
    {GPIOE, NULL, NULL,  GPIO_Pin_1, 0, ADC_Channel_X}, /* PE1 */
    {GPIOE, NULL, NULL,  GPIO_Pin_2, 0, ADC_Channel_X}, /* PE2 */
    {GPIOE, NULL, NULL,  GPIO_Pin_3, 0, ADC_Channel_X}, /* PE3 */
    {GPIOE, NULL, NULL,  GPIO_Pin_4, 0, ADC_Channel_X}, /* PE4 */
    {GPIOE, TIM9, NULL,  GPIO_Pin_5, 1, ADC_Channel_X}, /* PE5 */
    {GPIOE, TIM9, NULL,  GPIO_Pin_6, 2, ADC_Channel_X}, /* PE6 */
    {GPIOE, NULL, NULL,  GPIO_Pin_7, 0, ADC_Channel_X}, /* PE7 */
    {GPIOE, NULL, NULL,  GPIO_Pin_8, 0, ADC_Channel_X}, /* PE8 */
    {GPIOE, TIM1, NULL,  GPIO_Pin_9, 1, ADC_Channel_X}, /* PE9 */
    {GPIOE, NULL, NULL, GPIO_Pin_10, 0, ADC_Channel_X}, /* PE10 */
    {GPIOE, TIM1, NULL, GPIO_Pin_11, 2, ADC_Channel_X}, /* PE11 */
    {GPIOE, NULL, NULL, GPIO_Pin_12, 0, ADC_Channel_X}, /* PE12 */
    {GPIOE, TIM1, NULL, GPIO_Pin_13, 3, ADC_Channel_X}, /* PE13 */
    {GPIOE, TIM1, NULL, GPIO_Pin_14, 4, ADC_Channel_X}, /* PE14 */
    {GPIOE, NULL, NULL, GPIO_Pin_15, 0, ADC_Channel_X}, /* PE15 */
#endif /*GPIOE*/

#ifdef GPIOF
    {GPIOF, NULL, NULL,  GPIO_Pin_0, 0, ADC_Channel_X}, /* PF0 */
    {GPIOF, NULL, NULL,  GPIO_Pin_1, 0, ADC_Channel_X}, /* PF1 */
    {GPIOF, NULL, NULL,  GPIO_Pin_2, 0, ADC_Channel_X}, /* PF2 */
#ifdef ADC3
    {GPIOF, NULL, ADC3,  GPIO_Pin_3, 0, ADC_Channel_9}, /* PF3 */
    {GPIOF, NULL, ADC3,  GPIO_Pin_4, 0, ADC_Channel_14},/* PF4 */
    {GPIOF, NULL, ADC3,  GPIO_Pin_5, 0, ADC_Channel_15},/* PF5 */
    {GPIOF, TIM10, ADC3,  GPIO_Pin_6, 1, ADC_Channel_4}, /* PF6 */
    {GPIOF, TIM11, ADC3,  GPIO_Pin_7, 1, ADC_Channel_5}, /* PF7 */
    {GPIOF, TIM13, ADC3,  GPIO_Pin_8, 1, ADC_Channel_6}, /* PF8 */
    {GPIOF, TIM14, ADC3,  GPIO_Pin_9, 1, ADC_Channel_7}, /* PF9 */
    {GPIOF, NULL, ADC3, GPIO_Pin_10, 0, ADC_Channel_8}, /* PF10 */
#else
    {GPIOF, NULL, NULL,  GPIO_Pin_3, 0, ADC_Channel_X}, /* PF3 */
    {GPIOF, NULL, NULL,  GPIO_Pin_4, 0, ADC_Channel_X},/* PF4 */
    {GPIOF, NULL, NULL,  GPIO_Pin_5, 0, ADC_Channel_X},/* PF5 */
    {GPIOF, TIM10, NULL,  GPIO_Pin_6, 1, ADC_Channel_X}, /* PF6 */
    {GPIOF, TIM11, NULL,  GPIO_Pin_7, 1, ADC_Channel_X}, /* PF7 */
    {GPIOF, NULL, NULL,  GPIO_Pin_8, 0, ADC_Channel_X}, /* PF8 */
    {GPIOF, NULL, NULL,  GPIO_Pin_9, 0, ADC_Channel_X}, /* PF9 */
    {GPIOF, NULL, NULL, GPIO_Pin_10, 0, ADC_Channel_X}, /* PF10 */
#endif /*ADC3*/
    {GPIOF, NULL, NULL, GPIO_Pin_11, 0, ADC_Channel_X}, /* PF11 */
    {GPIOF, NULL, NULL, GPIO_Pin_12, 0, ADC_Channel_X}, /* PF12 */
    {GPIOF, NULL, NULL, GPIO_Pin_13, 0, ADC_Channel_X}, /* PF13 */
    {GPIOF, NULL, NULL, GPIO_Pin_14, 0, ADC_Channel_X}, /* PF14 */
    {GPIOF, NULL, NULL, GPIO_Pin_15, 0, ADC_Channel_X}, /* PF15 */
#endif /*GPIOF*/

#ifdef GPIOG
    {GPIOG, NULL, NULL,  GPIO_Pin_0, 0, ADC_Channel_X}, /* PG0 */
    {GPIOG, NULL, NULL,  GPIO_Pin_1, 0, ADC_Channel_X}, /* PG1 */
    {GPIOG, NULL, NULL,  GPIO_Pin_2, 0, ADC_Channel_X}, /* PG2 */
    {GPIOG, NULL, NULL,  GPIO_Pin_3, 0, ADC_Channel_X}, /* PG3 */
    {GPIOG, NULL, NULL,  GPIO_Pin_4, 0, ADC_Channel_X}, /* PG4 */
    {GPIOG, NULL, NULL,  GPIO_Pin_5, 0, ADC_Channel_X}, /* PG5 */
    {GPIOG, NULL, NULL,  GPIO_Pin_6, 0, ADC_Channel_X}, /* PG6 */
    {GPIOG, NULL, NULL,  GPIO_Pin_7, 0, ADC_Channel_X}, /* PG7 */
    {GPIOG, NULL, NULL,  GPIO_Pin_8, 0, ADC_Channel_X}, /* PG8 */
    {GPIOG, NULL, NULL,  GPIO_Pin_9, 0, ADC_Channel_X}, /* PG9 */
    {GPIOG, NULL, NULL, GPIO_Pin_10, 0, ADC_Channel_X}, /* PG10 */
    {GPIOG, NULL, NULL, GPIO_Pin_11, 0, ADC_Channel_X}, /* PG11 */
    {GPIOG, NULL, NULL, GPIO_Pin_12, 0, ADC_Channel_X}, /* PG12 */
    {GPIOG, NULL, NULL, GPIO_Pin_13, 0, ADC_Channel_X}, /* PG13 */
    {GPIOG, NULL, NULL, GPIO_Pin_14, 0, ADC_Channel_X}, /* PG14 */
    {GPIOG, NULL, NULL, GPIO_Pin_15, 0, ADC_Channel_X}, /* PG15 */
#endif /*GPIOG*/
};

/**
  * @brief  GPIO初始化
  * @param  GPIOx: GPIO地址
  * @param  GPIO_Pin_x: GPIO对应位
  * @param  GPIO_Mode_x: GPIO模式
  * @param  GPIO_Speed_x: GPIO速度
  * @retval 无
  */
void GPIOx_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_x, pinMode_TypeDef pinMode_x, GPIOSpeed_TypeDef GPIO_Speed_x)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint32_t RCC_AHB1Periph_GPIOx;
    GPIOMode_TypeDef GPIO_Mode_x;
    GPIOOType_TypeDef GPIO_OType_x = 0;
    GPIOPuPd_TypeDef GPIO_PuPd_x;

    if(GPIOx == GPIOA)RCC_AHB1Periph_GPIOx = RCC_AHB1Periph_GPIOA;
    else if(GPIOx == GPIOB)RCC_AHB1Periph_GPIOx = RCC_AHB1Periph_GPIOB;
    else if(GPIOx == GPIOC)RCC_AHB1Periph_GPIOx = RCC_AHB1Periph_GPIOC;
    else if(GPIOx == GPIOD)RCC_AHB1Periph_GPIOx = RCC_AHB1Periph_GPIOD;
#ifdef GPIOE
    else if(GPIOx == GPIOE)RCC_AHB1Periph_GPIOx = RCC_AHB1Periph_GPIOE;
#endif
#ifdef GPIOF
    else if(GPIOx == GPIOF)RCC_AHB1Periph_GPIOx = RCC_AHB1Periph_GPIOF;
#endif
#ifdef GPIOG
    else if(GPIOx == GPIOG)RCC_AHB1Periph_GPIOx = RCC_AHB1Periph_GPIOG;
#endif
    else return;

    if(pinMode_x == INPUT)
    {
        GPIO_Mode_x  = GPIO_Mode_IN;
        GPIO_PuPd_x  = GPIO_PuPd_NOPULL;
    }
    else if(pinMode_x == INPUT_PULLUP)
    {
        GPIO_Mode_x  = GPIO_Mode_IN;
        GPIO_PuPd_x  = GPIO_PuPd_UP;
    }
    else if(pinMode_x == INPUT_PULLDOWN)
    {
        GPIO_Mode_x  = GPIO_Mode_IN;
        GPIO_PuPd_x  = GPIO_PuPd_DOWN;
    }
    else if(pinMode_x == INPUT_ANALOG)
    {
        GPIO_Mode_x  = GPIO_Mode_AN;
        GPIO_PuPd_x  = GPIO_PuPd_NOPULL;
    }
    else if(pinMode_x == OUTPUT)
    {
        GPIO_Mode_x  = GPIO_Mode_OUT;
        GPIO_OType_x = GPIO_OType_PP;
        GPIO_PuPd_x  = GPIO_PuPd_NOPULL;
    }
    else if(pinMode_x == OUTPUT_OPEN_DRAIN)
    {
        GPIO_Mode_x  = GPIO_Mode_OUT;
        GPIO_OType_x = GPIO_OType_OD;
        GPIO_PuPd_x  = GPIO_PuPd_NOPULL;
    }
    else if(pinMode_x == OUTPUT_AF_PP)
    {
        GPIO_Mode_x  = GPIO_Mode_AF;
        GPIO_OType_x = GPIO_OType_PP;
        GPIO_PuPd_x  = GPIO_PuPd_NOPULL;
    }
    else if(pinMode_x == OUTPUT_AF_OD)
    {
        GPIO_Mode_x  = GPIO_Mode_AF;
        GPIO_OType_x = GPIO_OType_OD;
        GPIO_PuPd_x  = GPIO_PuPd_NOPULL;
    }
    else
    {
        return;
    }

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_x;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_x;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_x;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_x;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);
    GPIO_Init(GPIOx, &GPIO_InitStructure);
}

/**
  * @brief  禁用JTAG引脚
  * @param  无
  * @retval 无
  */
void GPIO_JTAG_Disable(void)
{
#warning "Implement GPIO_JTAG_Disable for STM32F4!"
#if 0
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
    GPIO_PinsRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
#endif
}

/**
  * @brief  获取当前引脚对应的GPIOx编号
  * @param  Pin: 引脚编号
  * @retval 无
  */
uint8_t GPIO_GetPortNum(uint8_t Pin)
{
    if(PIN_MAP[Pin].GPIOx == GPIOA)return 0;
    else if(PIN_MAP[Pin].GPIOx == GPIOB)return 1;
    else if(PIN_MAP[Pin].GPIOx == GPIOC)return 2;
    else if(PIN_MAP[Pin].GPIOx == GPIOD)return 3;
#ifdef GPIOE
    else if(PIN_MAP[Pin].GPIOx == GPIOE)return 4;
#endif /*GPIOE*/
#ifdef GPIOF
    else if(PIN_MAP[Pin].GPIOx == GPIOF)return 5;
#endif /*GPIOF*/
#ifdef GPIOG
    else if(PIN_MAP[Pin].GPIOx == GPIOG)return 6;
#endif /*GPIOG*/
    else return 0xFF;
}

/**
  * @brief  获取当前引脚对应的 PinSource
  * @param  GPIO_Pin_x: GPIO对应位
  * @retval 无
  */
uint8_t GPIO_GetPinSource(uint16_t GPIO_Pin_x)
{
    uint16_t PinSource = 0;
    while(GPIO_Pin_x > 1)
    {
        GPIO_Pin_x >>= 1;
        PinSource++;
    }
    return PinSource;
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
