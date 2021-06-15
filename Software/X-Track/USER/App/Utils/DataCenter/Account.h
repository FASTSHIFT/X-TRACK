/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
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
#ifndef __ACCOUNT_H
#define __ACCOUNT_H

#include <stdint.h>
#include <vector>
#include "PingPongBuffer/PingPongBuffer.h"
#include "lvgl/lvgl.h"

class DataCenter;

class Account
{
public:
    typedef enum
    {
        EVENT_NONE,
        EVENT_PUB_PUBLISH,
        EVENT_SUB_PULL,
        EVENT_NOTIFY,
        EVENT_TIMER,
        _EVENT_LAST
    } EventCode_t;

    typedef enum
    {
        ERROR_NONE = 0,
        ERROR_UNKNOW = -1,
        ERROR_SIZE_MISMATCH = -2,
        ERROR_UNSUPPORTED_REQUEST = -3,
        ERROR_NO_CALLBACK = -4,
        ERROR_NO_CACHE = -5,
        ERROR_NO_COMMITED = -6,
        ERROR_NOT_FOUND = -7,
        ERROR_PARAM_ERROR = -8
    } ErrorCode_t;

    typedef struct
    {
        EventCode_t event;
        Account* tran;
        Account* recv;
        void* data_p;
        uint32_t size;
    } EventParam_t;

    typedef int (*EventCallback_t)(Account* account, EventParam_t* param);

public:
    Account(
        const char* id,
        DataCenter* center,
        uint32_t bufSize = 0,
        void* userData = nullptr
    );
    ~Account();

    /* 关注 */
    Account* Subscribe(const char* pubID);

    /* 取关 */
    bool Unsubscribe(const char* pubID);

    /* 提交 */
    bool Commit(const void* data_p, uint32_t size);

    /* 推送 */
    int Publish();

    /* 拉取 */
    int Pull(const char* pubID, void* data_p, uint32_t size);
    int Pull(Account* pub, void* data_p, uint32_t size);

    /* 私信 */
    int Notify(const char* pubID, const void* data_p, uint32_t size);
    int Notify(Account* pub, const void* data_p, uint32_t size);

    /* 事件回调 */
    void SetEventCallback(EventCallback_t callback);

    /* 定时上报 */
    void SetTimerPeriod(uint32_t period);

    /* 定时器使能 */
    void SetTimerEnable(bool en);

    /* 关注UP数 */
    uint32_t GetPublisherSize();

    /* 粉丝数 */
    uint32_t GetSubscribeSize();

public:
    const char* ID;      /* B站帐号 */
    DataCenter* Center;  /* B站 */
    void* UserData;

    std::vector<Account*> publishers;  /* 关注列表 */
    std::vector<Account*> subscribers; /* 粉丝列表 */

    struct
    {
        EventCallback_t eventCallback;  /* 事件 */

        lv_timer_t* timer;

        PingPongBuffer_t BufferManager;
        uint32_t BufferSize;
    } priv;

private:
    static void TimerCallbackHandler(lv_timer_t* task);
};

#endif
