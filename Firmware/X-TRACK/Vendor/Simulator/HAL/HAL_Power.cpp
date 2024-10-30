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
#include <stdlib.h>

namespace HAL {

class Power : private DeviceObject {
public:
    Power(const char* name)
        : DeviceObject(name)
    {
    }

private:
    virtual int onInit();
    virtual int onRead(void* buffer, size_t size);
    virtual int onIoctl(DeviceObject::IO_Cmd_t cmd, void* data);
};

int Power::onInit()
{
    return DeviceObject::RES_OK;
}

int Power::onRead(void* buffer, size_t size)
{
    if (size != sizeof(HAL::Power_Info_t)) {
        return DeviceObject::RES_PARAM_ERROR;
    }

    HAL::Power_Info_t* info = (HAL::Power_Info_t*)buffer;
    info->voltage = rand() % 1000 + 3200;
    info->level = rand() % 100;
    info->isReady = (rand() % 2) == 0;
    info->isCharging = (rand() % 2) == 0;

    return sizeof(HAL::Power_Info_t);
}

int Power::onIoctl(DeviceObject::IO_Cmd_t cmd, void* data)
{
    int retval = 0;

    switch (cmd.full) {
    case POWER_IOCMD_POWER_ON:
        HAL_LOG_INFO("Power: ON");
        break;
    case POWER_IOCMD_POWER_OFF:
        HAL_LOG_INFO("Power: OFF");
        exit(0);
        break;
    case POWER_IOCMD_REBOOT:
        HAL_LOG_INFO("Power: Reboot");
        break;
    case POWER_IOCMD_GAUGE_RESET:
        HAL_LOG_INFO("Power: Reset Fuel Gauge");
        break;
    case POWER_IOCMD_SET_SYS_VOLTAGE:
        break;
    default:
        retval = -1;
    }
    return retval;
}

} /* namespace HAL */

DEVICE_OBJECT_MAKE(Power);
