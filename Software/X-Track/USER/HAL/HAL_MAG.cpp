#include "HAL.h"
#include "LIS3MDL/LIS3MDL.h"

static LIS3MDL mag;

static char report[80];

void HAL::MAG_Init()
{
    if (!mag.init())
    {
        Serial.println("Failed to detect and initialize magnetometer!");
        while (1);
    }

    mag.enableDefault();
}

void HAL::MAG_Update()
{
    mag.read();

    snprintf(report, sizeof(report), "M: %6d %6d %6d",
             mag.m.x, mag.m.y, mag.m.z);
    Serial.println(report);

    delay(100);
}
