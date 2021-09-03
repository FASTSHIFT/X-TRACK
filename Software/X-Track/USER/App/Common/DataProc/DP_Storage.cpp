#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Utils/StorageService/StorageService.h"
#include "Utils/MapConv/MapConv.h"
#include "Config/Config.h"
#include <stdlib.h>

using namespace DataProc;

static StorageService storageService(CONFIG_SYSTEM_SAVE_FILE_PATH);

static void MapConvGetRange(const char* dirName, int16_t* min, int16_t* max)
{
    lv_fs_dir_t dir;
    lv_fs_res_t res = lv_fs_dir_open(&dir, dirName);

    if (res == LV_FS_RES_OK)
    {
        LV_LOG_USER("%s open success", dirName);

        int16_t levelMin = 19;
        int16_t levelMax = 0;

        char name[128];
        while (1)
        {
            lv_fs_dir_read(&dir, name);
            if (name[0] == '\0')
            {
                break;
            }

            if (name[0] == '/')
            {
                int level = atoi(name + 1);

                if (level < levelMin)
                {
                    levelMin = level;
                }

                if (level > levelMax)
                {
                    levelMax = level;
                }
            }
        }

        *min = levelMin;
        *max = levelMax;

        lv_fs_dir_close(&dir);
    }
    else
    {
        LV_LOG_USER("%s open faild", dirName);
    }
}

static void onLoad(Account* account)
{
    storageService.LoadFile();

    SysConfig_Info_t sysConfig;
    account->Pull("SysConfig", &sysConfig, sizeof(sysConfig));

    MapConv::SetDirPath(sysConfig.mapDirPath);
    MapConv::SetExtName(sysConfig.mapExtName);
    MapConv::SetCoordTransformEnable(!sysConfig.mapWGS84);

    int16_t levelMin = 19;
    int16_t levelMax = 0;
    MapConvGetRange(sysConfig.mapDirPath, &levelMin, &levelMax);

    MapConv::SetLevelRange(levelMin, levelMax);

    LV_LOG_USER(
        "Map path: %s, WGS84: %d, level min = %d, max = %d",
        sysConfig.mapDirPath,
        sysConfig.mapWGS84,
        MapConv::GetLevelMin(),
        MapConv::GetLevelMax()
    );
    LV_LOG_USER("Map ext name: *.%s", sysConfig.mapExtName);
#if CONFIG_MAP_PNG_DECODE_ENABLE
    LV_LOG_USER("Map PNG decoder enable");
#endif
}

static void onNotify(Account* account, Storage_Info_t* info)
{
    switch (info->cmd)
    {
    case STORAGE_CMD_LOAD:
        onLoad(account);
        break;
    case STORAGE_CMD_SAVE:
        storageService.SaveFile();
        break;
    case STORAGE_CMD_ADD:
        storageService.Add(
            info->key,
            info->value,
            info->size,
            (StorageService::DataType_t)info->type
        );
        break;
    case STORAGE_CMD_REMOVE:
        storageService.Remove(info->key);
        break;
    default:
        break;
    }
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event == Account::EVENT_SUB_PULL)
    {
        if (param->size != sizeof(Storage_Basic_Info_t))
        {
            return Account::ERROR_SIZE_MISMATCH;
        }

        Storage_Basic_Info_t* info = (Storage_Basic_Info_t*)param->data_p;
        info->isDetect = HAL::SD_GetReady();
        info->totalSizeMB = HAL::SD_GetCardSizeMB();
        info->freeSizeMB = 0.0f;
        return 0;
    }

    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(Storage_Info_t))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    Storage_Info_t* info = (Storage_Info_t*)param->data_p;
    onNotify(account, info);

    return 0;
}


static void onSDEvent(bool insert)
{
    if(insert)
    {
        DataProc::Storage_Info_t info;
        info.cmd = DataProc::STORAGE_CMD_LOAD;
        DataProc::Center()->AccountMain.Notify("Storage", &info, sizeof(info));
    }
}

DATA_PROC_INIT_DEF(Storage)
{
    account->SetEventCallback(onEvent);
    account->Subscribe("SysConfig");
    HAL::SD_SetEventCallback(onSDEvent);
}
