#include "HAL.h"
#include <windows.h>

void HAL::Power_GetInfo(Power_Info_t* info)
{
    SYSTEM_POWER_STATUS sysPower = { 0 };
    GetSystemPowerStatus(&sysPower);

    info->isCharging = sysPower.ACLineStatus;
    info->voltage = 3700;
    info->usage = sysPower.BatteryLifePercent;
}
