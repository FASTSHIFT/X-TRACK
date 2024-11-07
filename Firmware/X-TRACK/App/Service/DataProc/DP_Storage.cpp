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
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHERDP_Storage
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "Config/Config.h"
#include "DataProc.h"
#include "Service/HAL/HAL.h"
#include "Utils/StorageService/StorageService.h"
#include "lvgl/lvgl.h"

/* System configuration file */
#define SYSTEM_SAVE_PATH RESOURCE_MAKE_PATH("/SystemSave.json")
#define SYSTEM_SAVE_BACKUP_DIR RESOURCE_MAKE_PATH("/Backup")
#define SYSTEM_SAVE_BACKUP_FMT SYSTEM_SAVE_BACKUP_DIR "/SystemSave_%d%02d%02d_%02d%02d%02d.json"

#define IS_STR_EQ(STR1, STR2) (strcmp(STR1, STR2) == 0)

using namespace DataProc;

class DP_Storage {
public:
    DP_Storage(DataNode* node);

public:
    DataNode* _node;
    const DataNode* _nodeGlobal;
    Env_Helper _env;
    StorageService _service;
    DeviceObject* _dev;

public:
    int onEvent(DataNode::EventParam_t* param);
    int onNotify(Storage_Info_t* info);
    int onSdCardMount(bool isMount);
    const char* getTimeString(const char* format, char* buf, size_t size);
};

DP_Storage::DP_Storage(DataNode* node)
    : _node(node)
    , _env(node)
    , _service(SYSTEM_SAVE_PATH)
{
    _dev = HAL::Manager()->getDevice("SdCard");
    if (!_dev) {
        return;
    }

    /* create backup dir */
    const char* backupPath = SYSTEM_SAVE_BACKUP_DIR;
    _dev->ioctl(SDCARD_IOCMD_MKDIR, (void*)backupPath, sizeof(backupPath));

    node->subscribe("Clock");
    _nodeGlobal = node->subscribe("Global");

    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_Storage*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PULL | DataNode::EVENT_NOTIFY | DataNode::EVENT_PUBLISH);
}

int DP_Storage::onEvent(DataNode::EventParam_t* param)
{
    switch (param->event) {
    case DataNode::EVENT_PULL: {
        if (param->size != sizeof(Storage_Device_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }

        Storage_Device_Info_t* info = (Storage_Device_Info_t*)param->data_p;
        HAL::SdCard_Info_t sdInfo;
        if (_dev->read(&sdInfo, sizeof(sdInfo)) != sizeof(sdInfo)) {
            return DataNode::RES_NO_DATA;
        }

        info->isDetect = sdInfo.isInsert;
        info->isActive = sdInfo.isActive;
        info->totalSize = sdInfo.totalCapacity;
        info->freeSize = sdInfo.freeCapacity;
    } break;

    case DataNode::EVENT_NOTIFY: {
        if (param->size != sizeof(Storage_Info_t)) {
            return DataNode::RES_SIZE_MISMATCH;
        }

        Storage_Info_t* info = (Storage_Info_t*)param->data_p;
        return onNotify(info);
    } break;

    case DataNode::EVENT_PUBLISH: {
        if (param->tran == _env) {
            auto envInfo = (DataProc::Env_Info_t*)param->data_p;
            if (IS_STR_EQ(envInfo->key, "storage")) {
                Storage_Info_t info;
                if (IS_STR_EQ(envInfo->value, "save")) {
                    info.cmd = STORAGE_CMD::SAVE;
                } else if (IS_STR_EQ(envInfo->value, "load")) {
                    info.cmd = STORAGE_CMD::LOAD;
                } else {
                    return DataNode::RES_PARAM_ERROR;
                }

                return onNotify(&info);
            }
        }

        if (param->tran == _nodeGlobal) {
            auto globalInfo = (const DataProc::Global_Info_t*)param->data_p;

            /* Load data after initialization */
            if (globalInfo->event == DataProc::GLOBAL_EVENT::DATA_PROC_INIT_FINISHED) {
                Storage_Info_t info;
                info.cmd = STORAGE_CMD::LOAD;

                /* Unsubscribe global data node */
                _node->unsubscribe("Global");
                _nodeGlobal = nullptr;
                return onNotify(&info);
            }
        }
    } break;

    default:
        break;
    }

    return DataNode::RES_OK;
}

int DP_Storage::onNotify(Storage_Info_t* info)
{
    switch (info->cmd) {
    case STORAGE_CMD::LOAD: {
        if (_service.load()) {
            _node->publish(info, sizeof(Storage_Info_t));
        } else {
            LV_LOG_WARN("Load " SYSTEM_SAVE_PATH " error");
        }
    } break;

    case STORAGE_CMD::SAVE: {
        bool ret = _service.save();
        LV_LOG_USER("Saving file '" SYSTEM_SAVE_PATH "', ret: %d", ret);

        char buf[128];
        const char* timeStr = getTimeString(SYSTEM_SAVE_BACKUP_FMT, buf, sizeof(buf));
        ret = _service.save(timeStr);
        LV_LOG_USER("Saving backup file '%s', ret: %d", timeStr, ret);
    } break;

    case STORAGE_CMD::ADD:
        _service.add(
            info->key,
            info->value,
            info->size,
            (StorageService::TYPE)info->type);
        break;
    case STORAGE_CMD::MOUNT:
    case STORAGE_CMD::UNMOUNT:
        return onSdCardMount(info->cmd == STORAGE_CMD::MOUNT);

    default:
        break;
    }

    return DataNode::RES_OK;
}

int DP_Storage::onSdCardMount(bool isMount)
{
    int ret = isMount ? _dev->ioctl(SDCARD_IOCMD_MOUNT) : _dev->ioctl(SDCARD_IOCMD_UNMOUNT);
    LV_LOG_WARN("isMount: %d, ret = %d", isMount, ret);

    return (ret == DeviceObject::RES_OK) ? DataNode::RES_OK : DataNode::RES_UNKNOWN;
}

const char* DP_Storage::getTimeString(const char* format, char* buf, size_t size)
{
    HAL::Clock_Info_t clock;
    if (_node->pull("Clock", &clock, sizeof(clock)) != DataNode::RES_OK) {
        return nullptr;
    }

    int retval = lv_snprintf(
        buf,
        size,
        format,
        clock.year,
        clock.month,
        clock.day,
        clock.hour,
        clock.minute,
        clock.second);

    if (retval < 0 || retval >= (int)size) {
        return nullptr;
    }

    return buf;
}

DATA_PROC_DESCRIPTOR_DEF(Storage)
