/*
 * MIT License
 * Copyright (c) 2018-2020 _VIFEXTech
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the follo18wing conditions:
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
 
/**
  ******************************************************************************
  * @file    MillisTaskManager.h
  * @author  _VIFEXTech
  * @version v2.1
  * @date    2020-7-23
  * @brief   超轻量级分时合作式任务调度器，可以替代旧的millis()轮询方案，不依赖ArduinoAPI
  * @Upgrade 2018.7.26  v1.0 初代
  * @Upgrade 2018.10.9  v1.1 将任务状态标志位类型换成bool类型
  * @Upgrade 2018.11.17 v1.2 将typedef转移至class内部
  * @Upgrade 2018.11.18 v1.3 修复50天后数值溢出导致任务停止的bug
  * @Upgrade 2018.12.17 v1.4 将 TaskCtrl 修改为 TaskStateCtrl，添加修改任务间隔时间的接口，添加 TaskFind 用于遍历列表寻找任务
  * @Upgrade 2019.2.5   v1.5 添加析构函数，用于释放内存
  * @Upgrade 2019.3.4   v1.6 将FuncPos改为ID，添加TaskFind(void_TaskFunction_t Function)
  * @Upgrade 2019.3.21  v1.7 支持设定优先级，优先级排列为任务ID号，数字越小优先级越高
  * @Upgrade 2019.4.24  v1.8 添加GetCPU_Useage()可获得CPU占用率
  * @Upgrade 2019.5.8   v1.9 为TaskRegister添加防冲突判断
  * @Upgrade 2019.9.3   v2.0 添加TimeCost任务时间开销计算
  * @Upgrade 2020.7.23  v2.1 使用单链表管理任务，添加 GetTickElaps 处理uint32溢出问题，添加时间误差记录
  ******************************************************************************
  * @attention
  * 需要提供一个精确到毫秒级的系统时钟，然后周期调用Running函数
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MILLISTASKMANAGER_H
#define __MILLISTASKMANAGER_H

#define MTM_USE_CPU_USAGE 0

#include <stdint.h>

class MillisTaskManager
{
public:
    typedef void(*TaskFunction_t)(void);//任务回调函数
    struct Task
    {
        bool State;                //任务状态
        TaskFunction_t Function;   //任务函数指针
        uint32_t Time;             //任务时间
        uint32_t TimePrev;         //任务上一次触发时间
        uint32_t TimeCost;         //任务时间开销(us)
        uint32_t TimeError;        //误差时间
        struct Task* Next;         //下一个节点
    };
    typedef struct Task Task_t;//任务类型定义

    MillisTaskManager(bool priorityEnable = false);
    ~MillisTaskManager();

    Task_t* Register(TaskFunction_t func, uint32_t timeMs, bool state = true);
    Task_t* Find(TaskFunction_t func);
    Task_t* GetPrev(Task_t* task);
    bool Logout(TaskFunction_t func);
    bool SetState(TaskFunction_t func, bool state);
    bool SetIntervalTime(TaskFunction_t func, uint32_t timeMs);
    uint32_t GetTimeCost(TaskFunction_t func);
    uint32_t GetTickElaps(uint32_t nowTick, uint32_t prevTick);
#if (MTM_USE_CPU_USAGE == 1)
    float GetCPU_Usage();
#endif
    void Running(uint32_t tick);

private:
    Task_t* Head;        //任务链表头
    Task_t* Tail;        //任务链表尾
    bool PriorityEnable; //优先级使能
};

#endif
