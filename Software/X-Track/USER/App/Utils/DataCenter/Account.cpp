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
#include "Account.h"
#include "DataCenter.h"
#include "DataCenterLog.h"

Account::Account(
    const char* id,
    DataCenter* center,
    uint32_t bufSize,
    void* userData
)
{
    memset(&priv, 0, sizeof(priv));

    ID = id;
    Center = center;
    UserData = userData;

    if (bufSize != 0)
    {
        uint8_t* buf0 = new uint8_t[bufSize];

        if (!buf0)
        {
            DC_LOG_ERROR("Account[%s] buf0 malloc failed!", ID);
            return;
        }

        uint8_t* buf1 = new uint8_t[bufSize];

        if (!buf1)
        {
            delete [] buf0;
            DC_LOG_ERROR("Account[%s] buf1 malloc failed!", ID);
            return;
        }

        PingPongBuffer_Init(&priv.BufferManager, buf0, buf1);
        DC_LOG_INFO("Account[%s] cached %d x2 bytes", ID, bufSize);
    }

    priv.BufferSize = bufSize;

    Center->AddAccount(this);

    DC_LOG_INFO("Account[%s] created", ID);
}

Account::~Account()
{
    DC_LOG_INFO("Account[%s] deleting...", ID);

    /* 删除定时刷新任务 */
    if (priv.timer)
    {
        lv_timer_del(priv.timer);
        DC_LOG_INFO("Account[%s] task deleted", ID);
    }

    /* 让我的粉丝们取关我这个UP */
    for(auto iter : subscribers)
    {
        iter->Unsubscribe(ID);
        DC_LOG_INFO("sub[%s] unsubscribed pub[%s]", iter->ID, ID);
    }

    /* 让我关注的UP们删除我这个粉丝 */
    for (auto iter : publishers)
    {
        Center->Remove(&iter->subscribers, this);
        DC_LOG_INFO("pub[%s] removed sub[%s]", iter->ID, ID);
    }

    /* 让B站删号*/
    Center->RemoveAccount(this);
    DC_LOG_INFO("Account[%s] deleted", ID);
}

Account* Account::Subscribe(const char* pubID)
{
    /* 不允许订阅自己 */
    if (strcmp(pubID, ID) == 0)
    {
        DC_LOG_ERROR("Account[%s] try to subscribe to it itself", ID);
        return nullptr;
    }

    /* 是否重复关注 */
    Account* pub = Center->Find(&publishers, pubID);
    if(pub != nullptr)
    {
        DC_LOG_ERROR("Multi subscribe pub[%s]", pubID);
        return nullptr;
    }

    /* B站是否有号 */
    pub = Center->SearchAccount(pubID);
    if (pub == nullptr)
    {
        DC_LOG_ERROR("pub[%s] was not found", pubID);
        return nullptr;
    }

    /* 将UP加至我的关注列表 */
    publishers.push_back(pub);

    /* 让UP添加一个我这个粉丝 */
    pub->subscribers.push_back(this);

    DC_LOG_INFO("sub[%s] subscribed pub[%s]", ID, pubID);

    return pub;
}

bool Account::Unsubscribe(const char* pubID)
{
    /* 我是否有关注这个UP */
    Account* pub = Center->Find(&publishers, pubID);
    if (pub == nullptr)
    {
        DC_LOG_WARN("sub[%s] was not subscribe pub[%s]", ID, pubID);
        return false;
    }

    /* 将UP移出我关注列表 */
    Center->Remove(&publishers, pub);

    /* UP删除我这个粉丝 */
    Center->Remove(&pub->subscribers, this);

    return true;
}

bool Account::Commit(const void* data_p, uint32_t size)
{
    if (!size || size != priv.BufferSize)
    {
        DC_LOG_ERROR("pub[%s] has not cache", ID);
        return false;
    }

    void* wBuf;
    PingPongBuffer_GetWriteBuf(&priv.BufferManager, &wBuf);

    memcpy(wBuf, data_p, size);

    PingPongBuffer_SetWriteDone(&priv.BufferManager);

    DC_LOG_INFO("pub[%s] commit data(0x%p)[%d] >> data(0x%p)[%d] done",
                ID, data_p, size, wBuf, size);

    return true;
}

int Account::Publish()
{
    int retval = ERROR_UNKNOW;

    if (priv.BufferSize == 0)
    {
        DC_LOG_ERROR("pub[%s] has not cache", ID);
        return ERROR_NO_CACHE;
    }

    void* rBuf;
    if (!PingPongBuffer_GetReadBuf(&priv.BufferManager, &rBuf))
    {
        DC_LOG_WARN("pub[%s] data was not commit", ID);
        return ERROR_NO_COMMITED;
    }

    EventParam_t param;
    param.event = EVENT_PUB_PUBLISH;
    param.tran = this;
    param.recv = nullptr;
    param.data_p = rBuf;
    param.size = priv.BufferSize;

    /* 向粉丝推送消息 */
    for(auto iter : subscribers)
    {
        Account* sub = iter;
        EventCallback_t callback = sub->priv.eventCallback;

        DC_LOG_INFO("pub[%s] push >> data(0x%p)[%d] >> sub[%s]...",
                    ID, param.data_p, param.size, sub->ID);

        if (callback != nullptr)
        {
            param.recv = sub;
            int ret = callback(sub, &param);

            DC_LOG_INFO("push done: %d", ret);
            retval = ret;
        }
        else
        {
            DC_LOG_INFO("sub[%s] not register callback", sub->ID);
        }
    }

    PingPongBuffer_SetReadDone(&priv.BufferManager);

    return retval;
}

int Account::Pull(const char* pubID, void* data_p, uint32_t size)
{
    Account* pub = Center->Find(&publishers, pubID);
    if (pub == nullptr)
    {
        DC_LOG_ERROR("sub[%s] was not subscribe pub[%s]", ID, pubID);
        return ERROR_NOT_FOUND;
    }
    return Pull(pub, data_p, size);
}

int Account::Pull(Account* pub, void* data_p, uint32_t size)
{
    int retval = ERROR_UNKNOW;

    if (pub == nullptr)
    {
        return ERROR_NOT_FOUND;
    }

    DC_LOG_INFO("sub[%s] pull << data(0x%p)[%d] << pub[%s] ...",
                ID, data_p, size, pub->ID);

    EventCallback_t callback = pub->priv.eventCallback;
    if (callback != nullptr)
    {
        EventParam_t param;
        param.event = EVENT_SUB_PULL;
        param.tran = this;
        param.recv = pub;
        param.data_p = data_p;
        param.size = size;

        int ret = callback(pub, &param);

        DC_LOG_INFO("pull done: %d", ret);
        retval = ret;
    }
    else
    {
        DC_LOG_INFO("pub[%s] not registed pull callback, read commit cache...", pub->ID);

        if (pub->priv.BufferSize == size)
        {
            void* rBuf;
            if (PingPongBuffer_GetReadBuf(&pub->priv.BufferManager, &rBuf))
            {
                memcpy(data_p, rBuf, size);
                PingPongBuffer_SetReadDone(&pub->priv.BufferManager);
                DC_LOG_INFO("read done");
                retval = 0;
            }
            else
            {
                DC_LOG_WARN("pub[%s] data was not commit!", pub->ID);
            }
        }
        else
        {
            DC_LOG_ERROR(
                "Data size pub[%s]:%d != sub[%s]:%d",
                pub->ID,
                pub->priv.BufferSize,
                this->ID,
                size
            );
        }
    }

    return retval;
}

int Account::Notify(const char* pubID, const void* data_p, uint32_t size)
{
    Account* pub = Center->Find(&publishers, pubID);
    if (pub == nullptr)
    {
        DC_LOG_ERROR("sub[%s] was not subscribe pub[%s]", ID, pubID);
        return ERROR_NOT_FOUND;
    }
    return Notify(pub, data_p, size);
}

int Account::Notify(Account* pub, const void* data_p, uint32_t size)
{
    int retval = ERROR_UNKNOW;

    if (pub == nullptr)
    {
        return ERROR_NOT_FOUND;
    }

    DC_LOG_INFO("sub[%s] notify >> data(0x%p)[%d] >> pub[%s] ...",
                ID, data_p, size, pub->ID);

    EventCallback_t callback = pub->priv.eventCallback;
    if (callback != nullptr)
    {
        EventParam_t param;
        param.event = EVENT_NOTIFY;
        param.tran = this;
        param.recv = pub;
        param.data_p = (void*)data_p;
        param.size = size;

        int ret = callback(pub, &param);

        DC_LOG_INFO("send done: %d", ret);
        retval = ret;
    }
    else
    {
        DC_LOG_WARN("pub[%s] not register callback", pub->ID);
        retval = ERROR_NO_CALLBACK;
    }

    return retval;
}

void Account::SetEventCallback(EventCallback_t callback)
{
    priv.eventCallback = callback;
}

void Account::TimerCallbackHandler(lv_timer_t* timer)
{
    Account* instance = (Account*)(timer->user_data);
    EventCallback_t callback = instance->priv.eventCallback;
    if(callback)
    {
        EventParam_t param;
        param.event = EVENT_TIMER;
        param.tran = instance;
        param.recv = instance;
        param.data_p = nullptr;
        param.size = 0;

        callback(instance, &param);
    }
}

void Account::SetTimerPeriod(uint32_t period)
{
    if(priv.timer)
    {
        lv_timer_del(priv.timer);
        priv.timer = nullptr;
    }

    if(period == 0)
    {
        return;
    }

    priv.timer = lv_timer_create(
                     TimerCallbackHandler,
                     period,
                     this
                 );
}

void Account::SetTimerEnable(bool en)
{
    lv_timer_t* timer = priv.timer;

    if (timer == nullptr)
    {
        return;
    }

    en ? lv_timer_resume(timer) : lv_timer_pause(timer);
}

uint32_t Account::GetPublisherSize()
{
    return publishers.size();
}

uint32_t Account::GetSubscribeSize()
{
    return subscribers.size();
}
