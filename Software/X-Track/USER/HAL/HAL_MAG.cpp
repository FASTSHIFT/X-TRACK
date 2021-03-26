#include "HAL.h"
#include "LIS3MDL/LIS3MDL.h"

static LIS3MDL mag;

void HAL::MAG_Init()
{
    Serial.print("MAG init...");
    Serial.println(mag.init() ? "success" : "failed");

    mag.enableDefault();
}

void HAL::MAG_Update()
{
    mag.read();

    Serial.printf("%d,%d,%d\n",
             mag.m.x, mag.m.y, mag.m.z);
}
