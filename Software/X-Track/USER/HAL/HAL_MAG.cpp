#include "HAL.h"
#include "LIS3MDL/LIS3MDL.h"

extern void DP_MAG_Commit(const void* data, uint32_t size);

static LIS3MDL mag;

void HAL::MAG_Init()
{
    Serial.print("MAG: init...");
    Serial.println(mag.init() ? "success" : "failed");

    mag.enableDefault();
}

void HAL::MAG_Update()
{
    mag.read();

    //Serial.printf("%d,%d,%d\n", mag.m.x, mag.m.y, mag.m.z);
    
    MAG_Info_t magInfo;
    magInfo.x = mag.m.x;
    magInfo.y = mag.m.y;
    magInfo.z = mag.m.z;
    
    DP_MAG_Commit(&magInfo, sizeof(magInfo));
}
