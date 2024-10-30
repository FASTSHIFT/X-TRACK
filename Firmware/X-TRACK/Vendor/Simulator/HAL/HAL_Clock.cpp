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
#include "HAL.h"
#include <stdio.h>
#include <time.h>

namespace HAL {

class Clock : private DeviceObject {
public:
    Clock(const char* name)
        : DeviceObject(name)
    {
    }

private:
    virtual int onInit();
    virtual int onRead(void* buffer, size_t size);
    virtual int onIoctl(DeviceObject::IO_Cmd_t cmd, void* data);
    void getInfo(HAL::Clock_Info_t* info);
    void calibrate(const HAL::Clock_Info_t* info);
};

int Clock::onInit()
{
    return DeviceObject::RES_OK;
}

int Clock::onRead(void* buffer, size_t size)
{
    if (size != sizeof(HAL::Clock_Info_t)) {
        return -1;
    }
    getInfo((HAL::Clock_Info_t*)buffer);
    return sizeof(HAL::Clock_Info_t);
}

int Clock::onIoctl(DeviceObject::IO_Cmd_t cmd, void* data)
{
    if (cmd.full != CLOCK_IOCMD_CALIBRATE) {
        return -1;
    }

    calibrate((HAL::Clock_Info_t*)data);
    return DeviceObject::RES_OK;
}

void Clock::getInfo(HAL::Clock_Info_t* info)
{
    struct tm* t;
    time_t tt;
    time(&tt);
    t = localtime(&tt);

    info->year = t->tm_year + 1900;
    info->month = t->tm_mon + 1;
    info->day = t->tm_mday;
    info->week = t->tm_wday;
    info->hour = t->tm_hour;
    info->minute = t->tm_min;
    info->second = t->tm_sec;
    info->millisecond = 0;
}

void Clock::calibrate(const HAL::Clock_Info_t* info)
{
    HAL_LOG_INFO(
        "Clock set: %04d-%02d-%02d %02d:%02d:%02d",
        info->year,
        info->month,
        info->day,
        info->hour,
        info->minute,
        info->second);
}

} /* namespace HAL */

DEVICE_OBJECT_MAKE(Clock);
