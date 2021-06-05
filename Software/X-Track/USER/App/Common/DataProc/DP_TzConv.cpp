#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Utils/Time/Time.h"

// Offset hours from gps time (UTC)
static int GMT_OffsetHours = 8; //GMT+8 Beijing

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_SUB_PULL)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(HAL::Clock_Info_t))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    HAL::GPS_Info_t gps;
    account->Pull("GPS", &gps, sizeof(gps));

    setTime(
        gps.clock.hour,
        gps.clock.minute,
        gps.clock.second,
        gps.clock.day,
        gps.clock.month,
        gps.clock.year
    );
    adjustTime(GMT_OffsetHours * SECS_PER_HOUR);

    HAL::Clock_Info_t* info = (HAL::Clock_Info_t*)param->data_p;
    info->year = year();
    info->month = month();
    info->day = day();
    info->hour = hour();
    info->minute = minute();
    info->second = second();
   
    return 0;
}

DATA_PROC_INIT_DEF(TzConv)
{
    act->Subscribe("GPS");
    act->SetEventCallback(onEvent);
}
