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
#include "pwm.h"
#include "timer.h"
#include "Arduino.h"

/**
  * @brief  定时器输出捕获初始化
  * @param  TIMx: 定时器地址
  * @param  arr: 自动重装值
  * @param  psc: 时钟预分频数
  * @param  TimerChannel: 定时器通道
  * @retval 无
  */
void TIMx_OCxInit(TIM_TypeDef* TIMx, uint16_t arr, uint16_t psc, uint8_t TimerChannel)
{
    TMR_TimerBaseInitType TMR_TimeBaseStructure;
    TMR_OCInitType TMR_OCInitStructure;
    
    if(!IS_TMR_ALL_PERIPH(TIMx))
        return;
    
    Timer_ClockCmd(TIMx, ENABLE);

    TMR_TimeBaseStructure.TMR_Period = arr;
    TMR_TimeBaseStructure.TMR_DIV = psc;
    TMR_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1;
    TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
    TMR_TimeBaseInit(TIMx, &TMR_TimeBaseStructure);

    TMR_OCStructInit(&TMR_OCInitStructure);
    TMR_OCInitStructure.TMR_OCMode = TMR_OCMode_PWM2;
    TMR_OCInitStructure.TMR_OCPolarity = TMR_OCPolarity_Low;
    TMR_OCInitStructure.TMR_OCIdleState = TMR_OCIdleState_Reset;
    TMR_OCInitStructure.TMR_OCNPolarity = TMR_OCNPolarity_Low;
    TMR_OCInitStructure.TMR_OCNIdleState = TMR_OCNIdleState_Reset;
    TMR_OCInitStructure.TMR_OutputState = TMR_OutputState_Enable;
    TMR_OCInitStructure.TMR_OutputNState = TMR_OutputNState_Disable;
    TMR_OCInitStructure.TMR_Pulse = 0;
    switch(TimerChannel)
    {
    case 1:
        TMR_OC1Init(TIMx, &TMR_OCInitStructure);
        TMR_OC1PreloadConfig(TIMx, TMR_OCPreload_Enable);
        break;
    case 2:
        TMR_OC2Init(TIMx, &TMR_OCInitStructure);
        TMR_OC2PreloadConfig(TIMx, TMR_OCPreload_Enable);
        break;
    case 3:
        TMR_OC3Init(TIMx, &TMR_OCInitStructure);
        TMR_OC3PreloadConfig(TIMx, TMR_OCPreload_Enable);
        break;
    case 4:
        TMR_OC4Init(TIMx, &TMR_OCInitStructure);
        TMR_OC4PreloadConfig(TIMx, TMR_OCPreload_Enable);
        break;
    }

    TMR_CtrlPWMOutputs(TIMx, ENABLE);
    TMR_Cmd(TIMx, ENABLE);
}

/**
  * @brief  PWM输出初始化
  * @param  Pin:引脚编号
  * @param  PWM_DutyCycle: PWM分级数
  * @param  PWM_Frequency: PWM频率
  * @retval 引脚对应的定时器通道
  */
uint8_t PWM_Init(uint8_t Pin, uint16_t PWM_DutyCycle, uint32_t PWM_Frequency)
{
    uint32_t arr, psc;
    
    if(!IS_PWM_PIN(Pin))
        return 0;
    
    if(PWM_DutyCycle == 0 || PWM_Frequency == 0 || (PWM_DutyCycle * PWM_Frequency) > F_CPU)
        return 0;

    pinMode(Pin, OUTPUT_AF_PP);

    arr = PWM_DutyCycle;
    psc = Timer_GetClockMax(PIN_MAP[Pin].TIMx) / PWM_DutyCycle / PWM_Frequency;

    TMR_Cmd(PIN_MAP[Pin].TIMx, DISABLE);
    TIMx_OCxInit(PIN_MAP[Pin].TIMx, arr - 1, psc - 1, PIN_MAP[Pin].TimerChannel);
    return PIN_MAP[Pin].TimerChannel;
}

/**
  * @brief  输出PWM信号
  * @param  Pin: 引脚编号
  * @param  val:PWM占空比值
  * @retval PWM占空比值
  */
uint16_t pwmWrite(uint8_t Pin, uint16_t val)
{
    switch(PIN_MAP[Pin].TimerChannel)
    {
    case 1:
        PIN_MAP[Pin].TIMx->CC1 = val;
        break;
    case 2:
        PIN_MAP[Pin].TIMx->CC2 = val;
        break;
    case 3:
        PIN_MAP[Pin].TIMx->CC3 = val;
        break;
    case 4:
        PIN_MAP[Pin].TIMx->CC4 = val;
        break;
    }
    return val;
}

/**
  * @brief  获取捕获值
  * @param  TIMx: 定时器地址
  * @param  TimerChannel: 定时器通道
  * @retval 捕获值
  */
uint16_t timer_get_compare(TIM_TypeDef* TIMx, uint8_t TimerChannel)
{
    uint16_t compare = 0;
    switch(TimerChannel)
    {
    case 1:
        compare = TIMx->CC1;
        break;
    case 2:
        compare = TIMx->CC2;
        break;
    case 3:
        compare = TIMx->CC3;
        break;
    case 4:
        compare = TIMx->CC4;
        break;
    }
    return compare;
}

/**
  * @brief  更新定时器时钟预分频数
  * @param  TIMx: 定时器地址
  * @param  psc: 时钟预分频数
  * @retval 无
  */
void timer_set_prescaler(TIM_TypeDef* TIMx, uint16_t psc)
{
    TIMx->DIV = psc;
}

/**
  * @brief  更新定时器自动重装值
  * @param  TIMx: 定时器地址
  * @param  arr: 自动重装值
  * @retval 无
  */
void timer_set_reload(TIM_TypeDef* TIMx, uint16_t arr)
{
    TIMx->AR = arr;
}

/**
  * @brief  应用定时器更改
  * @param  TIMx: 定时器地址
  * @retval 无
  */
void timer_generate_update(TIM_TypeDef* TIMx)
{
    TIMx->EVEG = TMR_DIVReloadMode_Immediate;
}
