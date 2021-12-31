#include "HAL.h"
#include <windows.h>

void HAL::Power_GetInfo(Power_Info_t* info)
{
    SYSTEM_POWER_STATUS sysPower = { 0 };
    GetSystemPowerStatus(&sysPower);

    int usage = sysPower.BatteryLifePercent;

    if (usage > 100)
    {
        usage = 100;
    }

    info->isCharging = sysPower.ACLineStatus;
    info->voltage = 3700;
    info->usage = usage;
    info->current = 200;
    info->average_power = 600;
    info->remaining_capacity = 980;
    info->fullcharge_capacity = 1000;
}

void HAL::Power_SetEventCallback(Power_CallbackFunction_t callback)
{

}
