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
#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_type.h"
    
#define Timer_PreemptionPriority_Default 0
#define Timer_SubPriority_Default        3
#define Timer_GetClockMax(TIMx)          F_CPU
   
typedef void(*Timer_CallbackFunction_t)(void);
    
void Timer_ClockCmd(TIM_TypeDef* TIMx, FunctionalState NewState);
uint32_t Timer_GetClockOut(TIM_TypeDef* TIMx);
void Timer_SetInterrupt(TIM_TypeDef* TIMx, uint32_t time, Timer_CallbackFunction_t function);
void Timer_SetInterruptTimeUpdate(TIM_TypeDef* TIMx, uint32_t time);
void Timer_SetInterruptFreqUpdate(TIM_TypeDef* TIMx, uint32_t freq);
void Timer_SetInterruptBase(
    TIM_TypeDef* TIMx, 
    uint16_t period, uint16_t prescaler, 
    Timer_CallbackFunction_t function, 
    uint8_t PreemptionPriority, uint8_t SubPriority
);

#define TimerClockCmd(TIMx,EN)                  Timer_ClockCmd(TIMx,EN)
#define TimerSet(TIMx,time,function)            Timer_SetInterrupt(TIMx,time,function)
#define Timer_Init(TIMx,time,function,pre,sub)  Timer_SetInterruptBase(TIMx,0xFF,0xFF,function,pre,sub),Timer_SetInterruptTimeUpdate(TIMx,time)
#define TimerSet_InterruptTimeUpdate(TIMx,time) Timer_SetInterruptTimeUpdate(TIMx,time)

#ifdef __cplusplus
}
#endif

#endif
