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

#define BACKLIGHT_RESOLUTION 1000

namespace HAL {

class Backlight : private DeviceObject {
public:
    Backlight(const char* name)
        : DeviceObject(name)
        , _backlightValue(0)
    {
    }

private:
    int _backlightValue;

private:
    virtual int onInit();
    virtual int onRead(void* buffer, size_t size);
    virtual int onWrite(const void* buffer, size_t size);
    virtual int onIoctl(DeviceObject::IO_Cmd_t cmd, void* data);
    int getValue();
    void setValue(int value);
    void forceLit();
};

int Backlight::onInit()
{
    setValue(0);
    return DeviceObject::RES_OK;
}

int Backlight::onRead(void* buffer, size_t size)
{
    if (size != sizeof(int)) {
        return DeviceObject::RES_PARAM_ERROR;
    }
    *(int*)buffer = getValue();
    return sizeof(int);
}

int Backlight::onWrite(const void* buffer, size_t size)
{
    if (size != sizeof(int)) {
        return DeviceObject::RES_PARAM_ERROR;
    }
    setValue(*(int*)buffer);
    return sizeof(int);
}

int Backlight::onIoctl(DeviceObject::IO_Cmd_t cmd, void* data)
{
    int retval = DeviceObject::RES_UNKNOWN;
    switch (cmd.full) {
    case BACKLIGHT_IOCMD_FORCE_LIT:
        forceLit();
        retval = DeviceObject::RES_OK;
        break;
    default:
        break;
    }
    return retval;
}

int Backlight::getValue()
{
    return _backlightValue;
}

void Backlight::setValue(int value)
{
    CM_VALUE_LIMIT(value, 0, BACKLIGHT_RESOLUTION);
    _backlightValue = value;
}

void Backlight::forceLit()
{
}

} /* namespace HAL */

DEVICE_OBJECT_MAKE(Backlight);
