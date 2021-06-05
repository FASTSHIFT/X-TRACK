#include "HAL.h"
#include <string.h>
#include <stdlib.h>

bool HAL::GPS_GetInfo(GPS_Info_t* info)
{
    static int angle;
    memset(info, 0, sizeof(GPS_Info_t));

    info->isVaild = true;
    info->satellites = 10;

    info->longitude = 116.391332;
    info->latitude = 39.907415;
    info->altitude = 53.0f;

    /*info->longitude = 117.789509;
    info->latitude = 26.405395;
    info->altitude = 150;*/

    Clock_GetInfo(&info->clock);

    info->speed = (rand() % 600) / 10.0f;

    return true;
}

void HAL::GPS_Update()
{

}

double HAL::GPS_GetDistanceOffset(GPS_Info_t* info, double preLong, double preLat)
{
    return 10;
}
