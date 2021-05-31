#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Utils/StorageService/StorageService.h"

#define SAVE_FILE_NAME "/SystemSave.json"

using namespace DataProc;

static StorageService storageService(SAVE_FILE_NAME);

static int onEvent(Account::EventParam_t* param)
{
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

void DP_Storage_Init(Account* act)
{
    act->SetEventCallback(onEvent);
}
