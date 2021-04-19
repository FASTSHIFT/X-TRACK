#include "DataProc.h"
#include "Utils/Filters/Filters.h"
#include "../HAL/HAL.h"

static int Power_Callback(
    Account* pub,
    Account* sub,
    int msgType,
    void* data_p,
    uint32_t size
)
{
    if (size != sizeof(HAL::Power_Info_t))
    {
        return -1;
    }

    HAL::Power_GetInfo((HAL::Power_Info_t*)data_p);

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

Account* DataProc::Power_Init(DataCenter* center)
{
    Account* account = new Account("Power", center);
    account->SetPullCallback(Power_Callback);
    account->SetTimerCallback(Power_Update, 500);
    return account;
}
