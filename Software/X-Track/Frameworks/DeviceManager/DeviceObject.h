/*
 * MIT License
 * Copyright (c) 2022 _VIFEXTech
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

#ifndef __DEVICE_OBJECT_H
#define __DEVICE_OBJECT_H

#include <stddef.h>
#include <stdint.h>

#define DEVICE_OBJECT_IOCMD_DEF(dir, size, type, number) \
    (uint32_t)(                                          \
        (((uint32_t)(dir) & 0x0003) << 30)               \
        | (((uint32_t)(size) & 0x3FFF) << 16)            \
        | (((uint32_t)(type) & 0x00FF) << 8)             \
        | ((uint32_t)(number) & 0x00FF))

class DeviceObject {
public:
    typedef enum {
        DIR_NONE = 0x00,
        DIR_IN = 0x01,
        DIR_OUT = 0x02,
        DIR_IN_OUT = 0x03,
    } Direction_t;

    typedef union {
        uint32_t full;
        struct
        {
            uint32_t number : 8;
            uint32_t type : 8;
            uint32_t size : 14;
            uint32_t dir : 2;
        } ch;
    } IO_Cmd_t;

    typedef enum {
        RES_OK = 0,
        RES_UNKNOWN = -1,
        RES_NO_IMPLEMENTED = -2,
        RES_INIT_FAILED = -3,
        RES_PARAM_ERROR = -4,
        RES_UNSUPPORT = -5,
    } ResCode_t;

public:
    DeviceObject(const char* name)
        : _name(name)
        , initOK(false)
    {
    }
    virtual ~DeviceObject() {};

    int init()
    {
        int retval = onInit();
        initOK = retval >= RES_OK;

        return retval;
    }

    int read(void* buffer, size_t size)
    {
        if (!initOK) {
            return RES_INIT_FAILED;
        }

        if (!buffer) {
            return RES_PARAM_ERROR;
        }

        return onRead(buffer, size);
    }

    int write(const void* buffer, size_t size)
    {
        if (!initOK) {
            return RES_INIT_FAILED;
        }

        if (!buffer) {
            return RES_PARAM_ERROR;
        }

        return onWrite(buffer, size);
    }

    int ioctl(uint32_t cmd, void* data = nullptr, size_t size = 0)
    {
        IO_Cmd_t* c = (IO_Cmd_t*)&cmd;

        if (!initOK) {
            return RES_INIT_FAILED;
        }

        if (size != c->ch.size) {
            return RES_PARAM_ERROR;
        }

        return onIoctl(*c, data);
    }

    const char* getName()
    {
        return _name;
    }

private:
    const char* _name;
    bool initOK;

private:
    virtual int onInit() { return RES_NO_IMPLEMENTED; };
    virtual int onRead(void* buffer, size_t size) { return RES_NO_IMPLEMENTED; }
    virtual int onWrite(const void* buffer, size_t size) { return RES_NO_IMPLEMENTED; }
    virtual int onIoctl(IO_Cmd_t cmd, void* data) { return RES_NO_IMPLEMENTED; }
};

#endif
