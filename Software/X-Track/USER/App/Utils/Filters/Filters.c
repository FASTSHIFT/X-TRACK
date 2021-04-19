#include "Filters.h"

#define FILTER_ABS(x)  (((x)>0)?(x):-(x))                      //取绝对值

/************************************************
*函数名称:LowpassFilter_Init()
*功能描述:低通滤波器初始化
*输入参数:
        filter: 滤波器地址
            dt: 采样周期(单位：秒)
        cutoff: 截至频率(单位：Hz)
*输出参数:
*调用函数:
*详细描述:
************************************************/
void LowpassFilter_Init(LowpassFilter_TypeDef *filter, float dt, float cutoff)
{
    filter->IsFirst = 1;
    filter->dT = dt;
    if(cutoff > 0.001f)                                    //判断截止频率
    {
        float RC = 1 / (2 * 3.141592653f * cutoff);
        filter->RCRate = filter->dT / (RC + filter->dT);   //计算RC系数
    }
    else
    {
        filter->RCRate = 1;   //如果截止频率太低，就将RC系数设置为1，不进行滤波
    }
}

/************************************************
*函数名称:LowpassFilter_GetNext()
*功能描述:低通滤波器输出
*输入参数:
        filter: 滤波器地址
         value: 被滤波的原始值
*输出参数:滤波后的值
*调用函数:
*详细描述:
************************************************/
float LowpassFilter_GetNext(LowpassFilter_TypeDef *filter, float value)
{
    if(filter->IsFirst)
    {
        filter->IsFirst = 0;
        return filter->LastValue = value;               //如果为第一次取值，则返回原数据
    }
    else
    {
        filter->LastValue = (filter->LastValue + (value - filter->LastValue) * filter->RCRate);
        return filter->LastValue;    //滤波输出
    }
}

/************************************************
*函数名称:BubbleSort()
*功能描述:冒泡排序
*输入参数:
        arr: 被排序的数组
          n: 数据长度
*输出参数:
*调用函数:
*详细描述:被中值滤波器调用
************************************************/
static void BubbleSort(int32_t *arr, uint32_t n)
{
    int32_t temp = 0;
    uint32_t i, j;
    for(i = 0; i < n - 1; i++)
    {
        for(j = 0; j < n - 1 - i; j++)
        {
            if(arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

/************************************************
*函数名称:MedianFilter_Init()
*功能描述:中值滤波器初始化
*输入参数:
        filter: 滤波器地址
        buffer: 缓冲区地址
    bufferSize: 缓冲区长度
*输出参数:
*调用函数:
*详细描述:
************************************************/
void MedianFilter_Init(MedianFilter_TypeDef *filter, int32_t *buffer, uint32_t bufferSize)
{
    uint32_t i = 0;
    filter->IsFirst = 1;
    filter->Buffer = buffer;
    filter->BufferSize = bufferSize;
    filter->DataIndex = 0;
    filter->LastValue = 0;

    for(i = 0; i < bufferSize; i++)   //中值滤波器缓存数组初始化，全部清零
    {
        buffer[i] = 0;
    }
}

/************************************************
*函数名称:MedianFilter_GetNext()
*功能描述:中值滤波器输出
*输入参数:
        filter: 滤波器地址
         value: 被滤波的原始值
*输出参数:滤波后的值
*调用函数:
*详细描述:
************************************************/
int32_t MedianFilter_GetNext(MedianFilter_TypeDef *filter, int32_t value)
{
    if(filter->IsFirst == 1 && filter->DataIndex < filter->BufferSize)//首次先填满缓冲区
    {
        filter->Buffer[filter->DataIndex] = value;             //填入缓冲区

        filter->LastValue = value;                             //如果为第一次取值，则返回原数据

        if(filter->DataIndex >= (filter->BufferSize - 1))      //缓冲区是否填满
        {
            filter->IsFirst = 0;
        }
        filter->DataIndex++;                                   //数据索引累加
    }
    else
    {
        if(filter->DataIndex >= filter->BufferSize)                     //如果缓冲区被填满，则开始中值滤波
        {
            BubbleSort(filter->Buffer, filter->BufferSize);             //对缓冲区内的数据进行排序

            filter->LastValue = filter->Buffer[filter->BufferSize / 2]; //保存滤波后的值
            filter->DataIndex = 0;                                      //数据缓存计数清0
        }

        filter->Buffer[filter->DataIndex] = value;             //填入缓冲区
        filter->DataIndex++;                                   //数据缓存计数自加
    }

    return filter->LastValue;                                  //返回滤波后的值
}

/************************************************
*函数名称:MedianQueueFilter_Init()
*功能描述:中值队列滤波器初始化
*输入参数:
        filter: 滤波器地址
        buffer: 缓冲区地址
        bufferSort: 排序缓冲区地址
        bufferSize: 缓冲区长度
*输出参数:
*调用函数:
*详细描述:
************************************************/
void MedianQueueFilter_Init(MedianQueueFilter_TypeDef *filter, int32_t *buffer, int32_t *bufferSort, uint32_t bufferSize)
{
    uint32_t i = 0;
    filter->IsFirst = 1;
    filter->Buffer = buffer;
    filter->BufferSort = bufferSort;
    filter->BufferSize = bufferSize;
    filter->DataIndex = 0;
    filter->LastValue = 0;

    for(i = 0; i < bufferSize; i++)   //中值滤波器缓存数组初始化，全部清零
    {
        buffer[i] = 0;
        bufferSort[i] = 0;
    }
}

/************************************************
*函数名称:MedianQueueFilter_GetNext()
*功能描述:中值队列滤波器输出
*输入参数:
        filter: 滤波器地址
         value: 被滤波的原始值
*输出参数:滤波后的值
*调用函数:
*详细描述:
************************************************/
int32_t MedianQueueFilter_GetNext(MedianQueueFilter_TypeDef *filter, int32_t value)
{
    if(filter->IsFirst == 1 && filter->DataIndex < filter->BufferSize)//首次先填满缓冲区
    {
        filter->Buffer[filter->DataIndex] = value;             //填入缓冲区

        filter->LastValue = value;                             //如果为第一次取值，则返回原数据

        if(filter->DataIndex >= (filter->BufferSize - 1))      //缓冲区是否填满
        {
            filter->IsFirst = 0;
        }
        filter->DataIndex++;                                   //数据索引累加
    }
    else
    {
        uint32_t i = 0;

        filter->DataIndex %= filter->BufferSize;               //防止索引越界
        filter->Buffer[filter->DataIndex] = value;             //填入缓冲区
        filter->DataIndex++;                                   //数据索引累加

        for(i = 0; i < filter->BufferSize; i++)                //拷贝至排序缓冲区
        {
            filter->BufferSort[i] = filter->Buffer[i];
        }
        BubbleSort(filter->BufferSort, filter->BufferSize);    //对缓冲区内的数据进行排序
        filter->LastValue = filter->BufferSort[(filter->BufferSize / 2)];//取中值
    }

    return filter->LastValue;                                //返回滤波后的值
}

/************************************************
*函数名称:HysteresisFilter_Init()
*功能描述:滞回滤波器初始化
*输入参数:
        filter: 滤波器地址
        hysValue: 滞回系数
*输出参数:
*调用函数:
*详细描述:
************************************************/
void HysteresisFilter_Init(HysteresisFilter_TypeDef *filter, int32_t hysValue)
{
    filter->HysValue = hysValue;
}

/************************************************
*函数名称:HysteresisFilter_GetNext()
*功能描述:滞回滤波器输出
*输入参数:
        filter: 滤波器地址
         value: 被滤波的原始值
*输出参数:滤波后的值
*调用函数:
*详细描述:
************************************************/
int32_t HysteresisFilter_GetNext(HysteresisFilter_TypeDef *filter, int32_t value)
{
    if(FILTER_ABS(value - filter->LastValue) > filter->HysValue)   //波动范围是否大于滞回系数
    {
        filter->LastValue = value;                          //如果大于则保存
    }
    return filter->LastValue;                               //返回滤波后的值
}

/************************************************
*函数名称:SlidingFilter_Init()
*功能描述:滞回滤波器初始化
*输入参数:
        filter: 滤波器地址
        sldValue: 滑动系数
*输出参数:
*调用函数:
*详细描述:
************************************************/
void SlidingFilter_Init(SlidingFilter_TypeDef *filter, uint32_t sldValue)
{
    filter->IsFirst = 1;
    filter->SlideValue = sldValue;
    filter->LastValue = 0;
}

/************************************************
*函数名称:SlidingFilter_GetNext()
*功能描述:滑动滤波器输出
*输入参数:
        filter: 滤波器地址
         value: 被滤波的原始值
*输出参数:滤波后的值
*调用函数:
*详细描述:
************************************************/
int32_t SlidingFilter_GetNext(SlidingFilter_TypeDef *filter, int32_t value)
{
    if(filter->IsFirst == 1)
    {
        filter->IsFirst = 0;                                                //如果为第一次取值，则返回原数据
        filter->LastValue = value;
    }
    else
    {
        if(FILTER_ABS(value - filter->LastValue) < filter->SlideValue)      //如果变化量小于滑动系数
        {
            filter->LastValue = value;                                      //直接赋值
        }
        else
        {
            if(filter->LastValue < value)                                   //滑动接近
            {
                filter->LastValue += filter->SlideValue;
            }
            else if(filter->LastValue > value)
            {
                filter->LastValue -= filter->SlideValue;
            }
        }
    }
    return filter->LastValue;                                               //返回滤波后的值
}
