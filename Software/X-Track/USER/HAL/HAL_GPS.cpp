#include "HAL.h"
#include "TinyGPSPlus/src/TinyGPS++.h"

#define DEBUG_SERIAL           Serial
#define GPS_SERIAL             Serial2
#define GPS_USE_TRANSPARENT    0

static TinyGPSPlus gps;

void HAL::GPS_Init()
{
    GPS_SERIAL.begin(9600);

    DEBUG_SERIAL.print("GPS: TinyGPS++ library v. ");
    DEBUG_SERIAL.print(TinyGPSPlus::libraryVersion());
    DEBUG_SERIAL.println(" by Mikal Hart");
}

void HAL::GPS_Update()
{
#if GPS_USE_TRANSPARENT
    while (GPS_SERIAL.available() > 0)
    {
        DEBUG_SERIAL.write(GPS_SERIAL.read());
    }

    while (DEBUG_SERIAL.available() > 0)
    {
        GPS_SERIAL.write(DEBUG_SERIAL.read());
    }
#else
    while (GPS_SERIAL.available() > 0)
    {
        if (gps.encode(GPS_SERIAL.read()))
        {
        }
    }
#endif
}

bool HAL::GPS_GetInfo(GPS_Info_t* info)
{
    bool retval = false;
    memset(info, 0, sizeof(GPS_Info_t));

    if(gps.location.isValid())
    {
        info->longitude = gps.location.lng();
        info->latitude = gps.location.lat();
        info->altitude = gps.altitude.meters();
        info->speed = gps.speed.kmph();
        info->compass = gps.course.deg();
        retval = true;
    }
    else
    {
        info->longitude = 116.391332;
        info->latitude = 39.907415;
        info->altitude = 53.0f;
    }

    info->clock.year = gps.date.year();
    info->clock.month = gps.date.month();
    info->clock.day = gps.date.day();
    info->clock.hour = (gps.time.hour() + 8) % 24;
    info->clock.min = gps.time.minute();
    info->clock.sec = gps.time.second();
    info->satellites = gps.satellites.value();

    return retval;
}

bool HAL::GPS_LocationIsValid()
{
    return gps.location.isValid();
}

double HAL::GPS_GetDistanceOffset(GPS_Info_t* info,  double preLong, double preLat)
{
    return gps.distanceBetween(info->latitude, info->longitude, preLat, preLong);
}
