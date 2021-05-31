#include "DataProc.h"
#include "Utils/Filters/Filters.h"
#include "../HAL/HAL.h"

using namespace DataProc;

#ifndef ABS
#  define ABS(x) (((x)>0)?(x):-(x))
#endif

static HAL::SportStatus_Info_t sportStatus;

static int onEvent(Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_SUB_PULL)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(sportStatus))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    memcpy(param->data_p, &sportStatus, param->size);
    return 0;
}

static double SportStatus_GetRealDistance(HAL::GPS_Info_t* gpsInfo, double dist)
{
    static double preLongitude;
    static double preLatitude;

    double offset = HAL::GPS_GetDistanceOffset(gpsInfo, preLongitude, preLatitude);

    preLongitude = gpsInfo->longitude;
    preLatitude = gpsInfo->latitude;

    double realDist = ABS(offset - dist) < 100 ? offset : dist;

    return realDist;
}

static void SportStatus_Update(Account* account)
{
    HAL::GPS_Info_t gpsInfo;
    account->Pull("GPS", &gpsInfo, sizeof(gpsInfo));

    uint32_t timeElaps = DataProc::GetTickElaps(sportStatus.lastTick);

    float speedKph;

    if (gpsInfo.satellites >= 3)
    {
        float spd = gpsInfo.speed;//speedFilter.GetNext(gpsInfo.speed);
        speedKph = spd > 1 ? spd : 0;
    }
    else
    {
        speedKph = 0;
    }

    if (speedKph > 0.0f)
    {
        sportStatus.singleTime += timeElaps;
        sportStatus.totalTime += timeElaps;
        float dist = speedKph / 3.6f * timeElaps / 1000;

        dist = SportStatus_GetRealDistance(&gpsInfo, dist);

        sportStatus.singleDistance += dist;
        sportStatus.totalDistance += dist;

        if (speedKph > sportStatus.speedMaxKph)
        {
            sportStatus.speedMaxKph = speedKph;
        }

        float meterPerSec = sportStatus.singleDistance * 1000 / sportStatus.totalTime;
        sportStatus.speedAvgKph = meterPerSec * 3.6f;

        float calorie = speedKph * 65 * 1.05f * timeElaps / 1000 / 3600;
        sportStatus.singleCalorie += calorie;
    }

    sportStatus.speedKph = speedKph;

    sportStatus.lastTick = DataProc::GetTick();
    account->Commit(&sportStatus, sizeof(sportStatus));
    account->Publish();
}

void DP_SportStatus_Init(Account* act)
{
    act->Subscribe("GPS");
    act->Subscribe("Storage");

#define STORAGE_VALUE_REG(data, dataType)\
do{\
    Storage_Info_t info; \
    info.cmd = STORAGE_CMD_ADD; \
    info.key = #data; \
    info.value = &data; \
    info.size = sizeof(data); \
    info.type = dataType; \
    act->Notify("Storage", &info, sizeof(info)); \
}while(0)

    STORAGE_VALUE_REG(sportStatus.totalDistance, STORAGE_TYPE_FLOAT);
    STORAGE_VALUE_REG(sportStatus.totalTimeUINT32[0], STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(sportStatus.totalTimeUINT32[1], STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(sportStatus.speedMaxKph, STORAGE_TYPE_FLOAT);

    sportStatus.lastTick = DataProc::GetTick();
    
    act->SetTimerCallback(SportStatus_Update, 500);
    act->SetEventCallback(onEvent);
}
