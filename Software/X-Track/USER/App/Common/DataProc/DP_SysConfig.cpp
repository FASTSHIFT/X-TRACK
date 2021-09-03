#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Config/Config.h"

using namespace DataProc;

static SysConfig_Info_t sysConfig =
{
    .cmd         = SYSCONFIG_CMD_LOAD,
    .longitude   = CONFIG_GPS_LONGITUDE_DEFAULT,
    .latitude    = CONFIG_GPS_LATITUDE_DEFAULT,
    .timeZone    = CONFIG_SYSTEM_TIME_ZONE_DEFAULT,
    .soundEnable = CONFIG_SYSTEM_SOUND_ENABLE_DEFAULT,
    .language    = CONFIG_SYSTEM_LANGUAGE_DEFAULT,
    .arrowTheme  = CONFIG_ARROW_THEME_DEFAULT,
    .mapDirPath  = CONFIG_MAP_DIR_PATH_DEFAULT,
    .mapExtName  = CONFIG_MAP_EXT_NAME_DEFAULT,
    .mapWGS84    = CONFIG_MAP_USE_WGS84_DEFAULT,
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
        else if (info->cmd == SYSCONFIG_CMD_SAVE)
        {
            HAL::GPS_Info_t gpsInfo;
            account->Pull("GPS", &gpsInfo, sizeof(gpsInfo));
            
            if(gpsInfo.isVaild)
            {
                sysConfig.longitude = (float)gpsInfo.longitude;
                sysConfig.latitude = (float)gpsInfo.latitude;
            }
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
    account->Subscribe("GPS");
    account->SetEventCallback(onEvent);

    STORAGE_VALUE_REG(account, sysConfig.longitude, STORAGE_TYPE_FLOAT);
    STORAGE_VALUE_REG(account, sysConfig.latitude, STORAGE_TYPE_FLOAT);

    STORAGE_VALUE_REG(account, sysConfig.soundEnable, STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(account, sysConfig.timeZone, STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(account, sysConfig.language, STORAGE_TYPE_STRING);
    STORAGE_VALUE_REG(account, sysConfig.arrowTheme, STORAGE_TYPE_STRING);
    STORAGE_VALUE_REG(account, sysConfig.mapDirPath, STORAGE_TYPE_STRING);
    STORAGE_VALUE_REG(account, sysConfig.mapExtName, STORAGE_TYPE_STRING);
    STORAGE_VALUE_REG(account, sysConfig.mapWGS84, STORAGE_TYPE_INT);   
}
