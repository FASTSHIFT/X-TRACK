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
static void TIMx_OCxInit(TIM_TypeDef* TIMx, uint32_t arr, uint16_t psc, uint8_t TimerChannel)
{
    TMR_TimerBaseInitType TMR_TimeBaseStructure;
    TMR_OCInitType TMR_OCInitStructure;

    Timer_ClockCmd(TIMx, ENABLE);

    TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
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
    default:
        return;
    }

    TMR_CtrlPWMOutputs(TIMx, ENABLE);
    TMR_Cmd(TIMx, ENABLE);
}

/**
  * @brief  PWM输出初始化
  * @param  Pin:引脚编号
  * @param  Resolution: PWM分辨率
  * @param  Frequency: PWM频率
  * @retval 引脚对应的定时器通道
  */
uint8_t PWM_Init(uint8_t Pin, uint32_t Resolution, uint32_t Frequency)
{
    uint32_t arr, psc;

    if(!IS_PWM_PIN(Pin))
    {
        return 0;
    }

    if(Resolution == 0 || Frequency == 0 || (Resolution * Frequency) > F_CPU)
    {
        return 0;
    }

    pinMode(Pin, OUTPUT_AF_PP);

    arr = Resolution;
    psc = TIMER_GET_CLOCK_MAX(PIN_MAP[Pin].TIMx) / Resolution / Frequency;

    TMR_Cmd(PIN_MAP[Pin].TIMx, DISABLE);
    TIMx_OCxInit(PIN_MAP[Pin].TIMx, arr - 1, psc - 1, PIN_MAP[Pin].TimerChannel);
    return PIN_MAP[Pin].TimerChannel;
}

/**
  * @brief  输出PWM信号
  * @param  Pin: 引脚编号
  * @param  Value: PWM输出值
  * @retval PWM占空比值
  */
void PWM_Write(uint8_t Pin, uint32_t Value)
{
    Timer_SetCompare(PIN_MAP[Pin].TIMx, PIN_MAP[Pin].TimerChannel, Value);
}
