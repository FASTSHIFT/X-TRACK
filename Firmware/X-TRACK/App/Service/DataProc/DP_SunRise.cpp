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
#include "DataProc.h"
#include "Utils/SunRiseCalc/SunRiseCalc.h"

#define IS_STR_EQ(STR1, STR2) (strcmp(STR1, STR2) == 0)

using namespace DataProc;

class DP_SunRise {
public:
    DP_SunRise(DataNode* node);

private:
    DataNode* _node;
    const DataNode* _nodeClock;
    Env_Helper _env;
    SunRise_Info_t _info;

private:
    int onEvent(DataNode::EventParam_t* param);
    int onClockEvent(const HAL::Clock_Info_t* clock);
    SunRise_Info_t getInfo(const HAL::Clock_Info_t* clock);
    SUNRISE_STATE compare(const HAL::Clock_Info_t* clock, const SunRise_Info_t* sunRise);
};

DP_SunRise::DP_SunRise(DataNode* node)
    : _node(node)
    , _env(node)
{
    node->subscribe("SportStatus");
    _nodeClock = node->subscribe("Clock");

    _node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_SunRise*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PULL | DataNode::EVENT_PUBLISH);
}

int DP_SunRise::onEvent(DataNode::EventParam_t* param)
{
    switch (param->event) {
    case DataNode::EVENT_PULL: {
        if (param->size != sizeof(SunRise_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }

        auto info = (SunRise_Info_t*)param->data_p;

        if (_info.state != SUNRISE_STATE::UNKNOWN) {
            /* use the cached value */
            *info = _info;
            return DataNode::RES_OK;
        }

        HAL::Clock_Info_t clockInfo;
        if (_node->pull(_nodeClock, &clockInfo, sizeof(clockInfo)) != DataNode::RES_OK) {
            return DataNode::RES_NO_DATA;
        }

        *info = getInfo(&clockInfo);
    } break;

    case DataNode::EVENT_PUBLISH:
        if (param->tran == _nodeClock) {
            return onClockEvent((HAL::Clock_Info_t*)param->data_p);
        } else if (param->tran == _env) {
            auto envInfo = (const Env_Info_t*)param->data_p;

            /* when timezone changed, reset the state */
            if (IS_STR_EQ(envInfo->key, "timeZone")) {
                _info.state = SUNRISE_STATE::UNKNOWN;
            }
        }
        break;

    default:
        return DataNode::RES_UNSUPPORTED_REQUEST;
    }

    return DataNode::RES_OK;
}

int DP_SunRise::onClockEvent(const HAL::Clock_Info_t* clock)
{
    if (_info.state == SUNRISE_STATE::UNKNOWN) {
        _info = getInfo(clock);
        _node->publish(&_info, sizeof(_info));
        return DataNode::RES_OK;
    }

    /* compare the state */
    auto newState = compare(clock, &_info);
    if (newState != _info.state) {
        _info.state = newState;
        _node->publish(&_info, sizeof(_info));
    }

    return DataNode::RES_OK;
}

SunRise_Info_t DP_SunRise::getInfo(const HAL::Clock_Info_t* clock)
{
    SunRise_Info_t result;

    SportStatus_Info_t sportStatus;
    if (_node->pull("SportStatus", &sportStatus, sizeof(sportStatus)) == DataNode::RES_OK) {
        SunRiseSunSetCalculator(
            _env.getInt("timeZone"),
            clock->year,
            clock->month,
            clock->day,
            sportStatus.longitude,
            sportStatus.latitude,
            &result.sunriseHour,
            &result.sunriseMinute,
            &result.sunsetHour,
            &result.sunsetMinute);

        result.state = compare(clock, &result);
    }

    return result;
}

SUNRISE_STATE DP_SunRise::compare(const HAL::Clock_Info_t* clock, const SunRise_Info_t* sunRise)
{
    if ((clock->hour > sunRise->sunriseHour || (clock->hour == sunRise->sunriseHour && clock->minute >= sunRise->sunriseMinute))
        && (clock->hour < sunRise->sunsetHour || (clock->hour == sunRise->sunsetHour && clock->minute < sunRise->sunsetMinute))) {
        return SUNRISE_STATE::DAY;
    }

    return SUNRISE_STATE::NIGHT;
}

DATA_PROC_DESCRIPTOR_DEF(SunRise)
