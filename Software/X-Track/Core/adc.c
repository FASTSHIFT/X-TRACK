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

#define ADC_DMA_REGMAX 18

/*引脚注册个数*/
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
static int16_t ADC_DMA_SearchChannel(uint16_t ADC_Channel)
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

/**
  * @brief  注册需要DMA搬运的ADC通道
  * @param  ADC_Channel:ADC通道号
  * @retval 引脚注册列表对应索引号，-1:不支持ADC，-2:引脚重复注册，-3:超出最大注册个数
  */
int16_t ADC_DMA_Register(uint8_t ADC_Channel)
{
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
}

/**
  * @brief  ADC DMA 配置
  * @param  无
  * @retval 无
  */
void ADC_DMA_Init(void)
{
    DMA_InitType DMA_InitStructure;
    ADC_InitType ADC_InitStructure;
    uint16_t index;

    // 打开DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);
    // 打开ADC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC1, ENABLE);

    // 复位DMA控制器
    DMA_Reset(DMA1_Channel1);

    // 配置 DMA 初始化结构体
    // 外设基址为：ADC 数据寄存器地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(ADC1->RDOR));

    // 存储器地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_DMA_ConvertedValue;

    // 数据源来自外设
    DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALSRC;

    // 缓冲区大小，应该等于数据目的地的大小
    DMA_InitStructure.DMA_BufferSize = ADC_DMA_RegCnt;

    // 外设寄存器只有一个，地址不用递增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;

    // 存储器地址递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;

    // 外设数据大小为半字，即两个字节
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_HALFWORD;

    // 内存数据大小也为半字，跟外设数据大小相同
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_HALFWORD;

    // 循环传输模式
    DMA_InitStructure.DMA_Mode = DMA_MODE_CIRCULAR;

    // DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_HIGH;

    // 禁止存储器到存储器模式，因为是从外设到存储器
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;

    // 初始化DMA
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    // 使能 DMA 通道
    DMA_ChannelEnable(DMA1_Channel1, ENABLE);

    // ADC 模式配置
    // 只使用一个ADC，属于单模式
    ADC_Reset(ADC1);
    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;

    // 扫描模式
    ADC_InitStructure.ADC_ScanMode = ENABLE;

    // 连续转换模式
    ADC_InitStructure.ADC_ContinuousMode = ENABLE;

    // 不用外部触发转换，软件开启即可
    ADC_InitStructure.ADC_ExternalTrig = ADC_ExternalTrig_None;

    // 转换结果右对齐
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;

    // 转换通道个数
    ADC_InitStructure.ADC_NumOfChannel = ADC_DMA_RegCnt;

    // 初始化ADC
    ADC_Init(ADC1, &ADC_InitStructure);

    // 配置ADC时钟CLK2的8分频，即9MHz
    RCC_ADCCLKConfig(RCC_APB2CLK_Div8);

    // 配置ADC 通道的转换顺序和采样时间
    for(index = 0; index < ADC_DMA_RegCnt; index++)
    {
        ADC_RegularChannelConfig(
            ADC1,
            ADC_DMA_RegChannelList[index],
            index + 1,
            ADC_SampleTime_55_5
        );

        if(ADC_DMA_RegChannelList[index] == ADC_Channel_TempSensor)
        {
            ADC_TempSensorVrefintCtrl(ENABLE);
        }
    }

    // 使能ADC DMA 请求
    ADC_DMACtrl(ADC1, ENABLE);

    // 开启ADC ，并开始转换
    ADC_Ctrl(ADC1, ENABLE);

    // 初始化ADC 校准寄存器
    ADC_RstCalibration(ADC1);

    // 等待校准寄存器初始化完成
    while(ADC_GetResetCalibrationStatus(ADC1));

    // ADC开始校准
    ADC_StartCalibration(ADC1);
    // 等待校准完成
    while(ADC_GetCalibrationStatus(ADC1));

    // 由于没有采用外部触发，所以使用软件触发ADC转换
    ADC_SoftwareStartConvCtrl(ADC1, ENABLE);
}

/**
  * @brief  获取DMA搬运的ADC值
  * @param  ADC_Channel:ADC通道号
  * @retval ADC值
  */
uint16_t ADC_DMA_GetValue(uint8_t ADC_Channel)
{
    int16_t index;

    if(!IS_ADC_CHANNEL(ADC_Channel))
        return 0;

    index = ADC_DMA_SearchChannel(ADC_Channel);
    if(index == -1)
        return 0;

    return ADC_DMA_ConvertedValue[index];
}

/**
  * @brief  ADC 配置
  * @param  ADCx: ADC地址
  * @retval 无
  */
void ADCx_Init(ADC_Type* ADCx)
{
    ADC_InitType ADC_InitStructure;

    if(ADCx == ADC1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC1, ENABLE);
    }
    else if(ADCx == ADC2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC2, ENABLE);
    }
#ifdef ADC3
    else if(ADCx == ADC3)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_ADC3, ENABLE);
    }
#endif
    else
    {
        return;
    }

    RCC_ADCCLKConfig(RCC_APB2CLK_Div8);

    ADC_Reset(ADCx);
    ADC_StructInit(&ADC_InitStructure);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrig = ADC_ExternalTrig_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NumOfChannel = 1;
    ADC_Init(ADCx, &ADC_InitStructure);

    ADC_Ctrl(ADCx, ENABLE);
    ADC_RstCalibration(ADCx);
    while(ADC_GetResetCalibrationStatus(ADCx));
    ADC_StartCalibration(ADCx);
    while(ADC_GetCalibrationStatus(ADCx));
}

/**
  * @brief  获取 ADC 值
  * @param  ADCx: ADC地址
  * @param  ADC_Channel: ADC通道
  * @retval 无
  */
uint16_t ADCx_GetValue(ADC_Type* ADCx, uint16_t ADC_Channel)
{
    ADC_RegularChannelConfig(ADCx, ADC_Channel, 1, ADC_SampleTime_41_5);

    ADC_SoftwareStartConvCtrl(ADCx, ENABLE);
    while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EC));
    return ADC_GetConversionValue(ADCx);
}
