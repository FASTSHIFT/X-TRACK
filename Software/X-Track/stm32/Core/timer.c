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
#include "timer.h"

/*定时器编号枚举*/
typedef enum
{
    TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER6, TIMER7, TIMER8,
    TIMER9, TIMER10, TIMER11, TIMER12, TIMER13, TIMER14, TIMER_MAX
} TIMER_Type;

/*定时中断回调函数指针数组*/
static Timer_CallbackFunction_t TIMx_Function[TIMER_MAX] = {0};

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
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, NewState);
    }
    else if(TIMx == TIM2)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, NewState);
    }
    else if(TIMx == TIM3)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, NewState);
    }
    else if(TIMx == TIM4)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, NewState);
    }
    else if(TIMx == TIM5)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, NewState);
    }
#ifdef TIM6
    else if(TIMx == TIM6)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, NewState);
    }
#endif
#ifdef TIM7
    else if(TIMx == TIM7)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, NewState);
    }
#endif
    else if(TIMx == TIM8)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, NewState);
    }
    else if(TIMx == TIM9)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, NewState);
    }
    else if(TIMx == TIM10)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, NewState);
    }
    else if(TIMx == TIM11)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, NewState);
    }
#ifdef TIM12
    else if(TIMx == TIM12)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, NewState);
    }
#endif
#ifdef TIM13
    else if(TIMx == TIM13)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, NewState);
    }
#endif
#ifdef TIM14
    if(TIMx == TIM14)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, NewState);
    }
#endif
}

/*取绝对值*/
#define CLOCK_ABS(x) (((x)>0)?(x):-(x))

/*快速求平方根*/
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
  * @brief  将定时中断频率转换为重装值和时钟分频值
  * @param  freq: 中断频率(Hz)
  * @param  clock: 定时器时钟
  * @param  *period: 重装值地址
  * @param  *prescaler: 时钟分频值地址
  * @retval 误差值(Hz)
  */
static int32_t Timer_FreqToArrPsc(
    uint32_t freq, uint32_t clock, 
    uint16_t *period, uint16_t *prescaler
)
{
    uint32_t prodect;
    uint16_t psc, arr, arr_max;
    uint16_t max_error = 0xFFFF;

    if(freq == 0 || freq > clock)
        goto failed;

    /*获取arr和psc目标乘积*/
    prodect = clock / freq;

    /*从prodect的平方根开始计算*/
    psc = Qsqrt(prodect);

    /*arr必定小于等于prodect，尽量减少arr遍历次数*/
    arr_max = (prodect < 0xFFFF) ? prodect : 0xFFFF;

    /*遍历，使arr*psc足够接近prodect*/
    for(; psc > 1; psc--)
    {
        for(arr = psc; arr < arr_max; arr++)
        {
            /*求误差*/
            int32_t newerr = arr * psc - prodect;
            newerr = CLOCK_ABS(newerr);
            if(newerr < max_error)
            {
                /*保存最小误差*/
                max_error = newerr;
                /*保存arr和psc*/
                *period = arr;
                *prescaler = psc;
                /*最佳*/
                if(max_error == 0)
                    goto success;
            }
        }
    }

    /*计算成功*/
success:
    return (freq - clock / ((*period) * (*prescaler)));

    /*失败*/
failed:
    return (freq - clock);
}

/**
  * @brief  将定时中断时间转换为重装值和时钟分频值
  * @param  time: 中断时间(微秒)
  * @param  clock: 定时器时钟
  * @param  *period: 重装值地址
  * @param  *prescaler: 时钟分频值地址
  * @retval 无
  */
static void Timer_TimeToArrPsc(
    uint32_t time, uint32_t clock,
    uint16_t *period, uint16_t *prescaler
)
{
    uint32_t cyclesPerMicros = clock / 1000000U;
    uint32_t prodect = time * cyclesPerMicros;
    uint16_t arr, psc;

    if(prodect < cyclesPerMicros * 30)
    {
        arr = 10;
        psc = prodect / arr;
    }
    else if(prodect < 65535 * 1000)
    {
        arr = prodect / 1000;
        psc = prodect / arr;
    }
    else
    {
        arr = prodect / 20000;
        psc = prodect / arr;
    }
    *period = arr;
    *prescaler = psc;
}

/**
  * @brief  定时中断配置
  * @param  TIMx:定时器地址
  * @param  time: 中断时间(微秒)
  * @param  function: 定时中断回调函数
  * @retval 无
  */
void Timer_SetInterrupt(TIM_TypeDef* TIMx, uint32_t time, Timer_CallbackFunction_t function)
{
    uint16_t period, prescaler;
    uint32_t clock = (IS_APB2_TIM(TIMx) ? F_CPU : (F_CPU / 2));

    if(!IS_TIM_ALL_PERIPH(TIMx) || time == 0)
        return;

    /*将定时中断时间转换为重装值和时钟分频值*/
    Timer_TimeToArrPsc(
        time,
        clock,
        &period,
        &prescaler
    );

    /*定时中断配置*/
    Timer_SetInterruptBase(
        TIMx,
        period,
        prescaler,
        function,
        Timer_PreemptionPriority_Default,
        Timer_SubPriority_Default
    );
}

/**
  * @brief  更新定时中断频率
  * @param  TIMx:定时器地址
  * @param  freq:中断频率
  * @retval 无
  */
void Timer_SetInterruptFreqUpdate(TIM_TypeDef* TIMx, uint32_t freq)
{
    uint16_t period = 1, prescaler = 1;
    uint32_t clock = (IS_APB2_TIM(TIMx) ? F_CPU : (F_CPU / 2));

    if(!IS_TIM_ALL_PERIPH(TIMx) || freq == 0)
        return;

    Timer_FreqToArrPsc(
        freq,
        clock,
        &period,
        &prescaler
    );
    TIM_SetAutoreload(TIMx, period - 1);
    TIM_PrescalerConfig(TIMx, prescaler - 1, TIM_PSCReloadMode_Immediate);
}

/**
  * @brief  获取定时器中断频率
  * @param  TIMx:定时器地址
  * @retval 中断频率
  */
uint32_t Timer_GetClockOut(TIM_TypeDef* TIMx)
{
    uint32_t clock = (IS_APB2_TIM(TIMx) ? F_CPU : (F_CPU / 2));
    if(!IS_TIM_ALL_PERIPH(TIMx))
        return 0;

    return (clock / ((TIMx->ARR + 1) * (TIMx->PSC + 1)));
}

/**
  * @brief  更新定时中断时间
  * @param  TIMx:定时器地址
  * @param  time: 中断时间(微秒)
  * @retval 无
  */
void Timer_SetInterruptTimeUpdate(TIM_TypeDef* TIMx, uint32_t time)
{
    uint16_t period, prescaler;
    uint32_t clock = (IS_APB2_TIM(TIMx) ? F_CPU : (F_CPU / 2));

    if(!IS_TIM_ALL_PERIPH(TIMx))
        return;

    Timer_TimeToArrPsc(
        time,
        clock,
        &period,
        &prescaler
    );

    TIM_SetAutoreload(TIMx, period - 1);
    TIM_PrescalerConfig(TIMx, prescaler - 1, TIM_PSCReloadMode_Immediate);
}

/**
  * @brief  定时中断基本配置
  * @param  TIMx:定时器地址
  * @param  period:重装值
  * @param  prescaler:时钟分频值
  * @param  function: 定时中断回调函数
  * @param  PreemptionPriority: 抢占优先级
  * @param  SubPriority: 子优先级
  * @retval 无
  */
void Timer_SetInterruptBase(
    TIM_TypeDef* TIMx,
    uint16_t period, uint16_t prescaler,
    Timer_CallbackFunction_t function,
    uint8_t PreemptionPriority, uint8_t SubPriority
)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    uint8_t TIMx_IRQn;
    TIMER_Type TIMERx;

    if(!IS_TIM_ALL_PERIPH(TIMx))
        return;

#define TIMx_IRQn_DEF(n,x_IRQn)\
do{\
    if(TIMx == TIM##n)\
    {\
        TIMERx = TIMER##n;\
        TIMx_IRQn = x_IRQn;\
    }\
}\
while(0)

    /*如果提示 
     *identifier "xxx_IRQn" is undefined
     *把未定义的注释掉即可
     */
    TIMx_IRQn_DEF(1, TIM1_UP_TIM10_IRQn);
    TIMx_IRQn_DEF(2, TIM2_IRQn);
    TIMx_IRQn_DEF(3, TIM3_IRQn);
    TIMx_IRQn_DEF(4, TIM4_IRQn);
    TIMx_IRQn_DEF(5, TIM5_IRQn);
#if defined(TIM6_DAC_IRQn)
    TIMx_IRQn_DEF(6, TIM6_DAC_IRQn);
#endif
#if defined(TIM7_IRQn)
    TIMx_IRQn_DEF(7, TIM7_IRQn);
#endif
#if defined(TIM8_UP_TIM13_IRQn)
    TIMx_IRQn_DEF(8, TIM8_UP_TIM13_IRQn);
#endif
    TIMx_IRQn_DEF(9, TIM1_BRK_TIM9_IRQn);
    TIMx_IRQn_DEF(10, TIM1_UP_TIM10_IRQn);
    TIMx_IRQn_DEF(11, TIM1_TRG_COM_TIM11_IRQn);
#if defined(TIM8_BRK_TIM12_IRQn)
    TIMx_IRQn_DEF(12, TIM8_BRK_TIM12_IRQn);
#endif
#if defined(TIM8_UP_TIM13_IRQn)
    TIMx_IRQn_DEF(13, TIM8_UP_TIM13_IRQn);
#endif
#if defined(TIM8_TRG_COM_TIM14_IRQn)
    TIMx_IRQn_DEF(14, TIM8_TRG_COM_TIM14_IRQn);
#endif

    if(TIMx_IRQn == 0)
        return;

    /*register callback function*/
    TIMx_Function[TIMERx] = function;

    /*Enable PeriphClock*/
    TIM_DeInit(TIMx);
    Timer_ClockCmd(TIMx, ENABLE);

    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_Period = period - 1;         //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler - 1;  //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    /**********************************设置中断优先级************************************/
    NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQn;  //TIM中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;  //先占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;  //从优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TIM_ClearFlag(TIMx, TIM_FLAG_Update);
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);  //使能TIM中断
}

#define TIMx_IRQHANDLER(n) \
do{\
    if (TIM_GetITStatus(TIM##n, TIM_IT_Update) != RESET)\
    {\
        if(TIMx_Function[TIMER##n]) TIMx_Function[TIMER##n]();\
        TIM_ClearITPendingBit(TIM##n, TIM_IT_Update);\
    }\
}while(0)

/**
  * @brief  定时中断入口，定时器1
  * @param  无
  * @retval 无
  */
void TIM1_UP_TIM10_IRQHandler(void)   //TIM1中断
{
    TIMx_IRQHANDLER(1);
    TIMx_IRQHANDLER(10);
}

/**
  * @brief  定时中断入口，定时器2
  * @param  无
  * @retval 无
  */
void TIM2_IRQHandler(void)
{
    TIMx_IRQHANDLER(2);
}

/**
  * @brief  定时中断入口，定时器3
  * @param  无
  * @retval 无
  */
void TIM3_IRQHandler(void)
{
    TIMx_IRQHANDLER(3);
}

/**
  * @brief  定时中断入口，定时器4
  * @param  无
  * @retval 无
  */
void TIM4_IRQHandler(void)
{
    TIMx_IRQHANDLER(4);
}

/**
  * @brief  定时中断入口，定时器5
  * @param  无
  * @retval 无
  */
void TIM5_IRQHandler(void)
{
    TIMx_IRQHANDLER(5);
}

/**
  * @brief  定时中断入口，定时器6
  * @param  无
  * @retval 无
  */
void TIM6_DAC_IRQHandler(void)
{
#ifdef TIM6
    TIMx_IRQHANDLER(6);
#endif
}

/**
  * @brief  定时中断入口，定时器7
  * @param  无
  * @retval 无
  */
void TIM7_IRQHandler(void)
{
#ifdef TIM7
    TIMx_IRQHANDLER(7);
#endif
}

/**
  * @brief  定时中断入口，定时器8
  * @param  无
  * @retval 无
  */
void TIM8_UP_TIM13_IRQHandler(void)
{
    TIMx_IRQHANDLER(8);
#ifdef TIM13
    TIMx_IRQHANDLER(13);
#endif
}
