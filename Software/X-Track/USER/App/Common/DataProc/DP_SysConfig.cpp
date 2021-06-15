#include "DataProc.h"
#include "../HAL/HAL.h"

using namespace DataProc;

static struct{
    bool SoundDisable;
    char Language[16];
}SysConfig;

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_NOTIFY)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(SysConfig_Info_t))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    SysConfig_Info_t* info = (SysConfig_Info_t*)param->data_p;

    if (info->cmd == SYSCONFIG_CMD_LOAD)
    {
        HAL::Buzz_SetEnable(!SysConfig.SoundDisable);
    }

    return 0;
}

DATA_PROC_INIT_DEF(SysConfig)
{
    account->Subscribe("Storage");
    account->SetEventCallback(onEvent);

    STORAGE_VALUE_REG(account, SysConfig.SoundDisable, STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(account, SysConfig.Language, STORAGE_TYPE_STRING);
}
