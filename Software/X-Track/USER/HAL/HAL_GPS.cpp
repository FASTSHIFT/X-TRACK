#include "HAL.h"
#include "TinyGPSPlus/src/TinyGPS++.h"

static TinyGPSPlus gps;

#define DEBUG_SERIAL           Serial
#define GPS_SERIAL             Serial2
#define GPS_USE_TRANSPARENT    0

void HAL::GPS_Init()
{
    GPS_SERIAL.begin(9600);
    DEBUG_SERIAL.print("TinyGPS++ library v. ");
    DEBUG_SERIAL.println(TinyGPSPlus::libraryVersion());
    DEBUG_SERIAL.println("by Mikal Hart");
    DEBUG_SERIAL.println();
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

bool HAL::GPS_GetInfo(HAL::GPS_Info_t* info)
{
    if(!gps.location.isValid())
    {
        info->longitude = 116.391332;
        info->latitude = 39.907415;
        info->altitude = 53.0f;
        info->speed = 0.0f;
        info->clock.hour = gps.time.hour();
        info->clock.min = gps.time.minute();
        info->clock.sec = gps.time.second();
        info->compass = 0.0f;
    }
    else
    {
        info->longitude = gps.location.lng();
        info->latitude = gps.location.lat();
        info->altitude = gps.altitude.meters();
        info->speed = gps.speed.kmph();
        info->satellites = 0;
        Clock_GetValue(&info->clock);
        info->compass = gps.course.deg();
    }

    info->satellites = gps.satellites.value();
    
    return true;
}
