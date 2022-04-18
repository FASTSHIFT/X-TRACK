#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Utils/Time/Time.h"
#include "Config/Config.h"

using namespace DataProc;

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_SUB_PULL)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(HAL::Clock_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    HAL::GPS_Info_t gps;
    if (account->Pull("GPS", &gps, sizeof(gps)) != Account::RES_OK)
    {
        return Account::RES_UNKNOW;
    }

    DataProc::SysConfig_Info_t sysCfg;
    if (account->Pull("SysConfig", &sysCfg, sizeof(sysCfg)) != Account::RES_OK)
    {
        return Account::RES_UNKNOW;
    }

    setTime(
        gps.clock.hour,
        gps.clock.minute,
        gps.clock.second,
        gps.clock.day,
        gps.clock.month,
        gps.clock.year
    );
    adjustTime(sysCfg.timeZone * SECS_PER_HOUR);

    HAL::Clock_Info_t* info = (HAL::Clock_Info_t*)param->data_p;
    info->year = year();
    info->month = month();
    info->day = day();
    info->hour = hour();
    info->minute = minute();
    info->second = second();

    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(TzConv)
{
    account->Subscribe("GPS");
    account->Subscribe("SysConfig");
    account->SetEventCallback(onEvent);
}
