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
#include "Service/HAL/HAL.h"
#include "Utils/CommonMacro/CommonMacro.h"
#include "Utils/easing/easing.h"
#include <cmath>

#define TIMER_PERIOD 30

// LN(1000) = 6.9077552789821f
#define PSYCHO2PHYSICAL 6.9077552789821f

using namespace DataProc;

class DP_Backlight {
public:
    DP_Backlight(DataNode* node);

private:
    DataNode* _node;
    const DataNode* _nodePower;
    const DataNode* _nodeStorage;
    const DataNode* _nodeSunRise;
    DeviceObject* _dev;
    easing_t _easing;
    int _targetBrightness;
    int _currentBrightness;
    int _dayBrightness;
    int _nightBrightness;

private:
    int onEvent(DataNode::EventParam_t* param);
    void onTimer();
    void setBacklight(int brightness, bool anim, bool disable = false);
    void setBacklight(int brightness);
    void updateAutoBrightness(const Backlight_Info_t* info);
};

DP_Backlight::DP_Backlight(DataNode* node)
    : _node(node)
    , _nodePower(nullptr)
    , _dev(nullptr)
    , _targetBrightness(500)
    , _currentBrightness(0)
    , _dayBrightness(500)
    , _nightBrightness(500)
{
    _dev = HAL::Manager()->getDevice("Backlight");
    if (!_dev) {
        return;
    }

    easing_init(
        &_easing,
        EASING_MODE_DEFAULT,
        _easing_calc_Linear,
        0,
        TIMER_PERIOD,
        0);
    easing_set_tick_callback(HAL::GetTick);

    _nodePower = node->subscribe("Power");
    _nodeStorage = node->subscribe("Storage");
    _nodeSunRise = node->subscribe("SunRise");

    Storage_Helper storage(node);
    storage.structStart("backlight");
    storage.add("current", &_targetBrightness);
    storage.add("day", &_dayBrightness);
    storage.add("night", &_nightBrightness);
    storage.structEnd();

    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_Backlight*)n->getUserData();
            return ctx->onEvent(param);
        });
}

int DP_Backlight::onEvent(DataNode::EventParam_t* param)
{
    auto info = (Backlight_Info_t*)param->data_p;

    switch (param->event) {
    case DataNode::EVENT_TIMER:
        onTimer();
        break;
    case DataNode::EVENT_NOTIFY:
        if (!info->disable && (info->brightnessDay >= 0 || info->brightnessNight >= 0)) {
            updateAutoBrightness(info);
        } else {
            setBacklight(info->brightness, info->anim, info->disable);
        }
        break;
    case DataNode::EVENT_PULL:
        info->brightness = _targetBrightness;
        info->anim = false;
        info->brightnessDay = _dayBrightness;
        info->brightnessNight = _nightBrightness;
        break;
    case DataNode::EVENT_PUBLISH:
        if (param->tran == _nodePower) {
            auto powerInfo = (const Power_Info_t*)param->data_p;
            if (powerInfo->isReadyToShutdown) {
                /* shutdown */
                setBacklight(0, true);
            }
        } else if (param->tran == _nodeStorage) {
            auto storageInfo = (const Storage_Info_t*)param->data_p;
            if (storageInfo->cmd == STORAGE_CMD::LOAD) {
                /* load success */
                setBacklight(_targetBrightness, true);
            }
        } else if (param->tran == _nodeSunRise) {
            auto sunRiseInfo = (const SunRise_Info_t*)param->data_p;
            if (sunRiseInfo->state == SUNRISE_STATE::DAY) {
                setBacklight(_dayBrightness, true);
            } else if (sunRiseInfo->state == SUNRISE_STATE::NIGHT) {
                setBacklight(_nightBrightness, true);
            }
        }
        break;

    default:
        break;
    }

    return DataNode::RES_OK;
}

void DP_Backlight::onTimer()
{
    easing_update(&_easing);
    int pos = easing_curpos(&_easing);

    setBacklight(pos);

    /* when animation is finish, stop timer */
    if (pos == _targetBrightness) {
        _node->stopTimer();
    }
}

void DP_Backlight::setBacklight(int brightness)
{
    /* record current brightness */
    _currentBrightness = brightness;

    /* gamma correction */
    if (brightness != 0 && brightness != 1000) {
        brightness = (int)expf((float)brightness / 1000.0f * PSYCHO2PHYSICAL);
    }

    _dev->write(&brightness, sizeof(brightness));
}

void DP_Backlight::setBacklight(int brightness, bool anim, bool disable)
{
    if (disable) {
        /* stop animation */
        easing_stop(&_easing, _targetBrightness);
        _node->stopTimer();
        setBacklight(0);
        return;
    }

    CM_VALUE_LIMIT(brightness, 0, 1000);
    _targetBrightness = brightness;

    /* start animation */
    if (anim) {
        easing_start_absolute(&_easing, _currentBrightness, _targetBrightness);
        _node->startTimer(TIMER_PERIOD);
        return;
    }

    /* stop animation */
    easing_stop(&_easing, _targetBrightness);
    _node->stopTimer();

    /* set backlight directly */
    setBacklight(_targetBrightness);
}

void DP_Backlight::updateAutoBrightness(const Backlight_Info_t* info)
{
    if (info->brightnessDay >= 0) {
        _dayBrightness = info->brightnessDay;
    }

    if (info->brightnessNight >= 0) {
        _nightBrightness = info->brightnessNight;
    }

    SunRise_Info_t sunRiseInfo;
    if (_node->pull(_nodeSunRise, &sunRiseInfo, sizeof(sunRiseInfo)) != DataNode::RES_OK) {
        return;
    }

    if (sunRiseInfo.state == SUNRISE_STATE::DAY) {
        setBacklight(_dayBrightness, info->anim);
    } else if (sunRiseInfo.state == SUNRISE_STATE::NIGHT) {
        setBacklight(_nightBrightness, info->anim);
    }
}

DATA_PROC_DESCRIPTOR_DEF(Backlight)
