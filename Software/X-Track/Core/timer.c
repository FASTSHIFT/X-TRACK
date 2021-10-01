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
    TIMER9, TIMER10, TIMER11, TIMER12, TIMER13, TIMER14, TIMER15, 
    TIMER_MAX
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
    uint32_t clock = Timer_GetClockMax(TIMx);
    
    if(!IS_TMR_ALL_PERIPH(TIMx) || time == 0)
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
    uint16_t period, prescaler;
    uint32_t clock = Timer_GetClockMax(TIMx);
    
    if(!IS_TMR_ALL_PERIPH(TIMx) || freq == 0)
        return;

    Timer_FreqToArrPsc(
        freq, 
        clock,
        &period, 
        &prescaler
    );
    TMR_SetAutoreload(TIMx, period - 1);
    TMR_DIVConfig(TIMx, prescaler - 1, TMR_DIVReloadMode_Immediate);
}

/**
  * @brief  获取定时器中断频率
  * @param  TIMx:定时器地址
  * @retval 中断频率
  */
uint32_t Timer_GetClockOut(TIM_TypeDef* TIMx)
{
    uint32_t clock = Timer_GetClockMax(TIMx);
    if(!IS_TMR_ALL_PERIPH(TIMx))
        return 0;

    return (clock / ((TIMx->AR + 1) * (TIMx->DIV + 1)));
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
    uint32_t clock = Timer_GetClockMax(TIMx);

    if(!IS_TMR_ALL_PERIPH(TIMx))
        return;

    Timer_TimeToArrPsc(
        time,
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
    TMR_TimerBaseInitType  TMR_TimeBaseStructure;
    NVIC_InitType NVIC_InitStructure;
    uint8_t TMRx_IRQn = 0;
    TIMER_Type TIMERx = TIMER1;

    if(!IS_TMR_ALL_PERIPH(TIMx))
        return;

#define TMRx_IRQ_DEF(n,x_IRQn)\
do{\
    if(TIMx == TIM##n)\
    {\
        TIMERx = TIMER##n;\
        TMRx_IRQn = x_IRQn;\
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
    //TMRx_IRQ_DEF(11, TMR1_TRG_HALL_TMR11_IRQn);
#ifdef TMR12
    TMRx_IRQ_DEF(12, TMR8_BRK_TMR12_IRQn);
#endif
#ifdef TMR13
    TMRx_IRQ_DEF(13, TMR8_OV_TMR13_IRQn);
#endif
#ifdef TMR14
    //TMRx_IRQ_DEF(14, TMR8_TRG_HALL_TMR14_IRQn);
#endif
    //TMRx_IRQ_DEF(15, TMR15_OV_IRQn);

    if(TMRx_IRQn == 0)
        return;

    /*register callback function*/
    TIMx_Function[TIMERx] = function;

    /*Enable PeriphClock*/
    TMR_Reset(TIMx);
    Timer_ClockCmd(TIMx, ENABLE);

    TMR_TimeBaseStructure.TMR_RepetitionCounter = 0;
    TMR_TimeBaseStructure.TMR_Period = period - 1;//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TMR_TimeBaseStructure.TMR_DIV = prescaler - 1;//设置用来作为TIMx时钟频率除数的预分频值 
    TMR_TimeBaseStructure.TMR_ClockDivision = TMR_CKD_DIV1;//设置时钟分割
    TMR_TimeBaseStructure.TMR_CounterMode = TMR_CounterDIR_Up;//TIM向上计数模式
    TMR_TimeBaseInit(TIMx, &TMR_TimeBaseStructure);
    
    /**********************************设置中断优先级************************************/
    NVIC_InitStructure.NVIC_IRQChannel = TMRx_IRQn;//TIM中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;//先占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;  //从优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    TMR_ClearFlag(TIMx, TMR_FLAG_Update);
    TMR_INTConfig(TIMx, TMR_INT_Overflow, ENABLE);  //使能TIM中断
}

#define TMRx_IRQHANDLER(n) \
do{\
    if (TMR_GetINTStatus(TMR##n, TMR_INT_Overflow) != RESET)\
    {\
        if(TIMx_Function[TIMER##n]) TIMx_Function[TIMER##n]();\
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
