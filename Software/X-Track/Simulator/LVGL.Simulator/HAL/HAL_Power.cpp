#include "HAL.h"

#ifdef WIN32
#  include <windows.h>
#endif

void HAL::Power_GetInfo(Power_Info_t* info)
{
    int usage = 100;
    bool isCharging = false;
    
#ifdef WIN32
    SYSTEM_POWER_STATUS sysPower = { 0 };
    GetSystemPowerStatus(&sysPower);
    usage = sysPower.BatteryLifePercent;
    isCharging = sysPower.ACLineStatus;
#endif

    if (usage > 100)
    {
        usage = 100;
    }

    info->isCharging = isCharging;
    info->voltage = 3700;
    info->usage = usage;
}

void HAL::Power_SetEventCallback(Power_CallbackFunction_t callback)
{

}
