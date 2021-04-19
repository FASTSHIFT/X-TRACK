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
#include "Account.h"
#include "DataCenter.h"
#include "DataCenterLog.h"

Account::Account(const char* id, DataCenter* center, void* userData)
{
    memset(&priv, 0, sizeof(priv));

    ID = id;
    Center = center;
    UserData = userData;

    Center->AddAccount(this);

    DC_LOG_INFO("Account[%s] created", ID);
}

Account::~Account()
{
    DC_LOG_INFO("Account[%s] deleting...", ID);

    /* 删除定时刷新任务 */
    if (priv.task)
    {
        lv_task_del(priv.task);
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

bool Account::Subscribe(const char* pubID, DataCallback_t callback)
{
    /* 不允许订阅自己 */
    if (strcmp(pubID, ID) == 0)
    {
        DC_LOG_ERROR("Account[%s] try to subscribe to it itself", ID);
        return false;
    }

    /* 是否重复关注 */
    Account* pub = Center->Find(&publishers, pubID);
    if(pub != nullptr)
    {
        DC_LOG_ERROR("Multi subscribe pub[%s]", pubID);
        return false;
    }

    /* B站是否有号 */
    pub = Center->SearchAccount(pubID);
    if (pub == nullptr)
    {
        DC_LOG_ERROR("pub[%s] was not found", pubID);
        return false;
    }

    priv.subscribeCallback = callback;

    /* 将UP加至我的关注列表 */
    publishers.push_back(pub);

    /* 让UP添加一个我这个粉丝 */
    pub->subscribers.push_back(this);

    DC_LOG_INFO("sub[%s] subscribed pub[%s]", ID, pubID);

    return true;
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

int Account::Publish(int msgType, const void* data_p, uint32_t size)
{
    int retval = -1;

    /* 向粉丝推送消息 */
    for(auto iter : subscribers)
    {
        Account* sub = iter;
        DataCallback_t callback = sub->priv.subscribeCallback;

        DC_LOG_INFO("pub[%s] push >> T:%d data(0x%p)[%d] >> sub[%s]...",
            ID, msgType, data_p, size, sub->ID);
        if (callback != nullptr)
        {
            int ret = callback(
                this,
                sub,
                msgType, 
                (void*)data_p, 
                size
            );

            DC_LOG_INFO("push done: %d", ret);
            retval = ret;
        }
        else
        {
            DC_LOG_WARN("sub[%s] not register callback", sub->ID);
        }
    }
    return retval;
}

int Account::Pull(const char* pubID, int msgType, void* data_p, uint32_t size)
{
    int retval = -1;

    /* 我是否有关注这个UP */
    Account* pub = Center->Find(&publishers, pubID);
    if (pub == nullptr)
    {
        DC_LOG_ERROR("sub[%s] was not subscribe pub[%s]", ID, pubID);
        return false;
    }

    DataCallback_t callback = pub->priv.pullCallback;
    if (callback != nullptr)
    {
        DC_LOG_INFO("sub[%s] pull << T:%d data(0x%p)[%d] << pub[%s] ...",
            ID, msgType, data_p, size, pub->ID);

        int ret = callback(
            pub,
            this,
            msgType, 
            data_p, 
            size
        );

        DC_LOG_INFO("pull done: %d", ret);
        retval = ret;
    }
    else
    {
        DC_LOG_WARN("pub[%s] not register pull callback", pub->ID);
    }

    return retval;
}

void Account::SetPullCallback(DataCallback_t callback)
{
    priv.pullCallback = callback;
}

void Account::TimerCallbackHandler(lv_task_t* task)
{
    Account* instance = (Account*)(task->user_data);
    TimerCallback_t callback = instance->priv.timerCallback;
    if(callback)
    {
        callback(instance);
    }
}

void Account::SetTimerCallback(TimerCallback_t callback, uint32_t intervalTime)
{
    priv.timerCallback = callback;
    
    if(priv.task)
    {
        lv_task_del(priv.task);
    }
    
    if(callback == nullptr)
    {
        return;
    }
    
    priv.task = lv_task_create(
        TimerCallbackHandler,
        intervalTime,
        LV_TASK_PRIO_MID,
        this
    );
}

uint32_t Account::GetPublisherLen()
{
    return publishers.size();
}

uint32_t Account::GetSubscribeLen()
{
    return subscribers.size();
}
