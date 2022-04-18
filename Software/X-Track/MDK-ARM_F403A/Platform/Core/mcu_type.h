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
#ifndef __MCU_TYPE_H
#define __MCU_TYPE_H

#include "at32f4xx.h"
#include "at32f4xx_conf.h"
#include "mcu_config.h"

#define __AT32__
#define __AT32F4__
#define __STM32__
#define __STM32F4__
#define F_CPU                       SystemCoreClock
#define CYCLES_PER_MICROSECOND      (F_CPU / 1000000U)

typedef GPIO_Type                   GPIO_TypeDef;
#define GPIO_Pin_0                  GPIO_Pins_0
#define GPIO_Pin_1                  GPIO_Pins_1
#define GPIO_Pin_2                  GPIO_Pins_2
#define GPIO_Pin_3                  GPIO_Pins_3
#define GPIO_Pin_4                  GPIO_Pins_4
#define GPIO_Pin_5                  GPIO_Pins_5
#define GPIO_Pin_6                  GPIO_Pins_6
#define GPIO_Pin_7                  GPIO_Pins_7
#define GPIO_Pin_8                  GPIO_Pins_8
#define GPIO_Pin_9                  GPIO_Pins_9
#define GPIO_Pin_10                 GPIO_Pins_10
#define GPIO_Pin_11                 GPIO_Pins_11
#define GPIO_Pin_12                 GPIO_Pins_12
#define GPIO_Pin_13                 GPIO_Pins_13
#define GPIO_Pin_14                 GPIO_Pins_14
#define GPIO_Pin_15                 GPIO_Pins_15
#define GPIO_Pin_All                GPIO_Pins_All

typedef TMR_Type                    TIM_TypeDef;
#define TIM1                        TMR1
#define TIM2                        TMR2
#define TIM3                        TMR3
#define TIM4                        TMR4
#define TIM5                        TMR5
#define TIM6                        TMR6
#define TIM7                        TMR7
#define TIM8                        TMR8
#define TIM9                        TMR9
#define TIM10                       TMR10
#define TIM11                       TMR11
#define TIM12                       TMR12
#define TIM13                       TMR13
#define TIM14                       TMR14
#define TIM15                       TMR15

typedef SPI_Type                    SPI_TypeDef;

#endif
