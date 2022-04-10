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
#include "exti.h"
#include "gpio.h"

#define EXTI_GetPortSourceGPIOx(Pin) GPIO_GetPortNum(Pin)
#define EXTI_GetPinSourcex(Pin)      GPIO_GetPinNum(Pin)

static EXTI_CallbackFunction_t EXTI_Function[16] = {0};

/**
  * @brief  获取外部中断的中断通道
  * @param  Pin: 引脚编号
  * @retval 通道编号
  */
static IRQn_Type EXTI_GetIRQn(uint8_t Pin)
{
    IRQn_Type EXINTx_IRQn = EXINT0_IRQn;
    uint8_t Pinx = GPIO_GetPinNum(Pin);

    if(Pinx <= 4)
    {
        switch(Pinx)
        {
        case 0:
            EXINTx_IRQn = EXINT0_IRQn;
            break;
        case 1:
            EXINTx_IRQn = EXINT1_IRQn;
            break;
        case 2:
            EXINTx_IRQn = EXINT2_IRQn;
            break;
        case 3:
            EXINTx_IRQn = EXINT3_IRQn;
            break;
        case 4:
            EXINTx_IRQn = EXINT4_IRQn;
            break;
        }
    }
    else if(Pinx >= 5 && Pinx <= 9)
    {
        EXINTx_IRQn = EXINT9_5_IRQn;
    }
    else if(Pinx >= 10 && Pinx <= 15)
    {
        EXINTx_IRQn = EXINT15_10_IRQn;
    }

    return EXINTx_IRQn;
}

/**
  * @brief  外部中断初始化
  * @param  Pin: 引脚编号
  * @param  Function: 回调函数
  * @param  polarity_config: 触发方式
  * @param  PreemptionPriority: 抢占优先级
  * @param  SubPriority: 子优先级
  * @retval 无
  */
void EXTIx_Init(
    uint8_t Pin,
    EXTI_CallbackFunction_t Function,
    exint_polarity_config_type line_polarity,
    uint8_t PreemptionPriority,
    uint8_t SubPriority
)
{
    exint_init_type exint_init_struct;
    uint8_t Pinx;

    if(!IS_PIN(Pin))
        return;

    Pinx = GPIO_GetPinNum(Pin);

    if(Pinx > 15)
        return;

    EXTI_Function[Pinx] = Function;

    crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
    scfg_exint_line_config(GPIO_GetPortNum(Pin), (scfg_pins_source_type)Pinx);

    exint_default_para_init(&exint_init_struct);
    exint_init_struct.line_select = 1 << Pinx;
    exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
    exint_init_struct.line_polarity = line_polarity;
    exint_init_struct.line_enable = TRUE;
    exint_init(&exint_init_struct);

    nvic_irq_enable(EXTI_GetIRQn(Pin), PreemptionPriority, SubPriority);
}

/**
  * @brief  外部中断初始化 (Arduino)
  * @param  Pin: 引脚编号
  * @param  function: 回调函数
  * @param  line_polarity: 触发方式
  * @retval 无
  */
void attachInterrupt(uint8_t Pin, EXTI_CallbackFunction_t Function, exint_polarity_config_type line_polarity)
{
    EXTIx_Init(
        Pin,
        Function,
        line_polarity,
        EXTI_PREEMPTIONPRIORITY_DEFAULT,
        EXTI_SUBPRIORITY_DEFAULT
    );
}

/**
  * @brief  关闭给定的中断 (Arduino)
  * @param  Pin: 引脚编号
  * @retval 无
  */
void detachInterrupt(uint8_t Pin)
{
    if(!IS_PIN(Pin))
        return;

    nvic_irq_disable(EXTI_GetIRQn(Pin));
}

#define EXTIx_IRQHANDLER(n) \
do{\
    if(exint_flag_get(EXINT_LINE_##n) != RESET)\
    {\
        if(EXTI_Function[n]) EXTI_Function[n]();\
        exint_flag_clear(EXINT_LINE_##n);\
    }\
}while(0)

/**
  * @brief  外部中断入口，通道0
  * @param  无
  * @retval 无
  */
void EXINT0_IRQHandler(void)
{
    EXTIx_IRQHANDLER(0);
}

/**
  * @brief  外部中断入口，通道1
  * @param  无
  * @retval 无
  */
void EXINT1_IRQHandler(void)
{
    EXTIx_IRQHANDLER(1);
}

/**
  * @brief  外部中断入口，通道2
  * @param  无
  * @retval 无
  */
void EXINT2_IRQHandler(void)
{
    EXTIx_IRQHANDLER(2);
}

/**
  * @brief  外部中断入口，通道3
  * @param  无
  * @retval 无
  */
void EXINT3_IRQHandler(void)
{
    EXTIx_IRQHANDLER(3);
}

/**
  * @brief  外部中断入口，通道4
  * @param  无
  * @retval 无
  */
void EXINT4_IRQHandler(void)
{
    EXTIx_IRQHANDLER(4);
}

/**
  * @brief  外部中断入口，通道9~5
  * @param  无
  * @retval 无
  */
void EXINT9_5_IRQHandler(void)
{
    EXTIx_IRQHANDLER(5);
    EXTIx_IRQHANDLER(6);
    EXTIx_IRQHANDLER(7);
    EXTIx_IRQHANDLER(8);
    EXTIx_IRQHANDLER(9);
}

/**
  * @brief  外部中断入口，通道15~10
  * @param  无
  * @retval 无
  */
void EXINT15_10_IRQHandler(void)
{
    EXTIx_IRQHANDLER(10);
    EXTIx_IRQHANDLER(11);
    EXTIx_IRQHANDLER(12);
    EXTIx_IRQHANDLER(13);
    EXTIx_IRQHANDLER(14);
    EXTIx_IRQHANDLER(15);
}
