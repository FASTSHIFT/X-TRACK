#include "DataProc.h"
#include "Utils/Filters/Filters.h"
#include "../HAL/HAL.h"
#include "Config/Config.h"

#define CALORIC_CORFFICIENT 0.5f

using namespace DataProc;

static HAL::SportStatus_Info_t sportStatus;

static double SportStatus_GetDistanceOffset(HAL::GPS_Info_t* gpsInfo)
{
    static bool isFirst = true;
    static double preLongitude;
    static double preLatitude;

    double offset = 0.0f;

    if (!isFirst)
    {
        offset = HAL::GPS_GetDistanceOffset(gpsInfo, preLongitude, preLatitude);
    }
    else
    {
        isFirst = false;
    }

    preLongitude = gpsInfo->longitude;
    preLatitude = gpsInfo->latitude;

    return offset;
}

static void onTimer(Account* account)
{
    HAL::GPS_Info_t gpsInfo;
    if(account->Pull("GPS", &gpsInfo, sizeof(gpsInfo)) != Account::RES_OK)
    {
        return;
    }

    uint32_t timeElaps = DataProc::GetTickElaps(sportStatus.lastTick);

    float speedKph = 0.0f;
    bool isSignalInterruption = (gpsInfo.isVaild && (gpsInfo.satellites == 0));

    if (gpsInfo.satellites >= 3)
    {
        float spd = gpsInfo.speed;
        speedKph = spd > 1 ? spd : 0;
    }

    if (speedKph > 0.0f || isSignalInterruption)
    {
        sportStatus.singleTime += timeElaps;
        sportStatus.totalTime += timeElaps;

        if (speedKph > 0.0f)
        {
            float dist = (float)SportStatus_GetDistanceOffset(&gpsInfo);

            sportStatus.singleDistance += dist;
            sportStatus.totalDistance += dist;

            float meterPerSec = sportStatus.singleDistance * 1000 / sportStatus.singleTime;
            sportStatus.speedAvgKph = meterPerSec * 3.6f;

            if (speedKph > sportStatus.speedMaxKph)
            {
                sportStatus.speedMaxKph = speedKph;
            }

            float calorie = speedKph * sportStatus.weight * CALORIC_CORFFICIENT * timeElaps / 1000 / 3600;
            sportStatus.singleCalorie += calorie;
        }
    }

    sportStatus.speedKph = speedKph;

    sportStatus.lastTick = DataProc::GetTick();
    account->Commit(&sportStatus, sizeof(sportStatus));
    account->Publish();
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event == Account::EVENT_TIMER)
    {
        onTimer(account);
        return Account::RES_OK;
    }

    if (param->event != Account::EVENT_SUB_PULL)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(sportStatus))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    memcpy(param->data_p, &sportStatus, param->size);
    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(SportStatus)
{
    memset(&sportStatus, 0, sizeof(sportStatus));
    sportStatus.weight = CONFIG_WEIGHT_DEFAULT;

    account->Subscribe("GPS");
    account->Subscribe("Storage");

    STORAGE_VALUE_REG(account, sportStatus.totalDistance, STORAGE_TYPE_FLOAT);
    STORAGE_VALUE_REG(account, sportStatus.totalTimeUINT32[0], STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(account, sportStatus.totalTimeUINT32[1], STORAGE_TYPE_INT);
    STORAGE_VALUE_REG(account, sportStatus.speedMaxKph, STORAGE_TYPE_FLOAT);
    STORAGE_VALUE_REG(account, sportStatus.weight, STORAGE_TYPE_FLOAT);

    sportStatus.lastTick = DataProc::GetTick();

    account->SetEventCallback(onEvent);
    account->SetTimerPeriod(500);
}
