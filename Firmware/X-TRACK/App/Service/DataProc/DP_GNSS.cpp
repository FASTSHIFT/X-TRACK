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
#include "Config/Config.h"
#include "DataProc.h"
#include "Service/HAL/HAL.h"
#include "Service/i18n/lv_i18n.h"
#include "lvgl/lvgl.h"
#include <cmath>

using namespace DataProc;

#define GNSS_EPHEMERIS_DEFAULT_FILE_PATH RESOURCE_MAKE_PATH("/mgaoffline.ubx")
#define GNSS_EPHEMERIS_LOAD_PERIOD 10
#define GNSS_EPHEMERIS_LOAD_BYTES 64
#define GNSS_SIGNAL_QUALITY_GOOD 7

#ifndef GNSS_ERROR_THRESHOLD
#define GNSS_ERROR_THRESHOLD 2
#endif

class DP_GNSS {
public:
    DP_GNSS(DataNode* node);

private:
    enum class STATUS {
        DISCONNECT,
        CONNECTED,
    };

private:
    DataNode* _node;
    const DataNode* _nodeGlobal;
    STATUS _nowStatus;
    STATUS _lastStatus;
    DeviceObject* _dev;
    FeedbackGen_Helper _feedback;
    MsgBox_Helper _msgbox;
    Toast_Helper _toast;
    LED_Helper _led;
    lv_timer_t* _timer;
    lv_fs_file_t _file;

#if GNSS_ERROR_THRESHOLD > 0
    int16_t _lastLongitude;
    int16_t _lastLatitude;
    int16_t _errorCount;
#endif

private:
    int onEvent(DataNode::EventParam_t* param);
    void onPublish();
    int onNotify(const GNSS_Config_Info_t* info);
    bool readGNSSInfo(HAL::GNSS_Info_t* info);
    bool loadEphemeris(const char* path);
    static void onTimer(lv_timer_t* timer);
};

DP_GNSS::DP_GNSS(DataNode* node)
    : _node(node)
    , _nowStatus(STATUS::DISCONNECT)
    , _lastStatus(STATUS::DISCONNECT)
    , _dev(nullptr)
    , _feedback(node)
    , _msgbox(node)
    , _toast(node)
    , _led(node, LED_ID::BLUE)
    , _timer(nullptr)
#if GNSS_ERROR_THRESHOLD > 0
    , _lastLongitude(0)
    , _lastLatitude(0)
    , _errorCount(0)
#endif
{
    _dev = HAL::Manager()->getDevice("GNSS");
    if (!_dev) {
        return;
    }

    _nodeGlobal = node->subscribe("Global");

    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_GNSS*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PULL | DataNode::EVENT_NOTIFY | DataNode::EVENT_PUBLISH);

    loadEphemeris(GNSS_EPHEMERIS_DEFAULT_FILE_PATH);
}

int DP_GNSS::onEvent(DataNode::EventParam_t* param)
{
    switch (param->event) {
    case DataNode::EVENT_PULL: {
        if (param->size != sizeof(HAL::GNSS_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }

        if (!readGNSSInfo((HAL::GNSS_Info_t*)param->data_p)) {
            return DataNode::RES_NO_DATA;
        }
    } break;

    case DataNode::EVENT_NOTIFY: {
        if (param->size != sizeof(GNSS_Config_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }

        return onNotify((const GNSS_Config_Info_t*)param->data_p);
    }

    case DataNode::EVENT_PUBLISH: {
        if (param->tran == _nodeGlobal) {
            auto globalInfo = (const Global_Info_t*)param->data_p;
            if (globalInfo->event == GLOBAL_EVENT::APP_RUN_LOOP_EXECUTE) {
                onPublish();
            }
        }
    } break;

    default:
        return DataNode::RES_UNSUPPORTED_REQUEST;
    }

    return DataNode::RES_OK;
}

void DP_GNSS::onPublish()
{
    if (_dev->ioctl(GNSS_IOCMD_UPDATE) != DeviceObject::RES_OK) {
        return;
    }

    HAL::GNSS_Info_t gnssInfo;
    if (!readGNSSInfo(&gnssInfo)) {
        static const LED_Squence_t seqError[] = {
            { LED_STATUS::ON, 50 },
            { LED_STATUS::OFF, 50 },
            { LED_STATUS::ON, 50 },
            { LED_STATUS::OFF, 50 },
            { LED_STATUS::ON, 50 },
            { LED_STATUS::OFF, 50 },
            { LED_STATUS::STOP, 0 },
        };
        _led.start(seqError);
        return;
    }

    int satellites = gnssInfo.satellites;

    if (satellites > GNSS_SIGNAL_QUALITY_GOOD) {
        _nowStatus = STATUS::CONNECTED;
    } else if (satellites == 0) {
        _nowStatus = STATUS::DISCONNECT;
    }

    /* on status changed */
    if (_nowStatus != _lastStatus) {
        _lastStatus = _nowStatus;
        static const FEEDBACK_GEN_EFFECT effect[] = {
            FEEDBACK_GEN_EFFECT::NOTIFICATION_ERROR,
            FEEDBACK_GEN_EFFECT::NOTIFICATION_SUCCESS
        };

        _feedback.trigger(effect[(int)_nowStatus]);

        /* publish disconnect info */
        if (_nowStatus == STATUS::DISCONNECT) {
            _node->publish(&gnssInfo, sizeof(gnssInfo));
        }
    }

    static const LED_Squence_t seqSlow[] = {
        { LED_STATUS::ON, 300 },
        { LED_STATUS::OFF, 0 },
        { LED_STATUS::STOP, 0 },
    };
    const LED_Squence_t* seq = seqSlow;

    /* publish info */
    if (gnssInfo.isVaild && satellites >= 3) {
        _node->publish(&gnssInfo, sizeof(gnssInfo));

        static const LED_Squence_t seqFast[] = {
            { LED_STATUS::ON, 50 },
            { LED_STATUS::OFF, 0 },
            { LED_STATUS::STOP, 0 },
        };
        seq = seqFast;
    }

    _led.start(seq);
}

int DP_GNSS::onNotify(const GNSS_Config_Info_t* info)
{
    switch (info->cmd) {
    case GNSS_CMD::LOAD_EPHEMERIS: {
        auto path = GNSS_EPHEMERIS_DEFAULT_FILE_PATH;

        /* use custom path */
        if (info->param.ephemerisPath) {
            path = info->param.ephemerisPath;
        }

        return loadEphemeris(path) ? DataNode::RES_OK : DataNode::RES_NO_DATA;
    }

    default:
        break;
    }

    return DataNode::RES_UNSUPPORTED_REQUEST;
}

bool DP_GNSS::readGNSSInfo(HAL::GNSS_Info_t* info)
{
    if (_dev->read(info, sizeof(HAL::GNSS_Info_t)) != sizeof(HAL::GNSS_Info_t)) {
        return false;
    }

#if GNSS_ERROR_THRESHOLD > 0
    if (_lastLatitude != 0 && _lastLongitude != 0
        && (std::fabs(info->latitude - _lastLatitude) > GNSS_ERROR_THRESHOLD
            || std::fabs(info->longitude - _lastLongitude) > GNSS_ERROR_THRESHOLD)) {

        _errorCount++;

        char buf[64];
        lv_snprintf(buf, sizeof(buf),
            "Latitude: %f\nLongitude: %f\nError Count: %d",
            info->latitude, info->longitude, _errorCount);
        static const char* btns[] = { "OK", "" };
        _msgbox.show("GNSS ERROR", buf, btns);
        return false;
    }

    _lastLatitude = info->latitude;
    _lastLongitude = info->longitude;
#endif

    return true;
}

bool DP_GNSS::loadEphemeris(const char* path)
{
    if (_timer) {
        LV_LOG_WARN("GNSS ephemeris is loading, please wait...");
        return false;
    }

    lv_fs_res_t res = lv_fs_open(&_file, path, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK) {
        LV_LOG_WARN("open %s failed: %d", path, res);
        return false;
    }

    _timer = lv_timer_create(onTimer, GNSS_EPHEMERIS_LOAD_PERIOD, this);
    return true;
}

void DP_GNSS::onTimer(lv_timer_t* timer)
{
    auto self = (DP_GNSS*)lv_timer_get_user_data(timer);

    uint32_t rd;
    uint8_t buff[GNSS_EPHEMERIS_LOAD_BYTES];
    auto res = lv_fs_read(&self->_file, buff, sizeof(buff), &rd);
    if (res != LV_FS_RES_OK) {
        goto onFinished;
    }

    if (rd < 1) {
        LV_LOG_WARN("read EOF Finished");
        self->_toast.show("%s %s", _("EPHEMERIS"), _("LOAD_SUCCESS"));
        goto onFinished;
    }

    if (self->_dev->write(buff, rd) < 0) {
        LV_LOG_ERROR("write GNSS failed");
        goto onFinished;
    }

    self->_led.on();

    return;

onFinished:
    lv_fs_close(&self->_file);
    lv_timer_del(self->_timer);
    self->_timer = nullptr;

    self->_led.off();
}

DATA_PROC_DESCRIPTOR_DEF(GNSS)
