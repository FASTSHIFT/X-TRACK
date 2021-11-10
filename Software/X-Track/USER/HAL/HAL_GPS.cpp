#include "HAL.h"
#include "TinyGPSPlus/src/TinyGPS++.h"

#define GPS_SERIAL             CONFIG_GPS_SERIAL
#define DEBUG_SERIAL           CONFIG_DEBUG_SERIAL
#define GPS_USE_TRANSPARENT    CONFIG_GPS_USE_TRANSPARENT

static TinyGPSPlus gps_decoder;

void HAL::GPS_Init()
{
    GPS_SERIAL.begin(9600);

    Serial.print("GPS: TinyGPS++ library v. ");
    Serial.print(TinyGPSPlus::libraryVersion());
    Serial.println(" by Mikal Hart");
}

void HAL::GPS_Update()
{
#if CONFIG_GPS_BUF_OVERLOAD_CHK && !GPS_USE_TRANSPARENT
    int available = GPS_SERIAL.available();
    DEBUG_SERIAL.printf("GPS: Buffer available = %d", available);
    if(available >= SERIAL_RX_BUFFER_SIZE / 2)
    {
        DEBUG_SERIAL.print(", maybe overload!");
    }
    DEBUG_SERIAL.println();
#endif

    while (GPS_SERIAL.available() > 0)
    {
        char c = GPS_SERIAL.read();
#if GPS_USE_TRANSPARENT
        DEBUG_SERIAL.write(c);
#endif
        gps_decoder.encode(c);
    }

#if GPS_USE_TRANSPARENT
    while (DEBUG_SERIAL.available() > 0)
    {
        GPS_SERIAL.write(DEBUG_SERIAL.read());
    }
#endif
}

bool HAL::GPS_GetInfo(GPS_Info_t* info)
{
    memset(info, 0, sizeof(GPS_Info_t));

    info->isVaild = gps_decoder.location.isValid();
    info->longitude = gps_decoder.location.lng();
    info->latitude = gps_decoder.location.lat();
    info->altitude = gps_decoder.altitude.meters();
    info->speed = gps_decoder.speed.kmph();
    info->course = gps_decoder.course.deg();

    info->clock.year = ll
    .date.year();
    info->clock.month = gps_decoder.date.month();
    info->clock.day = gps_decoder.date.day();
    info->clock.hour = gps_decoder.time.hour();
    info->clock.minute = gps_decoder.time.minute();
    info->clock.second = gps_decoder.time.second();
    info->satellites = gps_decoder.satellites.value();

    return info->isVaild;
}

bool HAL::GPS_LocationIsValid()
{
    return gps_decoder.location.isValid();
}

double HAL::GPS_GetDistanceOffset(GPS_Info_t* info,  double preLong, double preLat)
{
    return gps_decoder.distanceBetween(info->latitude, info->longitude, preLat, preLong);
}
