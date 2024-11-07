/*
 * MIT License
 * Copyright (c) 2023 - 2024 _VIFEXTech
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
#include "HAL.h"
#include "Utils/CommonMacro/CommonMacro.h"
#include "Utils/GPX_Parser/GPX_Parser.h"
#include "Utils/Geo/Geo.h"
#include "lvgl/lvgl.h"
#include <cstdlib>
#include <cstring>
#include <ctime>

#define CONFIG_GPX_FILE_PATH  RESOURCE_MAKE_PATH("/Track/TRK_EXAMPLE.gpx")
#define CONFIG_GPX_DELAY_INIT 5000
#define CONFIG_RANDOM_GNSS 0
#define CONFIG_NO_GNSS 0

namespace HAL {

class GNSS : private DeviceObject {
public:
    GNSS(const char* name)
        : DeviceObject(name)
        , _gnssInfo { 0 }
        , _parser(parserAvaliable, parserReadByte, this)
        , _file { 0 }
        , _size { 0 }
        , _prePoint { 0 }
        , _isReset(false)
        , _isUpdate(false)
    {
        _gnssInfo.longitude = CONFIG_GNSS_LONGITUDE_DEFAULT;
        _gnssInfo.latitude = CONFIG_GNSS_LATITUDE_DEFAULT;
    }

private:
    HAL::GNSS_Info_t _gnssInfo;
    HAL::GNSS_GSV_Item_t _gsvItems[12];
    GPX_Parser _parser;
    lv_fs_file_t _file;
    uint32_t _size;
    GPX_Parser::Point_t _prePoint;
    bool _isReset;
    bool _isUpdate;

private:
    virtual int onInit();
    virtual int onRead(void* buffer, size_t size);
    virtual int onWrite(const void* buffer, size_t size);
    virtual int onIoctl(DeviceObject::IO_Cmd_t cmd, void* data);
    bool parserInit();
    void paserStart();
    void paserUpdate();
    static int parserReadByte(GPX_Parser* parser);
    static int parserAvaliable(GPX_Parser* parser);
    time_t makeTime(const GPX_Parser::Time_t* time);
    double getDiffTime(const GPX_Parser::Time_t* time1, const GPX_Parser::Time_t* time2);
#if CONFIG_RANDOM_GNSS
    void makeRandomGNSSInfo(HAL::GNSS_Info_t* gnssInfo);
#endif /* CONFIG_RANDOM_GNSS */
};

int GNSS::onInit()
{
#if !CONFIG_NO_GNSS
    if (parserInit()) {
        _gnssInfo.isVaild = true;
        _gnssInfo.satellites = 10;

        /* Delay start GPX parser */
        lv_timer_t* timer = lv_timer_create(
            [](lv_timer_t* tmr) {
                auto ctx = (GNSS*)lv_timer_get_user_data(tmr);
                ctx->paserStart();
            },
            CONFIG_GPX_DELAY_INIT,
            this);
        lv_timer_set_repeat_count(timer, 1);
    }
#endif

    return DeviceObject::RES_OK;
}

int GNSS::onRead(void* buffer, size_t size)
{
    if (size != sizeof(HAL::GNSS_Info_t)) {
        return DeviceObject::RES_PARAM_ERROR;
    }

    HAL::GNSS_Info_t* gnssInfo = (HAL::GNSS_Info_t*)buffer;
    memset(gnssInfo, 0, sizeof(HAL::GNSS_Info_t));

    if (_size) {
        *gnssInfo = _gnssInfo;
    } else {
#if CONFIG_RANDOM_GNSS
        makeRandomGNSSInfo(gnssInfo);
#else
        gnssInfo->longitude = CONFIG_GNSS_LONGITUDE_DEFAULT;
        gnssInfo->latitude = CONFIG_GNSS_LATITUDE_DEFAULT;
#ifndef CONFIG_NO_GNSS
        gnssInfo->isVaild = true;
        gnssInfo->satellites = 10;
#endif /* CONFIG_NO_GNSS */
#endif /* CONFIG_RANDOM_GNSS */
    }

    /* GPGSV Info */
    gnssInfo->gsv.items = _gsvItems;
    gnssInfo->gsv.num = rand() % (CM_ARRAY_SIZE(_gsvItems) + 1);
    gnssInfo->gsv.satsInView = rand() % 10;

    DeviceObject* devClock = HAL::Manager()->getDevice("Clock");
    if (devClock) {
        devClock->read(&gnssInfo->clock, sizeof(gnssInfo->clock));
    }

    return sizeof(HAL::GNSS_Info_t);
}

int GNSS::onWrite(const void* buffer, size_t size)
{
    return size;
}

int GNSS::onIoctl(DeviceObject::IO_Cmd_t cmd, void* data)
{
    switch (cmd.full) {
    case GNSS_IOCMD_UPDATE: {
        if (_isUpdate) {
            _isUpdate = false;
            return DeviceObject::RES_OK;
        }

        return DeviceObject::RES_UNSUPPORT;
    }

    default:
        break;
    }

    return DeviceObject::RES_UNKNOWN;
}

int GNSS::parserReadByte(GPX_Parser* parser)
{
    auto ctx = (GNSS*)parser->getUserData();
    uint8_t data;
    uint32_t rd;
    lv_fs_res_t res = lv_fs_read(&ctx->_file, &data, sizeof(data), &rd);

    if (res != LV_FS_RES_OK || rd != sizeof(data)) {
        return 0;
    }

    return data;
}

int GNSS::parserAvaliable(GPX_Parser* parser)
{
    auto ctx = (GNSS*)parser->getUserData();

    uint32_t cur;
    lv_fs_res_t res = lv_fs_tell(&ctx->_file, &cur);

    if (res != LV_FS_RES_OK) {
        return 0;
    }

    return (ctx->_size - cur);
}

bool GNSS::parserInit()
{
    lv_fs_res_t res = lv_fs_open(&_file, CONFIG_GPX_FILE_PATH, LV_FS_MODE_RD);

    if (res != LV_FS_RES_OK) {
        return false;
    }

    lv_fs_seek(&_file, 0, LV_FS_SEEK_END);
    lv_fs_tell(&_file, &_size);
    lv_fs_seek(&_file, 0, LV_FS_SEEK_SET);
    return true;
}

void GNSS::paserStart()
{
    lv_timer_create(
        [](lv_timer_t* timer) {
            auto ctx = (GNSS*)lv_timer_get_user_data(timer);
            ctx->paserUpdate();
        },
        CONFIG_GNSS_UPDATE_PERIOD,
        this);
}

time_t GNSS::makeTime(const GPX_Parser::Time_t* time)
{
    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_year = time->year - 1900;
    t.tm_mon = time->month;
    t.tm_mday = time->day;
    t.tm_hour = time->hour;
    t.tm_min = time->minute;
    t.tm_sec = time->second;

    return mktime(&t);
}

double GNSS::getDiffTime(const GPX_Parser::Time_t* time1, const GPX_Parser::Time_t* time2)
{
    time_t t1 = makeTime(time1);
    time_t t2 = makeTime(time2);
    return difftime(t1, t2);
}

void GNSS::paserUpdate()
{
    GPX_Parser::Point_t point = { 0 };
    int parserFlag = _parser.getNext(&point);

    if (parserFlag & GPX_Parser::FLAG_LATITUDE && parserFlag & GPX_Parser::FLAG_LONGITUDE) {
        if (!_isReset) {
            _gnssInfo.longitude = point.longitude;
            _gnssInfo.latitude = point.latitude;
            _gnssInfo.altitude = point.altitude;

            _prePoint = point;
            _isReset = true;
            return;
        }

        double distance = Geo::distanceBetween(
            _gnssInfo.latitude, _gnssInfo.longitude,
            point.latitude, point.longitude);

        double diffTime = CONFIG_GNSS_UPDATE_PERIOD / 1000.0;

        if (parserFlag & GPX_Parser::FLAG_TIME) {
            diffTime = getDiffTime(&point.time, &_prePoint.time);
        }

        if (std::abs(diffTime) >= 0.0001) {
            _gnssInfo.speed = (float)(distance / diffTime) * 3.6f;
        }

        _gnssInfo.course = (float)Geo::courseTo(
            _gnssInfo.latitude,
            _gnssInfo.longitude,
            point.latitude,
            point.longitude);

        _gnssInfo.longitude = point.longitude;
        _gnssInfo.latitude = point.latitude;
        _gnssInfo.altitude = point.altitude;
        _prePoint = point;

        /* Gererate random GSV info */
        int index = rand() % CM_ARRAY_SIZE(_gsvItems);
        _gsvItems[index].id = rand() % 32;
        _gsvItems[index].elevation = rand() % 90;
        _gsvItems[index].azimuth = rand() % 360;
        _gsvItems[index].snr = rand() % 100;

        _isUpdate = true;
    } else if (parserFlag & GPX_Parser::FLAG_END_OF_FILE) {
        lv_fs_seek(&_file, 0, LV_FS_SEEK_SET);
        _isReset = false;
    }
}

#if CONFIG_RANDOM_GNSS

void GNSS::makeRandomGNSSInfo(HAL::GNSS_Info_t* gnssInfo)
{
    static int cnt = 0;
    static float speed = 0;

    cnt++;
    float diff = rand() % 200 / 10.0f - 10;
    speed += diff;
    CM_VALUE_LIMIT(speed, 0, 200);

    if (cnt > 100) {
        cnt = 0;
    }

    gnssInfo->latitude = CONFIG_GNSS_LATITUDE_DEFAULT + (rand() % 2000 - 1000) / 100000.0f;
    gnssInfo->longitude = CONFIG_GNSS_LONGITUDE_DEFAULT + (rand() % 2000 - 1000) / 100000.0f;
    gnssInfo->speed = speed;
    gnssInfo->course = rand() % 360;
    gnssInfo->satellites = cnt < 30 ? 0 : rand() % 20 + 5;
    gnssInfo->isVaild = cnt > 30;
}

#endif

} /* namespace HAL */

DEVICE_OBJECT_MAKE(GNSS);
