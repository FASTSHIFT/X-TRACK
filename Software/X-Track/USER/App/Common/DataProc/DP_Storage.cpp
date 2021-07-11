#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Utils/StorageService/StorageService.h"
#include "Config/Config.h"

using namespace DataProc;

static StorageService storageService(CONFIG_SYSTEM_SAVE_FILE_PATH);

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
        info->totalSize = 0;
        info->freeSize = 0;
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

    switch (info->cmd)
    {
    case STORAGE_CMD_LOAD:
        storageService.LoadFile();
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

    return 0;
}

static void onSDEvent(bool insert)
{
    if(insert)
    {
        storageService.LoadFile();
    }
}

DATA_PROC_INIT_DEF(Storage)
{
    account->SetEventCallback(onEvent);
    HAL::SD_SetEventCallback(onSDEvent);
}
