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
#include "Service/HAL/HAL_Log.h"
#include "Service/i18n/lv_i18n.h"

using namespace DataProc;

/* clang-format off */

#define POWER_SYSTEM_VOLTAGE                (2800) // mV
#define POWER_FORCE_SHUTDOWN_VOLTAGE        (3100) // mV
#define POWER_FORCE_SHUTDOWN_TIMEOUT        (10 * 1000) // ms

/* clang-format on */

class DP_Power {
public:
    DP_Power(DataNode* node);

private:
    enum class AUTO_SHUTDOWN_STATE {
        IDLE,
        ACTIVE,
        WAIT_USER_CONFIRM,
        USER_CANCELED,
        USER_ACCEPTED,
    };

private:
    DataNode* _node;
    const DataNode* _nodeBtn;
    MsgBox_Helper _msgBox;
    Env_Helper _env;
    DeviceObject* _dev;
    Power_Info_t _info;
    uint32_t _lastTick;
    uint32_t _pressStartTick;

    uint32_t _wakeUpTick;
    int _lockCount;

    AUTO_SHUTDOWN_STATE _autoShutdownState;

private:
    int onEvent(DataNode::EventParam_t* param);
    void onPowerNotify(const Power_Info_t* info);
    void onMsgBoxNotify(const MsgBox_Info_t* info);
    void requestShutdown(uint32_t delayTime = 2000);
    void checkShutdown();
    void publishInfo();
    void notifyUserReadyToShutdown(const char* title, const char* text);
};

DP_Power::DP_Power(DataNode* node)
    : _node(node)
    , _msgBox(node)
    , _env(node)
    , _lockCount(0)
    , _autoShutdownState(AUTO_SHUTDOWN_STATE::ACTIVE)
{
    _dev = HAL::Manager()->getDevice("Power");
    if (!_dev) {
        return;
    }

    /* power on */
    _dev->ioctl(POWER_IOCMD_POWER_ON);
    int sysVoltage = POWER_SYSTEM_VOLTAGE;
    _dev->ioctl(POWER_IOCMD_SET_SYS_VOLTAGE, &sysVoltage, sizeof(sysVoltage));

    /* init info */
    Storage_Helper storage(node);
    storage.addArray("batteryUseTime", &_info.batteryUseTime, sizeof(uint32_t), 2, STORAGE_TYPE::INT);
    storage.add("autoShutdownTime", &_info.autoShutdownTime);

    _nodeBtn = node->subscribe("Button");

    _node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_Power*)n->getUserData();
            return ctx->onEvent(param);
        });
    _node->startTimer(1000);

    _lastTick = HAL::GetTick();
    _wakeUpTick = HAL::GetTick();
}

int DP_Power::onEvent(DataNode::EventParam_t* param)
{
    switch (param->event) {
    case DataNode::EVENT_TIMER:
        checkShutdown();
        publishInfo();
        break;

    case DataNode::EVENT_PULL:
        if (param->size != sizeof(Power_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }
        memcpy(param->data_p, &_info, sizeof(Power_Info_t));
        break;

    case DataNode::EVENT_NOTIFY:
        if (param->tran == _msgBox) {
            onMsgBoxNotify((MsgBox_Info_t*)param->data_p);
        } else {
            if (param->size != sizeof(Power_Info_t)) {
                return DataNode::RES_SIZE_MISMATCH;
            }
            onPowerNotify((Power_Info_t*)param->data_p);
        }
        break;

    default:
        return DataNode::RES_UNSUPPORTED_REQUEST;
    }

    return DataNode::RES_OK;
}

void DP_Power::onPowerNotify(const Power_Info_t* info)
{
    switch (info->cmd) {
    case POWER_CMD::SHUTDOWN: {
        requestShutdown(info->delayShutdownTime);
    } break;

    case POWER_CMD::REBOOT: {
        _dev->ioctl(POWER_IOCMD_REBOOT);
    } break;

    case POWER_CMD::LOCK_WAKEUP: {
        _lockCount++;
        // HAL_LOG_INFO("Lock wakeup, count = %d", _lockCount);
    } break;

    case POWER_CMD::UNLOCK_WAKEUP: {
        _lockCount--;
        // HAL_LOG_INFO("Unlock wakeup, count = %d", _lockCount);

        if (_lockCount < 0) {
            HAL_LOG_WARN("Error unlock wakeup");
            _lockCount = 0;
        }

        /* update wake up tick */
        _wakeUpTick = HAL::GetTick();
    } break;

    case POWER_CMD::KICK_WAKEUP: {
        /* update wake up tick */
        _wakeUpTick = HAL::GetTick();
    } break;

    case POWER_CMD::SET_AUTO_SHUTDOWN_TIME: {
        _info.autoShutdownTime = info->autoShutdownTime;
    } break;

    case POWER_CMD::RESET_FUEL_GAUGE: {
        _dev->ioctl(POWER_IOCMD_GAUGE_RESET);
    } break;

    default:
        break;
    }
}

void DP_Power::onMsgBoxNotify(const MsgBox_Info_t* info)
{
    switch (info->activeBtn) {
    case 0:
        _autoShutdownState = AUTO_SHUTDOWN_STATE::USER_CANCELED;
        HAL_LOG_WARN("shutdown canceled");
        break;
    case 1:
        _autoShutdownState = AUTO_SHUTDOWN_STATE::USER_ACCEPTED;
        requestShutdown();
        break;

    default:
        break;
    }
}

void DP_Power::requestShutdown(uint32_t delayTime)
{
    HAL_LOG_WARN("Shutdown requested");

    _info.isReadyToShutdown = true;

    /* delay to shutdown */
    _node->setTimerPeriod(delayTime);

    /* notify storage to save data */
    _env.set("storage", "save");

    /* publish shutdown info */
    _node->publish(&_info, sizeof(_info));
}

void DP_Power::checkShutdown()
{
    /* check shutdown request */
    if (_info.isReadyToShutdown) {
        _dev->ioctl(POWER_IOCMD_POWER_OFF);
        _node->stopTimer();
        return;
    }

    switch (_autoShutdownState) {
    case AUTO_SHUTDOWN_STATE::ACTIVE: {
        if (_info.voltage > 0 && _info.voltage < POWER_FORCE_SHUTDOWN_VOLTAGE) {
            HAL_LOG_WARN("Voltage = %dmV is too low", _info.voltage);
            _info.isBatteryLow = true;
            notifyUserReadyToShutdown(_("LOW_BATTERY"), _("SHUTDOWN?"));
        }

        /* check auto shutdown */
        if (_info.autoShutdownTime > 0 && _lockCount == 0) {
            if (HAL::GetTickElaps(_wakeUpTick) > _info.autoShutdownTime * 1000U) {
                HAL_LOG_WARN("Auto shutdown after %dsec", _info.autoShutdownTime);
                notifyUserReadyToShutdown(_("NO_OPERATION"), _("SHUTDOWN?"));
            }
        }
    } break;

    default:
        break;
    }
}

void DP_Power::publishInfo()
{
    /* if ready to shutdown, do not publish info */
    if (_info.isReadyToShutdown) {
        return;
    }

    /* read power info */
    HAL::Power_Info_t info;
    if (_dev->read(&info, sizeof(info)) != sizeof(info)) {
        return;
    }

    /* basic info */
    _info.voltage = info.voltage;
    _info.level = info.level;
    _info.isReady = info.isReady;
    _info.isCharging = info.isCharging;

    uint32_t elaps = HAL::GetTickElaps(_lastTick);
    _lastTick = HAL::GetTick();

    if (info.isCharging) {
        _info.batteryUseTime = 0;
    } else {
        _info.batteryUseTime += elaps;
    }

    _info.uptime += elaps;

    _node->publish(&_info, sizeof(_info));
}

void DP_Power::notifyUserReadyToShutdown(const char* title, const char* text)
{
    static const char* btns[] = { _("NO"), _("YES"), "" };

    MsgBox_Info_t info;
    info.title = title;
    info.txt = text;
    info.btns = btns;
    info.autoSelectBtn = 1;
    _msgBox.show(&info);

    _autoShutdownState = AUTO_SHUTDOWN_STATE::WAIT_USER_CONFIRM;
}

DATA_PROC_DESCRIPTOR_DEF(Power)
