#include "DataProc.h"
#include "Utils/Filters/Filters.h"
#include "../HAL/HAL.h"

static Filter::Hysteresis<int> usageFilter(2);

static int onEvent(Account::EventParam_t* param)
{
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
    
    powerInfo.usage = usageFilter.GetNext(powerInfo.usage);

    memcpy(param->data_p, &powerInfo, param->size);

    return 0;
}

static void Power_Update(Account* account)
{
    static bool lastStatus = false;
    
    HAL::Power_Info_t info;
    HAL::Power_GetInfo(&info);
    if(info.isCharging != lastStatus)
    {
        HAL::Audio_PlayMusic(info.isCharging ? "BattChargeStart" : "BattChargeEnd");
        lastStatus = info.isCharging;
    }
}

void DP_Power_Register(DataCenter* center)
{
    Account* account = new Account("Power", center, sizeof(HAL::Power_Info_t));
    account->SetEventCallback(onEvent);
    account->SetTimerCallback(Power_Update, 500);
}
