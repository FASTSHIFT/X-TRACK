#include "DataProc.h"
#include "../HAL/HAL.h"

using namespace DataProc;

static struct
{
    bool soundDisable;
    char language[16];
} sysConfig;

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
        HAL::Buzz_SetEnable(!sysConfig.soundDisable);
    }

    return 0;
}

DATA_PROC_INIT_DEF(SysConfig)
{
    account->Subscribe("Storage");
    account->SetEventCallback(onEvent);

    STORAGE_VALUE_REG(account, sysConfig.soundDisable, STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(account, sysConfig.language, STORAGE_TYPE_STRING);
}
