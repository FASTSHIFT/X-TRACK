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

/**
  * @brief  Account constructor
  * @param  id:       Unique name
  * @param  center:   Pointer to the data center
  * @param  bufSize:  The length of the data to be cached
  * @param  userData: Point to the address of user-defined data
  * @retval None
  */
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

        memset(buf0, 0, bufSize);
        memset(buf1, 0, bufSize);
        PingPongBuffer_Init(&priv.BufferManager, buf0, buf1);
        DC_LOG_INFO("Account[%s] cached %d x2 bytes", ID, bufSize);
        priv.BufferSize = bufSize;
    }

    Center->AddAccount(this);

    DC_LOG_INFO("Account[%s] created", ID);
}

/**
  * @brief  Account destructor
  * @param  None
  * @retval None
  */
Account::~Account()
{
    DC_LOG_INFO("Account[%s] deleting...", ID);

    /* Release cache */
    if(priv.BufferSize)
    {
        delete priv.BufferManager.buffer[0];
        delete priv.BufferManager.buffer[1];
    }

    /* Delete timer */
    if (priv.timer)
    {
        lv_timer_del(priv.timer);
        DC_LOG_INFO("Account[%s] task deleted", ID);
    }

    /* Let subscribers unfollow */
    for(auto iter : subscribers)
    {
        iter->Unsubscribe(ID);
        DC_LOG_INFO("sub[%s] unsubscribed pub[%s]", iter->ID, ID);
    }

    /* Ask the publisher to delete this subscriber */
    for (auto iter : publishers)
    {
        Center->Remove(&iter->subscribers, this);
        DC_LOG_INFO("pub[%s] removed sub[%s]", iter->ID, ID);
    }

    /* Let the data center delete the account */
    Center->RemoveAccount(this);
    DC_LOG_INFO("Account[%s] deleted", ID);
}

/**
  * @brief  Subscribe to Publisher
  * @param  pubID: Publisher ID
  * @retval Pointer to publisher
  */
Account* Account::Subscribe(const char* pubID)
{
    /* Not allowed to subscribe to yourself */
    if (strcmp(pubID, ID) == 0)
    {
        DC_LOG_ERROR("Account[%s] try to subscribe to it itself", ID);
        return nullptr;
    }

    /* Whether to subscribe repeatedly */
    Account* pub = Center->Find(&publishers, pubID);
    if(pub != nullptr)
    {
        DC_LOG_ERROR("Multi subscribe pub[%s]", pubID);
        return nullptr;
    }

    /* BWhether the account is created */
    pub = Center->SearchAccount(pubID);
    if (pub == nullptr)
    {
        DC_LOG_ERROR("pub[%s] was not found", pubID);
        return nullptr;
    }

    /* Add the publisher to the subscription list */
    publishers.push_back(pub);

    /* Let the publisher add this subscriber */
    pub->subscribers.push_back(this);

    DC_LOG_INFO("sub[%s] subscribed pub[%s]", ID, pubID);

    return pub;
}

/**
  * @brief  Unsubscribe from publisher
  * @param  pubID: Publisher ID
  * @retval Return true if unsubscribe is successful
  */
bool Account::Unsubscribe(const char* pubID)
{
    /* Whether to subscribe to the publisher */
    Account* pub = Center->Find(&publishers, pubID);
    if (pub == nullptr)
    {
        DC_LOG_WARN("sub[%s] was not subscribe pub[%s]", ID, pubID);
        return false;
    }

    /* Remove the publisher from the subscription list */
    Center->Remove(&publishers, pub);

    /* Let the publisher add this subscriber */
    Center->Remove(&pub->subscribers, this);

    return true;
}

/**
  * @brief  Submit data to cache
  * @param  data_p: Pointer to data
  * @param  size:   The size of the data
  * @retval Return true if the submission is successful
  */
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

/**
  * @brief  Publish data to subscribers
  * @param  None
  * @retval error code
  */
int Account::Publish()
{
    int retval = RES_UNKNOW;

    if (priv.BufferSize == 0)
    {
        DC_LOG_ERROR("pub[%s] has not cache", ID);
        return RES_NO_CACHE;
    }

    void* rBuf;
    if (!PingPongBuffer_GetReadBuf(&priv.BufferManager, &rBuf))
    {
        DC_LOG_WARN("pub[%s] data was not commit", ID);
        return RES_NO_COMMITED;
    }

    EventParam_t param;
    param.event = EVENT_PUB_PUBLISH;
    param.tran = this;
    param.recv = nullptr;
    param.data_p = rBuf;
    param.size = priv.BufferSize;

    /* Publish messages to subscribers */
    for(auto iter : subscribers)
    {
        Account* sub = iter;
        EventCallback_t callback = sub->priv.eventCallback;

        DC_LOG_INFO("pub[%s] publish >> data(0x%p)[%d] >> sub[%s]...",
                    ID, param.data_p, param.size, sub->ID);

        if (callback != nullptr)
        {
            param.recv = sub;
            int ret = callback(sub, &param);

            DC_LOG_INFO("publish done: %d", ret);
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

/**
  * @brief  Pull data from the publisher
  * @param  pubID:  Publisher ID
  * @param  data_p: Pointer to data
  * @param  size:   The size of the data
  * @retval error code
  */
int Account::Pull(const char* pubID, void* data_p, uint32_t size)
{
    Account* pub = Center->Find(&publishers, pubID);
    if (pub == nullptr)
    {
        DC_LOG_ERROR("sub[%s] was not subscribe pub[%s]", ID, pubID);
        return RES_NOT_FOUND;
    }
    return Pull(pub, data_p, size);
}

int Account::Pull(Account* pub, void* data_p, uint32_t size)
{
    int retval = RES_UNKNOW;

    if (pub == nullptr)
    {
        return RES_NOT_FOUND;
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

/**
  * @brief  Send a notification to the publisher
  * @param  pubID: Publisher ID
  * @param  data_p: Pointer to data
  * @param  size:   The size of the data
  * @retval error code
  */
int Account::Notify(const char* pubID, const void* data_p, uint32_t size)
{
    Account* pub = Center->Find(&publishers, pubID);
    if (pub == nullptr)
    {
        DC_LOG_ERROR("sub[%s] was not subscribe pub[%s]", ID, pubID);
        return RES_NOT_FOUND;
    }
    return Notify(pub, data_p, size);
}

/**
  * @brief  Send a notification to the publisher
  * @param  pub:    Pointer to publisher
  * @param  data_p: Pointer to data
  * @param  size:   The size of the data
  * @retval error code
  */
int Account::Notify(Account* pub, const void* data_p, uint32_t size)
{
    int retval = RES_UNKNOW;

    if (pub == nullptr)
    {
        return RES_NOT_FOUND;
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
        retval = RES_NO_CALLBACK;
    }

    return retval;
}

/**
  * @brief  Set event callback
  * @param  callback: Callback function pointer
  * @retval None
  */
void Account::SetEventCallback(EventCallback_t callback)
{
    priv.eventCallback = callback;
}

/**
  * @brief  Timer callback entry function
  * @param  timer: Pointer to timer
  * @retval None
  */
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

/**
  * @brief  Set timing period
  * @param  period: Timing period
  * @retval None
  */
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

/**
  * @brief  Set timer enable
  * @param  en: Whether to enable
  * @retval None
  */
void Account::SetTimerEnable(bool en)
{
    lv_timer_t* timer = priv.timer;

    if (timer == nullptr)
    {
        return;
    }

    en ? lv_timer_resume(timer) : lv_timer_pause(timer);
}

/**
  * @brief  Get the number of publishers
  * @retval number of publishers
  */
size_t Account::GetPublishersSize()
{
    return publishers.size();
}

/**
  * @brief  Get the number of subscribes
  * @retval number of subscribes
  */
size_t Account::GetSubscribersSize()
{
    return subscribers.size();
}
