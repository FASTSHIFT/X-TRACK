#include "HAL.h"
#include "TinyGPSPlus/src/TinyGPS++.h"

#define GPS_SERIAL             CONFIG_GPS_SERIAL
#define GPS_USE_TRANSPARENT    CONFIG_GPS_USE_TRANSPARENT

static TinyGPSPlus gps;

void HAL::GPS_Init()
{
    GPS_SERIAL.begin(9600);

    Serial.print("GPS: TinyGPS++ library v. ");
    Serial.print(TinyGPSPlus::libraryVersion());
    Serial.println(" by Mikal Hart");
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
    memset(info, 0, sizeof(GPS_Info_t));

    info->isVaild = gps.location.isValid();
    info->longitude = gps.location.lng();
    info->latitude = gps.location.lat();
    info->altitude = gps.altitude.meters();
    info->speed = gps.speed.kmph();
    info->compass = gps.course.deg();

    info->clock.year = gps.date.year();
    info->clock.month = gps.date.month();
    info->clock.day = gps.date.day();
    info->clock.hour = gps.time.hour();
    info->clock.minute = gps.time.minute();
    info->clock.second = gps.time.second();
    info->satellites = gps.satellites.value();

    return info->isVaild;
}

bool HAL::GPS_LocationIsValid()
{
    return gps.location.isValid();
}

double HAL::GPS_GetDistanceOffset(GPS_Info_t* info,  double preLong, double preLat)
{
    return gps.distanceBetween(info->latitude, info->longitude, preLat, preLong);
}
