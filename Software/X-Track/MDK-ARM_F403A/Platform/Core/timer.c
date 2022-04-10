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
#include "timer.h"

typedef enum
{
    TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER6, TIMER7, TIMER8,
    TIMER9, TIMER10, TIMER11, TIMER12, TIMER13, TIMER14, TIMER15,
    TIMER_MAX
} TIMER_Type;

static Timer_CallbackFunction_t Timer_CallbackFunction[TIMER_MAX] = { 0 };

/**
  * @brief  启动或关闭指定定时器的时钟
  * @param  TIMx:定时器地址
  * @param  NewState: ENABLE启动，DISABLE关闭
  * @retval 无
  */
void Timer_ClockCmd(TIM_TypeDef* TIMx, FunctionalState NewState)
{
    if(TIMx == TIM1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR1, NewState);
    }
    else if(TIMx == TIM2)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR2, NewState);
    }
    else if(TIMx == TIM3)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR3, NewState);
    }
    else if(TIMx == TIM4)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR4, NewState);
    }
    else if(TIMx == TIM5)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR5, NewState);
    }
#ifdef TMR12
    else if(TIMx == TIM6)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR6, NewState);
    }
#endif
#ifdef TMR7
    else if(TIMx == TIM7)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR7, NewState);
    }
#endif
    else if(TIMx == TIM8)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR8, NewState);
    }
    else if(TIMx == TIM9)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR9, NewState);
    }
    else if(TIMx == TIM10)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR10, NewState);
    }
    else if(TIMx == TIM11)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR11, NewState);
    }
#ifdef TMR12
    else if(TIMx == TIM12)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR12, NewState);
    }
#endif
#ifdef TMR13
    else if(TIMx == TIM13)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR13, NewState);
    }
#endif
#ifdef TMR14
    else if(TIMx == TIM14)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_TMR14, NewState);
    }
#endif
#ifdef TMR15
    else if(TIMx == TIM15)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_TMR15, NewState);
    }
#endif
}

static float Qsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5f;
    x2 = number * 0.5f;
    y  = number;
    i  = *(long*)&y;
    i  = 0x5f3759df - (i >> 1);
    y  = *(float*)&i;
    y  = y * (threehalfs - (x2 * y * y));
    y  = y * (threehalfs - (x2 * y * y));
    return 1.0f / y;
}

/**
  * @brief  频率因数分解，获取接近的值
  * @param  freq: 中断频率(Hz)
  * @param  clock: 定时器时钟
  * @param  *period: 重装值地址
  * @param  *prescaler: 时钟分频值地址
  * @retval 误差值(Hz)
  */
static bool Timer_FreqFactorization(
    uint32_t freq,
    uint32_t clock,
    uint16_t* factor1,
    uint16_t* factor2,
    int32_t* error
)
{
    uint32_t targetProdect;
    uint16_t fct1;
    uint16_t fct2;
    uint16_t fct1_save = 1;
    uint16_t fct2_save = 1;
    uint16_t fct_max;
    uint16_t max_error = 0xFFFF;

    if(freq == 0 || freq > clock)
    {
        return false;
    }

    /* 获取目标乘积 */
    targetProdect = clock / freq;

    /* 从targetProdect的平方根开始计算 */
    fct1 = Qsqrt(targetProdect);

    /* 计算因数最大值，减少遍历次数 */
    fct_max = (targetProdect < 0xFFFF) ? targetProdect : 0xFFFF;

    /* 遍历，使两因数的乘积足够接近prodect */
    for(; fct1 > 1; fct1--)
    {
        for(fct2 = fct1; fct2 < fct_max; fct2++)
        {
            /* 求误差 */
            int32_t newerr = fct1 * fct2 - targetProdect;

            if(newerr < 0)
            {
                newerr = -newerr;
            }

            if(newerr < max_error)
            {
                /* 保存最小误差 */
                max_error = (uint16_t)newerr;

                fct1_save = fct1;
                fct2_save = fct2;

                /* 最佳 */
                if(max_error == 0)
                {
                    break;
                }
            }
        }
    }

    *factor1 = fct1_save;
    *factor2 = fct2_save;
    *error = (freq - clock / (fct1_save * fct2_save));

    return true;
}

/**
  * @brief  将定时中断时间转换为重装值和时钟分频值
  * @param  time: 中断时间(微秒)
  * @param  clock: 定时器时钟
  * @param  *period: 重装值地址
  * @param  *prescaler: 时钟分频值地址
  * @retval 无
  */
static void Timer_TimeFactorization(
    uint32_t time,
    uint32_t clock,
    uint16_t* factor1,
    uint16_t* factor2
)
{
    const uint32_t cyclesPerMicros = clock / 1000000U;
    const uint32_t prodect = time * cyclesPerMicros;
    uint16_t fct1, fct2;

    if(prodect < cyclesPerMicros * 30)
    {
        fct1 = 10;
        fct2 = prodect / 10;
    }
    else if(prodect < 65535 * 1000)
    {
        fct1 = prodect / 1000;
        fct2 = prodect / fct1;
    }
    else
    {
        fct1 = prodect / 20000;
        fct2 = prodect / fct1;
    }
    *factor1 = fct1;
    *factor2 = fct2;
}

/**
  * @brief  定时器使能
  * @param  TIMx: 定时器地址
  * @param  Enable: 使能
  * @retval 无
  */
void Timer_SetEnable(TIM_TypeDef* TIMx, bool Enable)
{
    TMR_Cmd(TIMx, Enable ? ENABLE : DISABLE);
}

/**
  * @brief  定时中断配置
  * @param  TIMx:定时器地址
  * @param  Time: 中断时间(微秒)
  * @param  Function: 定时中断回调函数
  * @retval 无
  */
void Timer_SetInterrupt(TIM_TypeDef* TIMx, uint32_t Time, Timer_CallbackFunction_t Function)
{
    uint16_t period = 0;
    uint16_t prescaler = 0;
    uint32_t clock = TIMER_GET_CLOCK_MAX(TIMx);

    if(!IS_TMR_ALL_PERIPH(TIMx) || Time == 0)
    {
        return;
    }

    /*将定时中断时间转换为重装值和时钟分频值*/
    Timer_TimeFactorization(
        Time,
        clock,
        &period,
        &prescaler
    );

    /*定时中断配置*/
    Timer_SetInterruptBase(
        TIMx,
        period,
        prescaler,
        Function,
        TIMER_PREEMPTIONPRIORITY_DEFAULT,
        TIMER_SUBPRIORITY_DEFAULT
    );
}

/**
  * @brief  更新定时中断频率
  * @param  TIMx:定时器地址
  * @param  Freq:中断频率
  * @retval true: 设置成功
  */
bool Timer_SetInterruptFreqUpdate(TIM_TypeDef* TIMx, uint32_t Freq)
{
    uint16_t period, prescaler;
    uint32_t clock = TIMER_GET_CLOCK_MAX(TIMx);
    int32_t error;

    if(!IS_TMR_ALL_PERIPH(TIMx) || Freq == 0)
        return false;

    bool success = Timer_FreqFactorization(
                       Freq,
                       clock,
                       &period,
                       &prescaler,
                       &error
                   );

    if(!success)
    {
        return false;
    }

    TMR_SetAutoreload(TIMx, period - 1);
    TMR_DIVConfig(TIMx, prescaler - 1, TMR_DIVReloadMode_Immediate);
    return true;
}

/**
  * @brief  获取定时器中断频率
  * @param  TIMx:定时器地址
  * @retval 中断频率
  */
uint32_t Timer_GetClockOut(TIM_TypeDef* TIMx)
{
    uint32_t clock = TIMER_GET_CLOCK_MAX(TIMx);
    if(!IS_TMR_ALL_PERIPH(TIMx))
        return 0;

    return (clock / ((TIMx->AR + 1) * (TIMx->DIV + 1)));
}

/**
  * @brief  更新定时中断时间
  * @param  TIMx:定时器地址
  * @param  Time: 中断时间(微秒)
  * @retval 无
  */
void Timer_SetInterruptTimeUpdate(TIM_TypeDef* TIMx, uint32_t Time)
{
    uint16_t period, prescaler;
    uint32_t clock = TIMER_GET_CLOCK_MAX(TIMx);

    if(!IS_TMR_ALL_PERIPH(TIMx))
        return;

    Timer_TimeFactorization(
        Time,
        clock,
        &period,
        &prescaler
    );

    TMR_SetAutoreload(TIMx, period - 1);
    TMR_DIVConfig(TIMx, prescaler - 1, TMR_DIVReloadMode_Immediate);
}

/**
  * @brief  定时中断基本配置
  * @param  TIMx:定时器地址
  * @param  Period:重装值
  * @param  Prescaler:时钟分频值
  * @param  Function: 定时中断回调函数
  * @param  PreemptionPriority: 抢占优先级
  * @param  SubPriority: 子优先级
  * @retval 无
  */
void Timer_SetInterruptBase(
    TIM_TypeDef* TIMx,
    uint16_t Period,
    uint16_t Prescaler,
    Timer_CallbackFunction_t Function,
    uint8_t PreemptionPriority,
    uint8_t SubPriority
)
{
    TMR_TimerBaseInitType  TMR_TimeBaseStructure;
    NVIC_InitType NVIC_InitStructure;
    uint8_t TMRx_IRQn = 0;
    TIMER_Type TIMERx = TIMER1;

    if(!IS_TMR_ALL_PERIPH(TIMx))
    {
        return;
    }

#define TMRx_IRQ_DEF(n,x_IRQn)\
do{\
    if(TIMx == TIM##n)\
    {\
        TIMERx = TIMER##n;\
        TMRx_IRQn = x_IRQn;\
        goto match;\
    }\
}\
while(0)

    /*如果编译器提示：identifier "xxx_IRQn" is undefined
     *把未定义的注释掉即可
     */
    TMRx_IRQ_DEF(1, TMR1_OV_TMR10_IRQn);
    TMRx_IRQ_DEF(2, TMR2_GLOBAL_IRQn);
    TMRx_IRQ_DEF(3, TMR3_GLOBAL_IRQn);
    TMRx_IRQ_DEF(4, TMR4_GLOBAL_IRQn);
    TMRx_IRQ_DEF(5, TMR5_GLOBAL_IRQn);

#ifdef TMR6
    TMRx_IRQ_DEF(6, TMR6_GLOBAL_IRQn);
#endif

#ifdef TMR7
    TMRx_IRQ_DEF(7, TMR7_GLOBAL_IRQn);
#endif

    TMRx_IRQ_DEF(8, TMR8_OV_TMR13_IRQn);
    TMRx_IRQ_DEF(9, TMR1_BRK_TMR9_IRQn);
    TMRx_IRQ_DEF(10, TMR1_OV_TMR10_IRQn);

#ifdef TMR11
    TMRx_IRQ_DEF(11, TMR1_TRG_COM_TMR11_IRQn);
#endif

#ifdef TMR12
    TMRx_IRQ_DEF(12, TMR8_BRK_TMR12_IRQn);
#endif

#ifdef TMR13
    TMRx_IRQ_DEF(13, TMR8_OV_TMR13_IRQn);
#endif

#ifdef TMR14
    TMRx_IRQ_DEF(14, TMR8_TRG_COM_TMR14_IRQn);
#endif

#ifdef TMR15
    TMRx_IRQ_DEF(15, TMR15_OV_IRQn);
#endif

match:

    if(TMRx_IRQn == 0)
    {
        return;
    }

    Timer_CallbackFunction[TIMERx] = Function;

    TMR_Reset(TIMx);
    Timer_ClockCmd(TIMx, ENABLE);

    TMR_TimeBaseStructInit(&TMR_TimeBaseStructure);
    TMR_TimeBaseStructure.TMR_RepetitionCounter = 0;
    TMR_TimeBaseStructure.TMR_Period = Period - 1;
    TMR_TimeBaseStructure.TMR_DIV = Prescaler - 1;
    TMR_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1;
    TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;
    TMR_TimeBaseInit(TIMx, &TMR_TimeBaseStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TMRx_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TMR_ClearFlag(TIMx, TMR_FLAG_Update);
    TMR_INTConfig(TIMx, TMR_INT_Overflow, ENABLE);
}

/**
  * @brief  设置输出比较值
  * @param  TIMx: 定时器地址
  * @param  TimerChannel: 定时器通道
  * @param  Compare:输出比较值
  * @retval 无
  */
void Timer_SetCompare(TIM_TypeDef* TIMx, uint8_t TimerChannel, uint32_t Compare)
{
    switch(TimerChannel)
    {
    case 1:
        TMR_SetCompare1(TIMx, Compare);
        break;
    case 2:
        TMR_SetCompare2(TIMx, Compare);
        break;
    case 3:
        TMR_SetCompare3(TIMx, Compare);
        break;
    case 4:
        TMR_SetCompare4(TIMx, Compare);
        break;
    default:
        break;
    }
}

/**
  * @brief  获取捕获值
  * @param  TIMx: 定时器地址
  * @param  TimerChannel: 定时器通道
  * @retval 捕获值
  */
uint16_t Timer_GetCompare(TIM_TypeDef* TIMx, uint8_t TimerChannel)
{
    uint16_t compare = 0;
    switch(TimerChannel)
    {
    case 1:
        compare = TMR_GetCapture1(TIMx);
        break;
    case 2:
        compare = TMR_GetCapture2(TIMx);
        break;
    case 3:
        compare = TMR_GetCapture3(TIMx);
        break;
    case 4:
        compare = TMR_GetCapture4(TIMx);
        break;
    default:
        break;
    }
    return compare;
}

/**
  * @brief  更新定时器时钟预分频数
  * @param  TIMx: 定时器地址
  * @param  rescaler: 时钟预分频数
  * @retval 无
  */
void Timer_SetPrescaler(TIM_TypeDef* TIMx, uint16_t rescaler)
{
    TIMx->DIV = rescaler;
}

/**
  * @brief  更新定时器自动重装值
  * @param  TIMx: 定时器地址
  * @param  Reload: 自动重装值
  * @retval 无
  */
void Timer_SetReload(TIM_TypeDef* TIMx, uint16_t Reload)
{
    TIMx->AR = Reload;
}

/**
  * @brief  应用定时器更改
  * @param  TIMx: 定时器地址
  * @retval 无
  */
void Timer_GenerateUpdate(TIM_TypeDef* TIMx)
{
    TIMx->EVEG = TMR_DIVReloadMode_Immediate;
}

#define TMRx_IRQHANDLER(n) \
do{\
    if (TMR_GetINTStatus(TMR##n, TMR_INT_Overflow) != RESET)\
    {\
        if(Timer_CallbackFunction[TIMER##n])\
        {\
            Timer_CallbackFunction[TIMER##n]();\
        }\
        TMR_ClearITPendingBit(TMR##n, TMR_INT_Overflow);\
    }\
}while(0)

/**
  * @brief  定时中断入口，定时器1、10
  * @param  无
  * @retval 无
  */
void TMR1_OV_TMR10_IRQHandler(void)
{
    TMRx_IRQHANDLER(1);
    TMRx_IRQHANDLER(10);
}

/**
  * @brief  定时中断入口，定时器2
  * @param  无
  * @retval 无
  */
void TMR2_GLOBAL_IRQHandler(void)
{
    TMRx_IRQHANDLER(2);
}

/**
  * @brief  定时中断入口，定时器3
  * @param  无
  * @retval 无
  */
void TMR3_GLOBAL_IRQHandler(void)
{
    TMRx_IRQHANDLER(3);
}

/**
  * @brief  定时中断入口，定时器4
  * @param  无
  * @retval 无
  */
void TMR4_GLOBAL_IRQHandler(void)
{
    TMRx_IRQHANDLER(4);
}

/**
  * @brief  定时中断入口，定时器5
  * @param  无
  * @retval 无
  */
void TMR5_GLOBAL_IRQHandler(void)
{
    TMRx_IRQHANDLER(5);
}

/**
  * @brief  定时中断入口，定时器6
  * @param  无
  * @retval 无
  */
void TMR6_GLOBAL_IRQHandler(void)
{
#ifdef TMR6
    TMRx_IRQHANDLER(6);
#endif
}

/**
  * @brief  定时中断入口，定时器7
  * @param  无
  * @retval 无
  */
void TMR7_GLOBAL_IRQHandler(void)
{
#ifdef TMR7
    TMRx_IRQHANDLER(7);
#endif
}

/**
  * @brief  定时中断入口，定时器8、13
  * @param  无
  * @retval 无
  */
void TMR8_OV_TMR13_IRQHandler(void)
{
    TMRx_IRQHANDLER(8);
#ifdef TMR13
    TMRx_IRQHANDLER(13);
#endif
}

/**
  * @brief  定时中断入口，定时器15
  * @param  无
  * @retval 无
  */
void TMR15_OV_IRQHandler(void)
{
#ifdef TMR15
    TMRx_IRQHANDLER(15);
#endif
}
