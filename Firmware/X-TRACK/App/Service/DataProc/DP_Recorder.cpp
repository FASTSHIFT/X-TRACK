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
#include "Utils/GPX/GPX.h"
#include "lvgl/lvgl.h"

using namespace DataProc;

#define IS_STR_EQ(STR1, STR2) (strcmp(STR1, STR2) == 0)

#define RECORDER_GPX_TIME_FMT "%d-%02d-%02dT%02d:%02d:%02dZ"
#define RECORDER_GPX_MONTH_FMT "%d_%02d"
#define RECORDER_GPX_FILE_NAME CONFIG_TRACK_RECORD_FILE_DIR_NAME "/" RECORDER_GPX_MONTH_FMT "/TRK_%d%02d%02d_%02d%02d%02d.gpx"

class DP_Recorder {
public:
    DP_Recorder(DataNode* node);

private:
    DataNode* _node;
    const DataNode* _nodeClock;
    const DataNode* _nodeGNSS;
    const DataNode* _nodePower;
    const DataNode* _nodeSlope;
    Env_Helper _env;
    bool _autoRec;
    bool _autoRecFinish;
    Recorder_Info_t _recInfo;
    lv_fs_file_t _file;
    GPX _gpx;

private:
    int onEvent(DataNode::EventParam_t* param);
    int onPublish(DataNode::EventParam_t* param);
    int onNotify(Recorder_Info_t* info);
    bool onAutoRecord();
    bool recStart(uint16_t time);
    bool recStop();
    void recPoint(const HAL::GNSS_Info_t* gnssInfo, const Slope_Info_t* slopeInfo = nullptr);
    void showToast(const char* msg);
    void setEnv(const char* value);
    int notifyFilter(bool active);
    lv_fs_res_t writeFile(lv_fs_file_t* file_p, const char* str);
    int getTimeString(const char* format, char* buf, uint32_t size);
    int makeDir(const char* path);
};

DP_Recorder::DP_Recorder(DataNode* node)
    : _node(node)
    , _env(node)
    , _autoRec(true)
    , _autoRecFinish(false)
    , _file { 0 }
{
    _nodeClock = node->subscribe("Clock");
    _nodeGNSS = node->subscribe("GNSS");
    _nodePower = node->subscribe("Power");
    _nodeSlope = node->subscribe("Slope");
    node->subscribe("TrackFilter");
    node->subscribe("Toast");
    node->subscribe("Version");

    /* create Track dir */
    makeDir(CONFIG_TRACK_RECORD_FILE_DIR_NAME);

    Storage_Helper storage(node);
    storage.add("autoRec", &_autoRec);

    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_Recorder*)n->getUserData();
            return ctx->onEvent(param);
        });
}

int DP_Recorder::onEvent(DataNode::EventParam_t* param)
{
    switch (param->event) {
    case DataNode::EVENT_PUBLISH:
        return onPublish(param);

    case DataNode::EVENT_PULL:
        if (param->size != sizeof(Recorder_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }

        _recInfo.autoRec = _autoRec;
        memcpy(param->data_p, &(_recInfo), param->size);
        break;

    case DataNode::EVENT_NOTIFY:
        if (param->size != sizeof(Recorder_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }

        if (!onNotify((Recorder_Info_t*)param->data_p)) {
            return DataNode::RES_UNSUPPORTED_REQUEST;
        }
        break;

    default:
        break;
    }

    return DataNode::RES_OK;
}

int DP_Recorder::onPublish(DataNode::EventParam_t* param)
{
    if (param->tran == _nodeGNSS) {
        auto info = (HAL::GNSS_Info_t*)param->data_p;

        if (info->isVaild && _autoRec && !_autoRecFinish) {
            onAutoRecord();
        }

        if (_recInfo.active) {
            Slope_Info_t slopeInfo;
            bool hasSlope = _node->pull(_nodeSlope, &slopeInfo, sizeof(slopeInfo)) == DataNode::RES_OK;

            recPoint(info, hasSlope ? &slopeInfo : nullptr);
        }

        return DataNode::RES_OK;
    }

    if (param->tran == _nodePower) {
        auto info = (Power_Info_t*)param->data_p;
        if (info->isReadyToShutdown) {
            Recorder_Info_t recInfo;
            recInfo.active = false;
            return onNotify(&recInfo);
        }

        return DataNode::RES_OK;
    }

    if (param->tran == _env) {
        auto info = (const Env_Info_t*)param->data_p;
        if (IS_STR_EQ(info->key, "storage") && IS_STR_EQ(info->value, "save")) {
            Recorder_Info_t recInfo;
            recInfo.active = false;
            return onNotify(&recInfo);
        }

        return DataNode::RES_OK;
    }

    return DataNode::RES_UNSUPPORTED_REQUEST;
}

int DP_Recorder::onNotify(Recorder_Info_t* info)
{
    if (info->cmd == RECORDER_CMD::ENABLE_AUTO_REC) {
        _autoRec = true;
    } else if (info->cmd == RECORDER_CMD::DISABLE_AUTO_REC) {
        _autoRec = false;
    }

    if (info->active) {
        if (!recStart(info->time)) {
            return DataNode::RES_NO_DATA;
        }

        setEnv("REC");
        notifyFilter(true);
        showToast(_("START_RECORD"));
        _node->publish(info, sizeof(Recorder_Info_t));
        return DataNode::RES_OK;
    }

    if (!recStop()) {
        return DataNode::RES_NO_DATA;
    }

    setEnv("");
    notifyFilter(false);
    showToast(_("STOP_RECORD"));
    _node->publish(info, sizeof(Recorder_Info_t));
    return DataNode::RES_OK;
}

bool DP_Recorder::onAutoRecord()
{
    Recorder_Info_t info;
    info.active = true,
    info.time = 1000;
    _autoRecFinish = true;

    /* notify self to start record */
    return onNotify(&info);
}

lv_fs_res_t DP_Recorder::writeFile(lv_fs_file_t* file_p, const char* str)
{
    // LV_LOG_USER(str);

    lv_fs_res_t res = lv_fs_write(
        file_p,
        str,
        (uint32_t)strlen(str),
        nullptr);

    return res;
}

int DP_Recorder::makeDir(const char* path)
{
    auto dev = HAL::Manager()->getDevice("SdCard");
    if (!dev) {
        return -1;
    }

    auto ret = dev->ioctl(SDCARD_IOCMD_MKDIR, (void*)path, sizeof(path));

    if (ret != DeviceObject::RES_OK) {
        LV_LOG_ERROR("mkdir: %s failed %d", path, ret);
    }

    return ret;
}

int DP_Recorder::getTimeString(const char* format, char* buf, uint32_t size)
{
    HAL::Clock_Info_t clock;
    int retval = -1;
    if (_node->pull(_nodeClock, &clock, sizeof(clock)) == DataNode::RES_OK) {
        retval = snprintf(
            buf,
            size,
            format,
            clock.year,
            clock.month,
            clock.day,
            clock.hour,
            clock.minute,
            clock.second);
    }

    return retval;
}

void DP_Recorder::recPoint(const HAL::GNSS_Info_t* gnssInfo, const Slope_Info_t* slopeInfo)
{
    // LV_LOG_USER("Track recording...");

    char timeBuf[64];
    int ret = getTimeString(RECORDER_GPX_TIME_FMT, timeBuf, sizeof(timeBuf));
    if (ret < 0) {
        LV_LOG_WARN("cant't get time");
        return;
    }

    auto altitude = slopeInfo ? slopeInfo->altitude : gnssInfo->altitude;

    _gpx.setEle(String(altitude, 2));
    _gpx.setTime(timeBuf);

    String gpxStr = _gpx.getPt(
        GPX_TRKPT,
        String(gnssInfo->longitude, 6),
        String(gnssInfo->latitude, 6));

    writeFile(&(_file), gpxStr.c_str());
}

void DP_Recorder::showToast(const char* msg)
{
    Toast_Info_t info;
    info.txt = msg;
    info.duration = 2000;
    _node->notify("Toast", &info, sizeof(info));
}

bool DP_Recorder::recStart(uint16_t time)
{
    if (_recInfo.active) {
        return false;
    }

    HAL::GNSS_Info_t gnssInfo;
    if (!(_node->pull("GNSS", &gnssInfo, sizeof(gnssInfo)) == DataNode::RES_OK
            && gnssInfo.isVaild)) {
        showToast(_("GNSS_NOT_READY"));
        return false;
    }

    Version_Info_t versionInfo;
    if (_node->pull("Version", &versionInfo, sizeof(versionInfo)) != DataNode::RES_OK) {
        return false;
    }

    LV_LOG_USER("start");

    HAL::Clock_Info_t clock;
    if (_node->pull(_nodeClock, &clock, sizeof(clock)) != DataNode::RES_OK) {
        LV_LOG_ERROR("cant't get clock");
        return false;
    }

    char filepath[128];

    /* create dir */
    lv_snprintf(filepath, sizeof(filepath), CONFIG_TRACK_RECORD_FILE_DIR_NAME "/" RECORDER_GPX_MONTH_FMT,
        clock.year, clock.month);
    if (makeDir(filepath) != DeviceObject::RES_OK) {
        return false;
    }

    /* create file */
    lv_snprintf(filepath, sizeof(filepath), RECORDER_GPX_FILE_NAME,
        clock.year, clock.month,
        clock.year, clock.month, clock.day, clock.hour, clock.minute, clock.second);

    LV_LOG_USER("Track file %s opening...", filepath);

    lv_fs_res_t res = lv_fs_open(&(_file), filepath, LV_FS_MODE_WR);

    if (res != LV_FS_RES_OK) {
        LV_LOG_ERROR("failed %d", res);
        showToast(_("OPEN_FILE_FAILED"));
        return false;
    }

    GPX* gpx = &_gpx;
    lv_fs_file_t* file_p = &(_file);

    gpx->setMetaName(String(versionInfo.name) + " " + versionInfo.software);
    gpx->setMetaDesc(versionInfo.website);
    gpx->setName(filepath);
    gpx->setDesc("");

    writeFile(file_p, gpx->getOpen().c_str());
    writeFile(file_p, gpx->getMetaData().c_str());
    writeFile(file_p, gpx->getTrakOpen().c_str());
    writeFile(file_p, gpx->getInfo().c_str());
    writeFile(file_p, gpx->getTrakSegOpen().c_str());

    _recInfo.active = true;
    return true;
}

bool DP_Recorder::recStop()
{
    if (!_recInfo.active) {
        return false;
    }

    GPX* gpx = &_gpx;
    lv_fs_file_t* file_p = &(_file);

    writeFile(file_p, gpx->getTrakSegClose().c_str());
    writeFile(file_p, gpx->getTrakClose().c_str());
    writeFile(file_p, gpx->getClose().c_str());
    lv_fs_close(file_p);

    _recInfo.active = false;
    return true;
}

void DP_Recorder::setEnv(const char* value)
{
    _env.set("rec", value);
}

int DP_Recorder::notifyFilter(bool active)
{
    TrackFilter_Info_t info;
    info.active = active;
    return _node->notify("TrackFilter", &info, sizeof(info));
}

DATA_PROC_DESCRIPTOR_DEF(Recorder)
