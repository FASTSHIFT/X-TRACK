#include "HAL.h"
#include <string.h>
#include <stdlib.h>

bool HAL::GPS_GetInfo(GPS_Info_t* info)
{
    static int angle;
    memset(info, 0, sizeof(GPS_Info_t));

    info->satellites = 10;
    info->longitude = 116.391332;
    info->latitude = 39.907415;
    info->altitude = 53.0f;

    Clock_GetInfo(&info->clock);

    info->speed = rand() % 60;

    return true;
}

void HAL::GPS_Update()
{

}
