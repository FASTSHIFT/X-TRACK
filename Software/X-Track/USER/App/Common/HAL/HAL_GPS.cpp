#include "HAL.h"
#include <string.h>

bool HAL::GPS_GetInfo(GPS_Info_t* info)
{
    static int angle;
    memset(info, 0, sizeof(GPS_Info_t));

    info->longitude = 116.391332;
    info->latitude = 39.907415;
    info->altitude = 53.0f;
    //info->compass = angle;

    Clock_GetValue(&info->clock);

    angle += 45;
    angle %= 360;

    return true;
}

void HAL::GPS_Update()
{

}
