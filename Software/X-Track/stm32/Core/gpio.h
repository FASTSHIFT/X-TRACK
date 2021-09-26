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
#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_type.h"

#ifndef NULL
#define NULL 0
#endif

#define ADC_Channel_X ((uint8_t)0xFF)

typedef enum {
    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
    PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
    PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
    PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
#ifdef GPIOE
    PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
#endif
#ifdef GPIOF
    PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15,
#endif
#ifdef GPIOG
    PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG7, PG8, PG9, PG10, PG11, PG12, PG13, PG14, PG15,
#endif
    PIN_MAX
} Pin_TypeDef;

typedef struct STM32_PinInfo {
    GPIO_TypeDef* GPIOx;
    TIM_TypeDef* TIMx;
    ADC_TypeDef* ADCx;

    uint16_t GPIO_Pin_x;
    uint8_t TimerChannel;
    uint8_t ADC_Channel;
} PinInfo_TypeDef;

typedef enum
{
    INPUT,
    INPUT_PULLUP,
    INPUT_PULLDOWN,
    INPUT_ANALOG,
    INPUT_ANALOG_DMA,
    OUTPUT,
    OUTPUT_OPEN_DRAIN,
    OUTPUT_AF_OD,
    OUTPUT_AF_PP,
    PWM
} pinMode_TypeDef;

extern const PinInfo_TypeDef PIN_MAP[PIN_MAX];

#define IS_PIN(Pin)     (Pin < PIN_MAX)
#define IS_ADC_PIN(Pin) (IS_PIN(Pin) && PIN_MAP[Pin].ADCx != NULL && PIN_MAP[Pin].ADC_Channel  != ADC_Channel_X)
#define IS_PWM_PIN(Pin) (IS_PIN(Pin) && PIN_MAP[Pin].TIMx != NULL && PIN_MAP[Pin].TimerChannel != 0)

#define GPIO_HIGH(GPIOx,GPIO_Pin_x)     ((GPIOx)->BSRRL = (GPIO_Pin_x))
#define GPIO_LOW(GPIOx,GPIO_Pin_x)      ((GPIOx)->BSRRH = (GPIO_Pin_x))
#define GPIO_READ(GPIOx,GPIO_Pin_x)    (((GPIOx)->IDR   & (GPIO_Pin_x))!=0)
#define GPIO_TOGGLE(GPIOx,GPIO_Pin_x)   ((GPIOx)->ODR  ^= (GPIO_Pin_x))

void GPIOx_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_x, pinMode_TypeDef pinMode_x, GPIOSpeed_TypeDef GPIO_Speed_x);
void GPIO_JTAG_Disable(void);
uint8_t GPIO_GetPortNum(uint8_t Pin);
uint8_t GPIO_GetPinNum(uint8_t Pin);
uint8_t GPIO_GetPinSource(uint16_t GPIO_Pin_x);

#ifdef __cplusplus
}// extern "C"
#endif

#endif
