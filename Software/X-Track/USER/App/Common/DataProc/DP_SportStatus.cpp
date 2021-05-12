#include "DataProc.h"
#include "Utils/Filters/Filters.h"
#include "../HAL/HAL.h"

#ifndef ABS
#  define ABS(x) (((x)>0)?(x):-(x))
#endif

/* SportStatus */
static HAL::SportStatus_Info_t sportStatusInfo;

//static Filter::MedianQueue<float, 3> speedFilter;

static int onEvent(Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_SUB_PULL)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(sportStatusInfo))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    memcpy(param->data_p, &sportStatusInfo, param->size);
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

    uint32_t timeElaps = DataProc::GetTickElaps(sportStatusInfo.lastTick);

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
        sportStatusInfo.singleTime += timeElaps;
        sportStatusInfo.totalTime += timeElaps;
        float dist = speedKph / 3.6f * timeElaps / 1000;

        dist = SportStatus_GetRealDistance(&gpsInfo, dist);

        sportStatusInfo.singleDistance += dist;
        sportStatusInfo.totalDistance += dist;

        if (speedKph > sportStatusInfo.speedMaxKph)
        {
            sportStatusInfo.speedMaxKph = speedKph;
        }

        float meterPerSec = sportStatusInfo.singleDistance * 1000 / sportStatusInfo.totalTime;
        sportStatusInfo.speedAvgKph = meterPerSec * 3.6f;

        float calorie = speedKph * 65 * 1.05f * timeElaps / 1000 / 3600;
        sportStatusInfo.singleCalorie += calorie;
    }

    sportStatusInfo.speedKph = speedKph;

    sportStatusInfo.lastTick = DataProc::GetTick();
    account->Commit(&sportStatusInfo, sizeof(sportStatusInfo));
    account->Publish();
}

void DP_SportStatus_Register(DataCenter* center)
{
    Account* account = new Account("SportStatus", center, sizeof(HAL::SportStatus_Info_t));
    account->Subscribe("GPS");
    sportStatusInfo.lastTick = DataProc::GetTick();
    
    account->SetTimerCallback(SportStatus_Update, 500);
    account->SetEventCallback(onEvent);
}
