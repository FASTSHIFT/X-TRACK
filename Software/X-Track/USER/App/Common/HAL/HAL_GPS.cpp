#include "HAL.h"

bool HAL::GPS_GetInfo(GPS_Info_t* info)
{
    info->longitude = 116.391332;
    info->latitude = 39.907415;
    info->altitude = 53.0f;
    info->speed = 0.0f;
    info->satellites = 0;

    Clock_GetValue(&info->clock);

    return true;
}

void HAL::GPS_Update()
{

}
