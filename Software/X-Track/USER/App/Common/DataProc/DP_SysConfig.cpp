#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Config/Config.h"

using namespace DataProc;

static SysConfig_Info_t sysConfig =
{
    .cmd = SYSCONFIG_CMD_LOAD,
    .soundEnable = true,
    .longitudeDefault = CONFIG_GPS_LNG_DEFAULT,
    .latitudeDefault = CONFIG_GPS_LAT_DEFAULT,
    .language = CONFIG_SYSTEM_LANGUAGE_DEFAULT,
    .mapDirPath = CONFIG_MAP_DIR_PATH,
    .WGS84 = CONFIG_MAP_USE_WGS84_DEFAULT,
    .arrowTheme = CONFIG_ARROW_THEME_DEFAULT,
};

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->size != sizeof(SysConfig_Info_t))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    SysConfig_Info_t* info = (SysConfig_Info_t*)param->data_p;

    switch (param->event)
    {
    case Account::EVENT_NOTIFY:
    {
        if (info->cmd == SYSCONFIG_CMD_LOAD)
        {
            HAL::Buzz_SetEnable(sysConfig.soundEnable);
        }
    }
    break;
    case Account::EVENT_SUB_PULL:
    {
        memcpy(info, &sysConfig, sizeof(sysConfig));
    }
    break;
    default:
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    return 0;
}

DATA_PROC_INIT_DEF(SysConfig)
{
    account->Subscribe("Storage");
    account->SetEventCallback(onEvent);

    STORAGE_VALUE_REG(account, sysConfig.soundEnable, STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(account, sysConfig.longitudeDefault, STORAGE_TYPE_DOUBLE);
    STORAGE_VALUE_REG(account, sysConfig.latitudeDefault, STORAGE_TYPE_DOUBLE);
    STORAGE_VALUE_REG(account, sysConfig.language, STORAGE_TYPE_STRING);
    STORAGE_VALUE_REG(account, sysConfig.mapDirPath, STORAGE_TYPE_STRING);
    STORAGE_VALUE_REG(account, sysConfig.WGS84, STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(account, sysConfig.arrowTheme, STORAGE_TYPE_STRING);
}
