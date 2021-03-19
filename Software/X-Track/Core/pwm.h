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
#ifndef __PWM_H
#define __PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_type.h"

uint8_t PWM_Init(uint8_t Pin, uint16_t PWM_DutyCycle, uint32_t PWM_Frequency);
uint16_t pwmWrite(uint8_t Pin, uint16_t val);
void TIMx_OCxInit(TIM_TypeDef* TIMx, uint16_t arr, uint16_t psc, uint8_t TimerChannel);

uint16_t timer_get_compare(TIM_TypeDef* TIMx, uint8_t TimerChannel);
void timer_set_prescaler(TIM_TypeDef* TIMx, uint16_t psc);
void timer_set_reload(TIM_TypeDef* TIMx, uint16_t arr);
void timer_generate_update(TIM_TypeDef* TIMx);

#ifdef __cplusplus
}
#endif

#endif
