#include "HAL.h"

#if (CONFIG_MAG_MODEL_LIS3MDL + CONFIG_MAG_MODEL_QMC6310) != 1
#error Specificated too many MAG Sensor Model
#endif

#if CONFIG_MAG_MODEL_LIS3MDL
#include "LIS3MDL/LIS3MDL.h"

static LIS3MDL mag;
#elif CONFIG_MAG_MODEL_QMC6310
#include "QMC6310/QMC6310.h"
static QMC6310 mag;
#else
#error Unspecificate MAG Sensor Model
#endif
static HAL::CommitFunc_t CommitFunc = nullptr;
static void* UserData = nullptr;

bool HAL::MAG_Init()
{
    Serial.print("MAG: init...");
    bool success = false;
#if CONFIG_MAG_MODEL_LIS3MDL
    success = mag.init();
    if (success){
        mag.enableDefault();
    }
#elif CONFIG_MAG_MODEL_QMC6310
    success = mag.init();
    if (success){
        mag.enableDefault();
    }
#endif
    Serial.println(success ? "success" : "failed");
    return success;
}

void HAL::MAG_SetCommitCallback(CommitFunc_t func, void* userData)
{
    CommitFunc = func;
    UserData = userData;
}

void HAL::MAG_Update()
{
    MAG_Info_t magInfo;
#if CONFIG_MAG_MODEL_LIS3MDL
    mag.read();
    
    magInfo.x = mag.m.x;
    magInfo.y = mag.m.y;
    magInfo.z = mag.m.z;
    
#elif CONFIG_MAG_MODEL_QMC6310
    mag.read();

    magInfo.x = mag.m.x;
    magInfo.y = mag.m.y;
    magInfo.z = mag.m.z;
    
#endif

    // Serial.printf("%d,%d,%d\n", mag.m.x, mag.m.y, mag.m.z);

    if(CommitFunc)
    {
        CommitFunc(&magInfo, UserData);
    }
}
