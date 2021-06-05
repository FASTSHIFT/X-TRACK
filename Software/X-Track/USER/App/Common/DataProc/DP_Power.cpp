#include "DataProc.h"
#include "Utils/Filters/Filters.h"
#include "../HAL/HAL.h"

static void onTimer(Account* account)
{
    static bool lastStatus = false;

    HAL::Power_Info_t info;
    HAL::Power_GetInfo(&info);
    if (info.isCharging != lastStatus)
    {
        HAL::Audio_PlayMusic(info.isCharging ? "BattChargeStart" : "BattChargeEnd");
        lastStatus = info.isCharging;
    }
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    static Filter::Hysteresis<int16_t>      battUsageHysFilter(2);
    static Filter::MedianQueue<int16_t, 10> battUsageMqFilter;
    
    if (param->event == Account::EVENT_TIMER)
    {
        onTimer(account);
        return 0;
    }

    if (param->event != Account::EVENT_SUB_PULL)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(HAL::Power_Info_t))
    {
        return -1;
    }

    HAL::Power_Info_t powerInfo;
    HAL::Power_GetInfo(&powerInfo);
    
    int16_t usage = powerInfo.usage;
    usage = battUsageHysFilter.GetNext(usage);
    usage = battUsageMqFilter.GetNext(usage);
    powerInfo.usage = usage;

    memcpy(param->data_p, &powerInfo, param->size);

    return 0;
}

DATA_PROC_INIT_DEF(Power)
{
    act->SetEventCallback(onEvent);
    act->SetTimerPeriod(500);
}
