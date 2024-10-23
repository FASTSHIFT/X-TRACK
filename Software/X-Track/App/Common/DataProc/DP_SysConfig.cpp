#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Config/Config.h"

using namespace DataProc;

static SysConfig_Info_t sysConfig;

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->size != sizeof(SysConfig_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
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
            if(account->Pull("GPS", &gpsInfo, sizeof(gpsInfo)) != Account::RES_OK)
            {
                return Account::RES_UNKNOW;
            }

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
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(SysConfig)
{
    account->Subscribe("Storage");
    account->Subscribe("GPS");
    account->SetEventCallback(onEvent);

    memset(&sysConfig, 0, sizeof(sysConfig));

#   define SYSCGF_STRCPY(dest, src) \
do{ \
    strncpy(dest, src, sizeof(dest)); \
    dest[sizeof(dest) - 1] = '\0'; \
}while(0)

    sysConfig.cmd         = SYSCONFIG_CMD_LOAD;
    sysConfig.longitude   = CONFIG_GPS_LONGITUDE_DEFAULT;
    sysConfig.latitude    = CONFIG_GPS_LATITUDE_DEFAULT;
    sysConfig.timeZone    = CONFIG_SYSTEM_TIME_ZONE_DEFAULT;
    sysConfig.soundEnable = CONFIG_SYSTEM_SOUND_ENABLE_DEFAULT;
    SYSCGF_STRCPY(sysConfig.language, CONFIG_SYSTEM_LANGUAGE_DEFAULT);
    SYSCGF_STRCPY(sysConfig.arrowTheme, CONFIG_ARROW_THEME_DEFAULT);
    SYSCGF_STRCPY(sysConfig.mapDirPath, CONFIG_MAP_DIR_PATH_DEFAULT);
    SYSCGF_STRCPY(sysConfig.mapExtName, CONFIG_MAP_EXT_NAME_DEFAULT);
    sysConfig.mapWGS84    = CONFIG_MAP_USE_WGS84_DEFAULT;

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
