/*
 * MIT License
 * Copyright (c) 2023 - 2024 HanfG, _VIFEXTech
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
#include <cstring>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SD_CARD_GB(byte) ((uint64_t)(byte)*1024 * 1024 * 1024)

namespace HAL {

class SdCard : private DeviceObject {
public:
    SdCard(const char* name)
        : DeviceObject(name)
    {
    }

private:
    HAL::SdCard_Info_t _info;

private:
    virtual int onInit();
    virtual int onRead(void* buffer, size_t size);
    virtual int onIoctl(DeviceObject::IO_Cmd_t cmd, void* data);
    const char* genRealPath(const char* path);
};

int SdCard::onInit()
{
    _info.isInsert = true;
    _info.freeCapacity = SD_CARD_GB(16);
    _info.totalCapacity = SD_CARD_GB(32);
    return DeviceObject::RES_OK;
}

int SdCard::onRead(void* buffer, size_t size)
{
    if (size != sizeof(_info)) {
        return DeviceObject::RES_PARAM_ERROR;
    }

    memcpy(buffer, &_info, sizeof(_info));

    return sizeof(_info);
}

int SdCard::onIoctl(DeviceObject::IO_Cmd_t cmd, void* data)
{
    switch (cmd.full) {
    case SDCARD_IOCMD_MOUNT:
        return DeviceObject::RES_OK;

    case SDCARD_IOCMD_UNMOUNT:
        return DeviceObject::RES_OK;

    case SDCARD_IOCMD_MKDIR: {
        const char* realPath = genRealPath((const char*)data);
        int ret = access(realPath, F_OK);
        if (ret == 0) {
            HAL_LOG_INFO("%s already exits", realPath);
            return DeviceObject::RES_OK;
        }
        return mkdir(realPath, 0755);
    }

    case SDCARD_IOCMD_REMOVE:
        return remove(genRealPath((const char*)data));

    default:
        break;
    }

    return DeviceObject::RES_UNSUPPORT;
}

const char* SdCard::genRealPath(const char* path)
{
    if (*path != '\0') {
        path++;
        if (*path == ':' || *path == '/') {
            path++;
        }
    }
    return path;
}

} /* namespace HAL */

DEVICE_OBJECT_MAKE(SdCard);
