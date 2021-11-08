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
#include "MillisTaskManager.h"

#ifndef NULL
#   define NULL 0
#endif

#define TASK_NEW(task) do{task = new Task_t;}while(0)
#define TASK_DEL(task) do{delete task;}while(0)

/**
  * @brief  初始化任务列表
  * @param  priorityEnable:设定是否开启优先级
  * @retval 无
  */
MillisTaskManager::MillisTaskManager(bool priorityEnable)
{
    PriorityEnable = priorityEnable;
    Head = NULL;
    Tail = NULL;
}

/**
  * @brief  调度器析构，释放任务链表内存
  * @param  无
  * @retval 无
  */
MillisTaskManager::~MillisTaskManager()
{
    /*移动到链表头*/
    Task_t* now = Head;
    while(true)
    {
        /*当前节点是否为空*/
        if(now == NULL)
            break;

        /*将当前节点缓存，等待删除*/
        Task_t* now_del = now;

        /*移动到下一个节点*/
        now = now->Next;

        /*删除当前节点内存*/
        TASK_DEL(now_del);
    }
}

/**
  * @brief  往任务链表添加一个任务，设定间隔执行时间
  * @param  func:任务函数指针
  * @param  timeMs:周期时间设定(毫秒)
  * @param  state:任务开关
  * @retval 任务节点地址
  */
MillisTaskManager::Task_t* MillisTaskManager::Register(TaskFunction_t func, uint32_t timeMs, bool state)
{
    /*寻找当前函数*/
    Task_t* task = Find(func);
    
    /*如果被注册*/
    if(task != NULL)
    {
        /*更新信息*/
        task->Time = timeMs;
        task->State = state;
        return task;
    }

    /*为新任务申请内存*/
    TASK_NEW(task);

    /*是否申请成功*/
    if(task == NULL)
    {
        return NULL;
    }

    task->Function = func;        //任务回调函数
    task->Time = timeMs;          //任务执行周期
    task->State = state;          //任务状态
    task->TimePrev = 0;           //上一次时间
    task->TimeCost = 0;           //时间开销
    task->TimeError = 0;          //误差时间
    task->Next = NULL;            //下一个节点
    
    /*如果任务链表为空*/
    if(Head == NULL)
    {
        /*将当前任务作为链表的头*/
        Head = task;
    }
    else
    {
        /*从任务链表尾部添加任务*/
        Tail->Next = task;
    }
    
    /*将当前任务作为链表的尾*/
    Tail = task;
    return task;
}

/**
  * @brief  寻找任务,返回任务节点
  * @param  func:任务函数指针
  * @retval 任务节点地址
  */
MillisTaskManager::Task_t* MillisTaskManager::Find(TaskFunction_t func)
{
    Task_t* now = Head;
    Task_t* task = NULL;
    while(true)
    {
        if(now == NULL)//当前节点是否为空
            break;

        if(now->Function == func)//判断函数地址是否相等
        {
            task = now;
            break;
        }

        now = now->Next;//移动到下一个节点
    }
    return task;
}

/**
  * @brief  获取当前节点的前一个节点
  * @param  task:当前任务节点地址
  * @retval 前一个任务节点地址
  */
MillisTaskManager::Task_t* MillisTaskManager::GetPrev(Task_t* task)
{
    Task_t* now = Head;    //当前节点
    Task_t* prev = NULL;   //前一节点
    Task_t* retval = NULL; //被返回的节点
    
    /*开始遍历链表*/
    while(true)
    {
        /*如果当前节点为空*/
        if(now == NULL)
        {
            /*遍历结束*/
            break;
        }
        
        /*如果当前节点与被搜索的节点匹配*/
        if(now == task)
        {
            /*保存前一个节点，准备返回*/
            retval = prev;
            break;
        }
        
        /*当前节点保存为前一节点*/
        prev = now;
        
        /*节点后移*/
        now = now->Next;
    }
    return retval;
}

/**
* @brief  注销任务（谨慎使用，线程不安全）
  * @param  func:任务函数指针
  * @retval true:成功 ; false:失败
  */
bool MillisTaskManager::Logout(TaskFunction_t func)
{
    Task_t* task = Find(func);
    if(task == NULL)
        return false;

    Task_t* prev = GetPrev(task); //前一个节点
    Task_t* next = task->Next;    //后一个节点
    
    /*如果被删除节点在链表头*/
    if(prev == NULL && next != NULL)
    {
        /*将后一个节点作为链表头*/
        Head = next;
    }
    /*如果被删除节点在链表尾*/
    else if(prev != NULL && next == NULL)
    {
        /*将前一个节点作为链表尾*/
        prev->Next = NULL;
    }
    /*如果被删除节点在链表中间*/
    else if(prev != NULL && next != NULL)
    {
        /*将前一个节点对接至后一个节点*/
        prev->Next = next;
    }
    
    /*删除当前节点*/
    TASK_DEL(task);
    
    return true;
}

/**
  * @brief  任务状态控制
  * @param  func:任务函数指针
  * @param  state:任务状态
  * @retval true:成功 ; false:失败
  */
bool MillisTaskManager::SetState(TaskFunction_t func, bool state)
{
    Task_t* task = Find(func);
    if(task == NULL)
        return false;

    task->State = state;
    return true;
}

/**
  * @brief  任务执行周期设置
  * @param  func:任务函数指针
  * @param  timeMs:任务执行周期
  * @retval true:成功 ; false:失败
  */
bool MillisTaskManager::SetIntervalTime(TaskFunction_t func, uint32_t timeMs)
{
    Task_t* task = Find(func);
    if(task == NULL)
        return false;

    task->Time = timeMs;
    return true;
}

#if (MTM_USE_CPU_USAGE == 1)
#include "Arduino.h"                //需要使用micros()
static uint32_t UserFuncLoopUs = 0; //累计时间
/**
  * @brief  获取CPU占用率
  * @param  无
  * @retval CPU占用率，0~100%
  */
float MillisTaskManager::GetCPU_Usage()
{
    static uint32_t MtmStartUs;
    float usage = (float)UserFuncLoopUs / (micros() - MtmStartUs) * 100.0f;

    if(usage > 100.0f)
        usage = 100.0f;

    MtmStartUs = micros();
    UserFuncLoopUs = 0;
    return usage;
}
#endif

/**
  * @brief  时间差判定
  * @param  nowTick:当前时间
  * @param  prevTick:上一个时间
  * @retval 时间差
  */
uint32_t MillisTaskManager::GetTickElaps(uint32_t nowTick, uint32_t prevTick)
{
    uint32_t actTime = nowTick;

    if(actTime >= prevTick)
    {
        prevTick = actTime - prevTick;
    }
    else
    {
        prevTick = /*UINT32_MAX*/0xFFFFFFFF - prevTick + 1;
        prevTick += actTime;
    }

    return prevTick;
}

/**
  * @brief  获取任务单次耗费时间(us)
  * @param  func:任务函数指针
  * @retval 任务单次耗费时间(us)
  */
uint32_t MillisTaskManager::GetTimeCost(TaskFunction_t func)
{
    Task_t* task = Find(func);
    if(task == NULL)
        return 0;

    return task->TimeCost;
}

/**
  * @brief  调度器(内核)
  * @param  tick:提供一个精确到毫秒的系统时钟变量
  * @retval 无
  */
void MillisTaskManager::Running(uint32_t tick)
{
    Task_t* now = Head;
    while(true)
    {
        /*当前节点是否为空*/
        if(now == NULL)
        {
            /*遍历结束*/
            break;
        }

        if(now->Function != NULL && now->State)
        {
            uint32_t elapsTime = GetTickElaps(tick, now->TimePrev);
            if(elapsTime >= now->Time)
            {
                /*获取时间误差，误差越大实时性越差*/
                now->TimeError = elapsTime - now->Time;
                
                /*记录时间点*/
                now->TimePrev = tick;

#if (MTM_USE_CPU_USAGE == 1)
                /*记录开始时间*/
                uint32_t start = micros();
                
                /*执行任务*/
                now->Function();
                
                /*获取执行时间*/
                uint32_t timeCost = micros() - start;
                
                /*记录执行时间*/
                now->TimeCost = timeCost;
                
                /*总时间累加*/
                UserFuncLoopUs += timeCost;
#else
                now->Function();
#endif
                
                /*判断是否开启优先级*/
                if(PriorityEnable)
                {
                    /*遍历结束*/
                    break;
                }
            }
        }

        /*移动到下一个节点*/
        now = now->Next;
    }
}
