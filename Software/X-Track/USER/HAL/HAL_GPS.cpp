#include "HAL.h"
#include "TinyGPSPlus/src/TinyGPS++.h"

#if CONFIG_SENSOR_PRESSURE_ENABLE
#include "BMP280/BMP280.h"
#endif


#define GPS_SERIAL             CONFIG_GPS_SERIAL
#define DEBUG_SERIAL           CONFIG_DEBUG_SERIAL
#define GPS_USE_TRANSPARENT    CONFIG_GPS_USE_TRANSPARENT

static TinyGPSPlus gps;

#if CONFIG_SENSOR_PRESSURE_ENABLE
BMP280 bmp;
double altitude = 0;
double prossure_base = -1;
double pressure = 0;
double temperature = 0;
#endif

void HAL::GPS_Init()
{
    GPS_SERIAL.begin(9600);

    Serial.print("GPS: TinyGPS++ library v. ");
    Serial.print(TinyGPSPlus::libraryVersion());
    Serial.println(" by Mikal Hart");
#if CONFIG_SENSOR_PRESSURE_ENABLE
    bmp.begin();
    bmp.setOversampling(4);
#endif
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
        gps.encode(c);
    }

#if GPS_USE_TRANSPARENT
    while (DEBUG_SERIAL.available() > 0)
    {
        GPS_SERIAL.write(DEBUG_SERIAL.read());
    }
#endif
#if CONFIG_SENSOR_PRESSURE_ENABLE
    if (bmp.startMeasurment() != 0){
        double t, p;
        if (bmp.getTemperatureAndPressure(t, p) != 0){
            temperature = t;
            pressure = p;
            // 海平面气压校准
            if (gps.location.isValid() && prossure_base < 0){
                prossure_base = bmp.sealevel(p, gps.altitude.meters());
            }
            if (prossure_base > 0){
                altitude = bmp.altitude(temperature, p, prossure_base);
            }else{
                altitude = 0.0;
            }
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
#if !CONFIG_SENSOR_PRESSURE_ENABLE    
    info->altitude = gps.altitude.meters();
#else
    info->altitude = altitude;
    info->temperature = temperature;
    info->pressure = pressure;
#endif
    info->speed = gps.speed.kmph();
    info->course = gps.course.deg();

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
