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
#include "adc.h"

#ifndef CFG_ENABLE_ADC_DMA
#define CFG_ENABLE_ADC_DMA 1
#endif

#if CFG_ENABLE_ADC_DMA
#define ADC_DMA_REGMAX 18

/*ADC通道注册个数*/
static uint8_t ADC_DMA_RegCnt = 0;

/*ADC通道注册列表*/
static uint8_t ADC_DMA_RegChannelList[ADC_DMA_REGMAX] = {0};

/*ADC DMA缓存数组*/
static uint16_t ADC_DMA_ConvertedValue[ADC_DMA_REGMAX] = {0};

/**
  * @brief  搜索注册列表，找出ADC通道对应的索引号
  * @param  ADC_Channel:ADC通道号
  * @retval ADC通道注册列表对应索引号，-1:未找到对应通道号
  */
static int16_t ADC_DMA_SearchChannel(uint8_t ADC_Channel)
{
    uint8_t index;

    for(index = 0; index < ADC_DMA_RegCnt; index++)
    {
        if(ADC_Channel == ADC_DMA_RegChannelList[index])
        {
            return index;
        }
    }
    return -1;
}

#endif

/**
  * @brief  注册需要DMA搬运的ADC通道
  * @param  ADC_Channel:ADC通道号
  * @retval 引脚注册列表对应索引号，-1:不支持ADC，-2:引脚重复注册，-3:超出最大注册个数
  */
int16_t ADC_DMA_Register(uint8_t ADC_Channel)
{
#if CFG_ENABLE_ADC_DMA
    /*初始化ADC通道列表*/
    static uint8_t IsInit = 0;
    if(!IsInit)
    {
        uint8_t i;
        for(i = 0; i < ADC_DMA_REGMAX; i++)
        {
            ADC_DMA_RegChannelList[i] = 0xFF;
        }
        IsInit = 1;
    }

    /*是否是合法ADC通道*/
    if(!IS_ADC_CHANNEL(ADC_Channel))
        return -1;

    /*是否已在引脚列表重复注册*/
    if(ADC_DMA_SearchChannel(ADC_Channel) != -1)
        return -2;

    /*是否超出最大注册个数*/
    if(ADC_DMA_RegCnt >= ADC_DMA_REGMAX)
        return -3;

    /*写入注册列表*/
    ADC_DMA_RegChannelList[ADC_DMA_RegCnt] = ADC_Channel;

    /*注册个数+1*/
    ADC_DMA_RegCnt++;

    return ADC_DMA_RegCnt - 1;
#else
    return -1;
#endif
}

#if CFG_ENABLE_ADC_DMA
/**
 * @brief Supports ADC1 only
 * 
 */
static void ADC_DMA_Config()
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA2_Stream0);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); /*DMA2的时钟使能*/
    while(DMA_GetCmdStatus(DMA2_Stream0) != DISABLE); /*等待DMA可以配置*/


    DMA_InitStructure.DMA_Channel = DMA_Channel_0;/*DMA通道0*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; /*外设地址*/
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)ADC_DMA_ConvertedValue;/*存取器地址*/
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;/*方向从外设到内存*/
    DMA_InitStructure.DMA_BufferSize = ADC_DMA_RegCnt;/*数据传输的数量为3*/
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;/*外设寄存器只有一个，地址不用递增*/
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;/*存储器地址递增*/
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;/*数据长度半字*/
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;/*数据长度半字*/
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;/*高优先级*/
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;/*循环模式*/
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;/*禁止FIFO*/
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;/*FIFO的值*/
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;/*单次传输*/
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;/*单次传输*/
    DMA_Init(DMA2_Stream0, &DMA_InitStructure); /**/
    DMA_Cmd(DMA2_Stream0, ENABLE); //开启DMA传输
}
#endif

#if CFG_ENABLE_ADC_DMA
/**
  * @brief  ADC DMA 配置
  * @param  无
  * @retval 无
  */
void ADC_DMA_Init(void)
{
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    uint16_t index;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC时钟

    ADC_DMA_Config();

    ADC_DeInit();
    
    /*通用控制寄存器的配置*/
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;/*DMA失能*/
    ADC_CommonInitStructure.ADC_Mode          = ADC_Mode_Independent;/*独立模式*/
    ADC_CommonInitStructure.ADC_Prescaler     = ADC_Prescaler_Div6;/*APB2的4分频 即84/4=21M*/
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;/*两个采样阶段的延时5个时钟*/
    ADC_CommonInit(&ADC_CommonInitStructure);/*初始化*/
    /*初始化ADC1*/
    ADC_InitStructure.ADC_Resolution  = ADC_Resolution_12b;/*12位模式*/
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;/*扫描模式*/
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;/*连续转换*/
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;/*禁止触发检测 使用软件触发*/
    ADC_InitStructure.ADC_DataAlign    = ADC_DataAlign_Right;/*右对齐*/
    ADC_InitStructure.ADC_NbrOfConversion = ADC_DMA_RegCnt;/*只使用1通道 规则通为1*/
    ADC_Init(ADC1, &ADC_InitStructure); /*初始化*/

    ADC_Cmd(ADC1, ENABLE); /*开启转换*/
    
    for(index = 0; index < ADC_DMA_RegCnt; index++)
    {
        ADC_RegularChannelConfig(
            ADC1,
            ADC_DMA_RegChannelList[index],
            index + 1,
            ADC_SampleTime_480Cycles
        );

        #if defined(ADC_Channel_TempSensor)
        if(ADC_DMA_RegChannelList[index] == ADC_Channel_TempSensor)
        {
            ADC_TempSensorVrefintCmd(ENABLE);
        }
        #endif
    }
    
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE); //源数据变化时开启DMA传输
    ADC_DMACmd(ADC1, ENABLE); //使能ADC传输 /*设置规则通道2 1个序列 采样时间 */
    ADC_SoftwareStartConv(ADC1);/*启动软件转换*/
}
#endif

/**
  * @brief  获取DMA搬运的ADC值
  * @param  ADC_Channel:ADC通道号
  * @retval ADC值
  */
uint16_t ADC_DMA_GetValue(uint8_t ADC_Channel)
{
#if CFG_ENABLE_ADC_DMA
    int16_t index;

    if(!IS_ADC_CHANNEL(ADC_Channel))
        return 0;

    index = ADC_DMA_SearchChannel(ADC_Channel);
    if(index == -1)
        return 0;

    return ADC_DMA_ConvertedValue[index];
#else
    return 0;
#endif
}

void ADCx_Init(ADC_TypeDef* ADCx)
{
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef       ADC_InitStructure;

    if(ADCx == ADC1)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);//复位结束
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟
    }
#ifdef ADC2
    else if(ADCx == ADC2)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2, DISABLE);//复位结束
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); //使能ADC2时钟
    }
#endif
#ifdef ADC3
    else if(ADCx == ADC3)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3, DISABLE);//复位结束
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE); //使能ADC3时钟
    }
#endif

    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
    ADC_CommonInit(&ADC_CommonInitStructure);//初始化

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
    ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1
    ADC_Init(ADCx, &ADC_InitStructure);//ADC初始化

    ADC_Cmd(ADCx, ENABLE);//开启AD转换器
}

uint16_t ADCx_GetValue(ADC_TypeDef* ADCx, uint8_t ADC_Channel)
{
    ADC_RegularChannelConfig(ADCx, ADC_Channel, 1, ADC_SampleTime_144Cycles);
    ADC_SoftwareStartConv(ADCx);
    while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADCx);
}
