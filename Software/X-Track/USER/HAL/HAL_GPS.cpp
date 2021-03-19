#include "HAL.h"
#include "TinyGPSPlus/src/TinyGPS++.h"

/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

#define ss Serial2

static void displayInfo();

void HAL::GPS_Init()
{
    Serial.begin(115200);
    ss.begin(GPSBaud);

    Serial.println(F("DeviceExample.ino"));
    Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
    Serial.print(F("Testing TinyGPS++ library v. "));
    Serial.println(TinyGPSPlus::libraryVersion());
    Serial.println(F("by Mikal Hart"));
    Serial.println();
}

void HAL::GPS_Update()
{
    // This sketch displays information every time a new sentence is correctly encoded.
    while (ss.available() > 0)
    {
        if (gps.encode(ss.read()))
        {
            //displayInfo();
        }
    }
}

static void displayInfo()
{
    Serial.print(F("Location: "));
    if (gps.location.isValid())
    {
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lng(), 6);
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F("  Date/Time: "));
    if (gps.date.isValid())
    {
        Serial.print(gps.date.month());
        Serial.print(F("/"));
        Serial.print(gps.date.day());
        Serial.print(F("/"));
        Serial.print(gps.date.year());
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F(" "));
    if (gps.time.isValid())
    {
        if (gps.time.hour() < 10) Serial.print(F("0"));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        if (gps.time.minute() < 10) Serial.print(F("0"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        if (gps.time.second() < 10) Serial.print(F("0"));
        Serial.print(gps.time.second());
        Serial.print(F("."));
        if (gps.time.centisecond() < 10) Serial.print(F("0"));
        Serial.print(gps.time.centisecond());
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.println();
}

bool HAL::GPS_GetInfo(HAL::GPS_Info_t* info)
{
    if(!gps.location.isValid())
    {
        info->longitude = 116.391332;
        info->latitude = 39.907415;
        info->altitude = 53.0f;
        info->speed = 0.0f;
        Clock_GetValue(&info->clock);
    }
    else
    {
        info->longitude = gps.location.lng();
        info->latitude = gps.location.lat();
        info->altitude = gps.altitude.meters();
        info->speed = gps.speed.mps();
        info->satellites = 0;
        info->clock.hour = gps.time.hour();
        info->clock.min = gps.time.minute();
        info->clock.sec = gps.time.second();
    }

    info->satellites = gps.satellites.value();
    
    return true;
}

