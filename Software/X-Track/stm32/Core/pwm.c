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

#define TIM_GetPinSourcex(Pin) GPIO_GetPinNum(Pin)

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
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    if(!IS_TIM_ALL_PERIPH(TIMx))
        return;
    
    Timer_ClockCmd(TIMx, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    switch(TimerChannel)
    {
    case 1:
        TIM_OC1Init(TIMx, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
        break;
    case 2:
        TIM_OC2Init(TIMx, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
        break;
    case 3:
        TIM_OC3Init(TIMx, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
        break;
    case 4:
        TIM_OC4Init(TIMx, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
        break;
    }

    TIM_Cmd(TIMx, ENABLE);
    
    if(IS_APB2_TIM(TIMx))
    {
        TIM_CtrlPWMOutputs(TIMx, ENABLE);
    }
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
    GPIO_PinAFConfig(PIN_MAP[Pin].GPIOx, TIM_GetPinSourcex(Pin), TIM_GetGPIO_AF(Pin));

    arr = PWM_DutyCycle;
    psc = F_CPU / PWM_DutyCycle / PWM_Frequency;
    
    if(!IS_APB2_TIM(PIN_MAP[Pin].TIMx))
        psc /= 2;

    TIM_Cmd(PIN_MAP[Pin].TIMx, DISABLE);
    TIMx_OCxInit(PIN_MAP[Pin].TIMx, arr - 1, psc - 1, PIN_MAP[Pin].TimerChannel);
    return PIN_MAP[Pin].TimerChannel;
}

/**
  * @brief  获取引脚对应的定时器复用编号
  * @param  Pin: 引脚编号
  * @retval 定时器复用编号
  */
uint8_t TIM_GetGPIO_AF(uint8_t Pin)
{
    uint8_t GPIO_AF_x = 0;
    TIM_TypeDef* TIMx = PIN_MAP[Pin].TIMx;
    
    if(!IS_TIM_ALL_PERIPH(TIMx))
        return 0;

#define TIMx_GPIO_AF_DEF(n)\
do{\
    if(TIMx == TIM##n)\
    {\
        GPIO_AF_x = GPIO_AF_TIM##n;\
    }\
}while(0)

    TIMx_GPIO_AF_DEF(1);
    TIMx_GPIO_AF_DEF(2);
    TIMx_GPIO_AF_DEF(3);
    TIMx_GPIO_AF_DEF(4);
    TIMx_GPIO_AF_DEF(5);
#ifdef GPIO_AF_TIM6
    TIMx_GPIO_AF_DEF(6);
#endif
#ifdef GPIO_AF_TIM7
    TIMx_GPIO_AF_DEF(7);
#endif
#ifdef GPIO_AF_TIM8
    TIMx_GPIO_AF_DEF(8);
#endif
#ifdef GPIO_AF_TIM9
    TIMx_GPIO_AF_DEF(9);
#endif
#ifdef GPIO_AF_TIM10
    TIMx_GPIO_AF_DEF(10);
#endif
#ifdef GPIO_AF_TIM11
    TIMx_GPIO_AF_DEF(11);
#endif
#ifdef GPIO_AF_TIM12
    TIMx_GPIO_AF_DEF(12);
#endif
#ifdef GPIO_AF_TIM13
    TIMx_GPIO_AF_DEF(13);
#endif
#ifdef GPIO_AF_TIM14
    TIMx_GPIO_AF_DEF(14);
#endif

    return GPIO_AF_x;
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
        PIN_MAP[Pin].TIMx->CCR1 = val;
        break;
    case 2:
        PIN_MAP[Pin].TIMx->CCR2 = val;
        break;
    case 3:
        PIN_MAP[Pin].TIMx->CCR3 = val;
        break;
    case 4:
        PIN_MAP[Pin].TIMx->CCR4 = val;
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
        compare = TIMx->CCR1;
        break;
    case 2:
        compare = TIMx->CCR2;
        break;
    case 3:
        compare = TIMx->CCR3;
        break;
    case 4:
        compare = TIMx->CCR4;
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
    TIMx->PSC = psc;
}

/**
  * @brief  更新定时器自动重装值
  * @param  TIMx: 定时器地址
  * @param  arr: 自动重装值
  * @retval 无
  */
void timer_set_reload(TIM_TypeDef* TIMx, uint16_t arr)
{
    TIMx->ARR = arr;
}

/**
  * @brief  应用定时器更改
  * @param  TIMx: 定时器地址
  * @retval 无
  */
void timer_generate_update(TIM_TypeDef* TIMx)
{
    TIMx->EGR = TIM_PSCReloadMode_Immediate;
}
