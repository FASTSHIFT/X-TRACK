#include "HAL.h"
#include "LIS3MDL/LIS3MDL.h"

static LIS3MDL mag;
static HAL::CommitFunc_t CommitFunc = nullptr;
static void* UserData = nullptr;

bool HAL::MAG_Init()
{
    Serial.print("MAG: init...");

    bool success = mag.init();

    Serial.println(success ? "success" : "failed");

    if(success)
    {
        mag.enableDefault();
    }

    return success;
}

void HAL::MAG_SetCommitCallback(CommitFunc_t func, void* userData)
{
    CommitFunc = func;
    UserData = userData;
}

void HAL::MAG_Update()
{
    mag.read();

    //Serial.printf("%d,%d,%d\n", mag.m.x, mag.m.y, mag.m.z);

    MAG_Info_t magInfo;
    magInfo.x = mag.m.x;
    magInfo.y = mag.m.y;
    magInfo.z = mag.m.z;

    if(CommitFunc)
    {
        CommitFunc(&magInfo, UserData);
    }
}
