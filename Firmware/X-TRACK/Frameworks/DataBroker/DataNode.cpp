/*
 * MIT License
 * Copyright (c) 2021 - 2024 _VIFEXTech
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
#include "DataNode.h"
#include "DataBroker.h"
#include "DataBrokerLog.h"
#include "DataTimer.h"
#include <cstring>

DataNode::DataNode(
    const char* id,
    DataBroker* broker,
    void* userData)
    : _ID(id)
    , _broker(broker)
    , _userData(userData)
    , _eventCallback(nullptr)
    , _eventFilterMask(0)
    , _timer(nullptr)
{
    _broker->add(this);

    DN_LOG_INFO("DataNode[%s] created", _ID);
}

DataNode::~DataNode()
{
    DN_LOG_INFO("DataNode[%s] deleting...", _ID);

    /* Delete timer */
    if (_timer) {
        delete _timer;
        _timer = nullptr;
    }

    auto next = _subscribers.begin();
    decltype(next) current;

    /* Let subscribers unfollow */
    while (next != _subscribers.end()) {
        current = next++;
        DN_LOG_INFO("sub[%s] unsubscribed pub[%s]", (*current)->_ID, _ID);
        (*current)->unsubscribe(_ID);
    }

    /* Ask the publisher to delete this subscriber */
    for (auto iter : _publishers) {
        DN_LOG_INFO("pub[%s] removed sub[%s]", iter->_ID, _ID);
        _broker->remove(&iter->_subscribers, this);
    }

    /* Let the data broker delete the node */
    _broker->remove(this);
    DN_LOG_INFO("DataNode[%s] deleted", _ID);
}

const DataNode* DataNode::subscribe(const char* pubID)
{
    /* Not allowed to subscribe to itself */
    if (strcmp(pubID, _ID) == 0) {
        DN_LOG_ERROR("DataNode[%s] try to subscribe to it itself", _ID);
        return nullptr;
    }

    /* Check subscribe repeatedly */
    DataNode* pub = _broker->search(&_publishers, pubID);
    if (pub) {
        DN_LOG_WARN("Multi subscribe pub[%s]", pubID);
        return pub;
    }

    /* Check node is created */
    pub = _broker->search(pubID);
    if (!pub) {
        DN_LOG_ERROR("pub[%s] was not found", pubID);
        return nullptr;
    }

    /* Add the publisher to the subscription list */
    _publishers.push_back(pub);

    /* Let the publisher add this subscriber */
    pub->_subscribers.push_back(this);

    DN_LOG_INFO("sub[%s] subscribed pub[%s]", _ID, pubID);

    return pub;
}

bool DataNode::unsubscribe(const char* pubID)
{
    /* Whether to subscribe to the publisher */
    DataNode* pub = _broker->search(&_publishers, pubID);
    if (!pub) {
        DN_LOG_WARN("sub[%s] was not subscribe pub[%s]", _ID, pubID);
        return false;
    }

    /* Remove the publisher from the subscription list */
    _broker->remove(&_publishers, pub);

    /* Let the publisher remove this subscriber */
    _broker->remove(&pub->_subscribers, this);

    return true;
}

int DataNode::publish(const void* data_p, size_t size)
{
    int retval = RES_OK;

    EventParam_t param(this, nullptr, EVENT_PUBLISH, (void*)data_p, size);

    auto next = _subscribers.begin();
    /* Publish messages to subscribers */
    while (next != _subscribers.end()) {
        auto sub = *next++;
        param.recv = sub;

        DN_LOG_INFO("pub[%s] publish >> data(%p)[%zu] >> sub[%s]...",
            _ID, param.data_p, param.size, sub->_ID);

        retval = sendEvent(sub, &param);
        DN_LOG_INFO("publish done: %d", retval);

        /* record the last error code */
        if (retval == RES_STOP_PROCESS) {
            DN_LOG_INFO("publish stop");
            break;
        }
    }

    return retval;
}

int DataNode::pull(const char* pubID, void* data_p, size_t size)
{
    DataNode* pub = _broker->search(&_publishers, pubID);
    if (!pub) {
        DN_LOG_ERROR("sub[%s] was not subscribe pub[%s]", _ID, pubID);
        return RES_NOT_FOUND;
    }
    return pull(pub, data_p, size);
}

int DataNode::pull(const DataNode* pub, void* data_p, size_t size)
{
    if (!pub) {
        return RES_PARAM_ERROR;
    }

    DN_LOG_INFO("sub[%s] pull << data(%p)[%zu] << pub[%s] ...",
        _ID, data_p, size, pub->_ID);

    EventCallback_t callback = pub->_eventCallback;
    if (!callback) {
        return RES_NO_CALLBACK;
    }

    EventParam_t param(this, pub, EVENT_PULL, data_p, size);
    int ret = sendEvent((DataNode*)pub, &param);

    DN_LOG_INFO("pull done: %d", ret);
    return ret;
}

int DataNode::notify(const char* pubID, const void* data_p, size_t size)
{
    DataNode* pub = _broker->search(&_publishers, pubID);
    if (!pub) {
        DN_LOG_ERROR("sub[%s] was not subscribe pub[%s]", _ID, pubID);
        return RES_NOT_FOUND;
    }

    return notify(pub, data_p, size);
}

int DataNode::notify(const DataNode* pub, const void* data_p, size_t size)
{
    if (!pub) {
        return RES_PARAM_ERROR;
    }

    DN_LOG_INFO("sub[%s] notify >> data(%p)[%zu] >> pub[%s] ...",
        _ID, data_p, size, pub->_ID);

    EventParam_t param(this, pub, EVENT_NOTIFY, (void*)data_p, size);
    int ret = sendEvent((DataNode*)pub, &param);

    DN_LOG_INFO("send done: %d", ret);

    return ret;
}

void DataNode::setEventCallback(EventCallback_t callback, uint32_t eventFilter)
{
    _eventCallback = callback;
    _eventFilterMask = eventFilter;
}

void DataNode::setEventFilter(uint32_t eventFilter)
{
    _eventFilterMask = eventFilter;
}

int DataNode::onEvent(EventParam_t* param)
{
    if (!_eventCallback) {
        DN_LOG_INFO("DataNode[%s] not register callback", _ID);
        return RES_NO_CALLBACK;
    }

    return _eventCallback(this, param);
}

int DataNode::sendEvent(DataNode* node, EventParam_t* param)
{
    if (!(node->_eventFilterMask & param->event)) {
        return RES_UNSUPPORTED_REQUEST;
    }

    return node->onEvent(param);
}

void DataNode::timerCallbackHandler(DataTimer* timer)
{
    auto instance = (DataNode*)timer->getUserData();

    EventParam_t param(instance, instance, EVENT_TIMER, nullptr, 0);
    instance->sendEvent(instance, &param);
}

void DataNode::startTimer(uint32_t period)
{
    if (_timer) {
        _timer->setPeriod(period);
        _timer->resume();
        return;
    }

    _timer = new DataTimer(_broker->timerManager(), timerCallbackHandler, period, this);
}

void DataNode::setTimerPeriod(uint32_t period)
{
    if (!_timer) {
        DN_LOG_ERROR("Timer not created");
        return;
    }

    _timer->setPeriod(period);
    _timer->resume();
}

void DataNode::resetTimer()
{
    if (!_timer) {
        DN_LOG_ERROR("Timer not created");
        return;
    }

    _timer->resume();
}

void DataNode::stopTimer()
{
    if (!_timer) {
        return;
    }

    _timer->pause();

    DN_LOG_INFO("DataNode[%s] timer stop", _ID);
}

size_t DataNode::getPublishersNumber()
{
    return _publishers.size();
}

size_t DataNode::getSubscribersNumber()
{
    return _subscribers.size();
}
