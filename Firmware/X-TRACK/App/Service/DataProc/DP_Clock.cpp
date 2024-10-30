/*
 * MIT License
 * Copyright (c) 2021 - 2023 _VIFEXTech
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
#include "Config/Config.h"
#include "DataProc.h"
#include "Service/HAL/HAL.h"
#include "Utils/Time/Time.h"
#include <stdlib.h>

#define TIMEZONE_STR "timeZone"

using namespace DataProc;

class DP_Clock {
public:
    DP_Clock(DataNode* node);

private:
    DataNode* _node;
    const DataNode* _nodeGNSS;
    const DataNode* _nodeStroage;
    Env_Helper _env;
    DeviceObject* _dev;
    int _timeZone;

private:
    int onEvent(DataNode::EventParam_t* param);
    bool calibrate(const HAL::Clock_Info_t* info);
};

DP_Clock::DP_Clock(DataNode* node)
    : _node(node)
    , _env(node)
    , _dev(nullptr)
    , _timeZone(CONFIG_TIME_ZONE_DEFAULT)
{
    _dev = HAL::Manager()->getDevice("Clock");
    if (!_dev) {
        return;
    }

    _nodeGNSS = node->subscribe("GNSS");
    _nodeStroage = node->subscribe("Storage");

    Storage_Helper storage(node);
    storage.add(TIMEZONE_STR, &_timeZone);

    _node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_Clock*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PUBLISH | DataNode::EVENT_PULL | DataNode::EVENT_TIMER);

    node->startTimer(1000);
}

int DP_Clock::onEvent(DataNode::EventParam_t* param)
{
    switch (param->event) {
    case DataNode::EVENT_PUBLISH: {
        if (param->tran == _nodeGNSS) {
            auto gnssInfo = (HAL::GNSS_Info_t*)param->data_p;

            if (!gnssInfo->isVaild) {
                return DataNode::RES_NO_DATA;
            }

            if (calibrate(&gnssInfo->clock)) {
                _node->unsubscribe("GNSS");
                _nodeGNSS = nullptr;
            }
        } else if (param->tran == _nodeStroage) {
            auto info = (const Storage_Info_t*)param->data_p;
            if (info->cmd == STORAGE_CMD::LOAD) {
                _env.setInt(TIMEZONE_STR, _timeZone);
            }
        } else if (param->tran == _env) {
            auto info = (const Env_Info_t*)param->data_p;
            if (strcmp(info->key, TIMEZONE_STR) == 0) {
                _timeZone = atoi(info->value);

                /* re-calibrate */
                _nodeGNSS = _node->subscribe("GNSS");
            }
        }

        return DataNode::RES_OK;
    }

    case DataNode::EVENT_PULL: {
        /* Clock info pull request */
        int ret = _dev->read(param->data_p, param->size);
        return ret == sizeof(HAL::Clock_Info_t) ? DataNode::RES_OK : DataNode::RES_NO_DATA;
    }

    case DataNode::EVENT_TIMER: {
        HAL::Clock_Info_t clock;
        int ret = _dev->read(&clock, sizeof(clock));
        if (ret != sizeof(clock)) {
            return DataNode::RES_NO_DATA;
        }
        return _node->publish(&clock, sizeof(clock));
    }

    default:
        break;
    }

    return DataNode::RES_UNKNOWN;
}

bool DP_Clock::calibrate(const HAL::Clock_Info_t* info)
{
    setTime(
        info->hour,
        info->minute,
        info->second,
        info->day,
        info->month,
        info->year);
    adjustTime(_timeZone * SECS_PER_HOUR);

    HAL::Clock_Info_t clock;
    clock.year = (uint16_t)year();
    clock.month = (uint8_t)month();
    clock.day = (uint8_t)day();
    clock.week = 0;
    clock.hour = (uint8_t)hour();
    clock.minute = (uint8_t)minute();
    clock.second = (uint8_t)second();
    clock.millisecond = 0;

    if (_dev->ioctl(CLOCK_IOCMD_CALIBRATE, &clock, sizeof(clock)) != DeviceObject::RES_OK) {
        return false;
    }

    return true;
}

DATA_PROC_DESCRIPTOR_DEF(Clock)
