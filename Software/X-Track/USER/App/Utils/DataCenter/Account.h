/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
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
#ifndef __ACCOUNT_H
#define __ACCOUNT_H

#include <stdint.h>
#include <vector>
#include "lvgl/lvgl.h"

class DataCenter;

class Account
{
public:
    typedef int (*DataCallback_t)(
        Account* pub,
        Account* sub,
        int msgType,
        void* data_p,
        uint32_t size
    );
    typedef void (*TimerCallback_t)(Account* account);

public:
    Account(const char* id, DataCenter* center, void* userData = nullptr);
    ~Account();

    /* 关注 */
    bool Subscribe(const char* pubID, DataCallback_t callback);

    /* 取关 */
    bool Unsubscribe(const char* pubID);

    /* 推送 */
    int Publish(int msgType, const void* data_p, uint32_t size);

    /* 催更 */
    int Pull(const char* pubID, int msgType, void* data, uint32_t size);

    /* UP催更入口 */
    void SetPullCallback(DataCallback_t callback);

    /* 定时上报 */
    void SetTimerCallback(TimerCallback_t callback, uint32_t intervalTime);

    /* 关注UP数 */
    uint32_t GetPublisherLen();

    /* 粉丝数 */
    uint32_t GetSubscribeLen();

public:
    const char* ID;      /* B站帐号 */
    DataCenter* Center;  /* B站 */
    void* UserData;

    std::vector<Account*> publishers;  /* 关注列表 */
    std::vector<Account*> subscribers; /* 粉丝列表 */

    struct
    {
        DataCallback_t subscribeCallback;  /* UP推送 */
        DataCallback_t pullCallback;       /* 粉丝催更 */

        lv_task_t* task;
        TimerCallback_t timerCallback;
    } priv;
        
private:
    static void TimerCallbackHandler(lv_task_t* task);
};

#endif
