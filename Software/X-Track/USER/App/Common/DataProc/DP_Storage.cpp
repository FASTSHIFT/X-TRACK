#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Utils/StorageService/StorageService.h"
#include "Utils/MapConv/MapConv.h"
#include "Config/Config.h"
#include <stdlib.h>

using namespace DataProc;

#define MAP_LEVEL_MIN    0
#define MAP_LEVEL_MAX    19

static StorageService storageService(CONFIG_SYSTEM_SAVE_FILE_PATH, 4096);

static bool MapConvGetRange(const char* dirName, int16_t* min, int16_t* max)
{
    bool retval = false;
    lv_fs_dir_t dir;

    if (lv_fs_dir_open(&dir, dirName) == LV_FS_RES_OK)
    {
        LV_LOG_USER("%s open success", dirName);

        int16_t levelMin = MAP_LEVEL_MAX;
        int16_t levelMax = MAP_LEVEL_MIN;

        char name[128];
        while (1)
        {
            lv_fs_res_t res = lv_fs_dir_read(&dir, name);

            if (name[0] == '\0' || res != LV_FS_RES_OK)
            {
                break;
            }

            if (name[0] == '/')
            {
                retval = true;
                int level = atoi(name + 1);

                if (level < MAP_LEVEL_MIN || level > MAP_LEVEL_MAX)
                {
                    LV_LOG_ERROR("Error level = %d", level);
                    retval = false;
                    break;
                }

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

        if (retval)
        {
            *min = levelMin;
            *max = levelMax;
        }

        lv_fs_dir_close(&dir);
    }
    else
    {
        LV_LOG_ERROR("%s open faild", dirName);
    }
    return retval;
}

static bool onLoad(Account* account)
{
    bool success = storageService.LoadFile();

    if (!success)
    {
        LV_LOG_WARN("Load " CONFIG_SYSTEM_SAVE_FILE_PATH " error");
    }

    SysConfig_Info_t sysConfig;
    if (account->Pull("SysConfig", &sysConfig, sizeof(sysConfig)) != Account::RES_OK)
    {
        LV_LOG_ERROR("Pull SysConfig failed!");
        return false;
    }

    MapConv::SetDirPath(sysConfig.mapDirPath);
    MapConv::SetExtName(sysConfig.mapExtName);
    MapConv::SetCoordTransformEnable(!sysConfig.mapWGS84);

    int16_t levelMin, levelMax;
    if (MapConvGetRange(sysConfig.mapDirPath, &levelMin, &levelMax))
    {
        MapConv::SetLevelRange(levelMin, levelMax);
    }
    else
    {
        LV_LOG_ERROR("Get map level range failed!");
    }

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

    return success;
}

static void onNotify(Account* account, Storage_Info_t* info)
{
    static bool isLoadSuccess = false;

    switch (info->cmd)
    {
    case STORAGE_CMD_LOAD:
        isLoadSuccess = onLoad(account);
        break;
    case STORAGE_CMD_SAVE:
        storageService.SaveFile();
        if (isLoadSuccess)
        {
            LV_LOG_USER("Saving backup file: " CONFIG_SYSTEM_SAVE_FILE_BACKUP_PATH);
            storageService.SaveFile(CONFIG_SYSTEM_SAVE_FILE_BACKUP_PATH);
        }
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
            return Account::RES_SIZE_MISMATCH;
        }

        Storage_Basic_Info_t* info = (Storage_Basic_Info_t*)param->data_p;
        info->isDetect = HAL::SD_GetReady();
        info->totalSizeMB = HAL::SD_GetCardSizeMB();
        info->freeSizeMB = 0.0f;
        info->type = HAL::SD_GetTypeName();
        return Account::RES_OK;
    }

    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(Storage_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    Storage_Info_t* info = (Storage_Info_t*)param->data_p;
    onNotify(account, info);

    return Account::RES_OK;
}


static void onSDEvent(bool insert)
{
    if(insert)
    {
        DataProc::Storage_Info_t info;
        DATA_PROC_INIT_STRUCT(info);
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
