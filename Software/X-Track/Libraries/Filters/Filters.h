#ifndef __FILTERS_H
#define __FILTERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*低通滤波器*/
typedef struct{
    float dT;            //采样周期
    uint8_t IsFirst;     //是否为第一次调用
    float RCRate;        //RC系数
    float LastValue;     //上一次的值
}LowpassFilter_TypeDef;  //低通滤波器类型定义

void LowpassFilter_Init(LowpassFilter_TypeDef *filter, float dt, float cutoff);   //低通滤波器初始化
float LowpassFilter_GetNext(LowpassFilter_TypeDef *filter, float value);          //低通滤波器输出

/*中值滤波器*/
typedef struct{
    uint8_t IsFirst;
    int32_t *Buffer;      //数据缓存地址
    uint32_t BufferSize;  //数据缓冲长度
    uint32_t DataIndex;   //当前数据索引
    int32_t LastValue;    //上一次的值
}MedianFilter_TypeDef;    //中值滤波器类型定义

void MedianFilter_Init(MedianFilter_TypeDef *filter, int32_t *buffer, uint32_t bufferSize);   //中值滤波器初始化
int32_t MedianFilter_GetNext(MedianFilter_TypeDef *filter, int32_t value);                    //中值滤波器输出

/*中值队列滤波器*/
typedef struct{
    uint8_t IsFirst;
    int32_t *Buffer;       //数据缓存地址
    int32_t *BufferSort;   //数据排序地址
    uint32_t BufferSize;   //数据缓冲长度
    uint32_t DataIndex;    //当前数据索引
    int32_t LastValue;     //上一次的值
}MedianQueueFilter_TypeDef;//中值队列滤波器类型定义
void MedianQueueFilter_Init(MedianQueueFilter_TypeDef *filter, int32_t *buffer, int32_t *bufferSort, uint32_t bufferSize);   //中值队列滤波器初始化
int32_t MedianQueueFilter_GetNext(MedianQueueFilter_TypeDef *filter, int32_t value);                    //中值队列滤波器输出

/*滞回滤波器*/
typedef struct{           
    int32_t HysValue;        //滞回系数
    int32_t LastValue;       //上一次的值
}HysteresisFilter_TypeDef;   //滞回滤波器类型定义

void HysteresisFilter_Init(HysteresisFilter_TypeDef *filter, int32_t hysValue);         //滞回滤波器初始化
int32_t HysteresisFilter_GetNext(HysteresisFilter_TypeDef *filter, int32_t value);      //滞回滤波器输出

/*滑动滤波器*/
typedef struct{
    uint8_t IsFirst;         //是否为第一次调用
    uint32_t SlideValue;     //滑动系数
    int32_t LastValue;       //上一次的值
}SlidingFilter_TypeDef;      //滑动滤波器类型定义
void SlidingFilter_Init(SlidingFilter_TypeDef *filter, uint32_t sldValue);              //滑动滤波器初始化
int32_t SlidingFilter_GetNext(SlidingFilter_TypeDef *filter, int32_t value);            //滑动滤波器输出

#ifdef __cplusplus
};
#endif

#endif
