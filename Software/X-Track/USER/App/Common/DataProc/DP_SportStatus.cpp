#include "DataProc.h"
#include "Utils/Filters/Filters.h"
#include "../HAL/HAL.h"

/* SportStatus */
static HAL::SportStatus_Info_t sportStatusInfo;
static MedianQueueFilter_TypeDef speedFilter;

static int SportStatus_Callback(
    Account* pub,
    Account* sub,
    int msgType, 
    void* data_p, 
    uint32_t size
)
{
    if (size != sizeof(sportStatusInfo))
    {
        return -1;
    }

    memcpy(data_p, &sportStatusInfo, size);
    return 0;
}

static void SportStatus_Update(Account* account)
{
    HAL::GPS_Info_t gpsInfo;
    account->Pull("GPS", 0, &gpsInfo, sizeof(gpsInfo));

    uint32_t timeElaps = DataProc::GetTickElaps(sportStatusInfo.lastTick);

    float speedKph;

    if (gpsInfo.satellites >= 3)
    {
        float spd = MedianQueueFilter_GetNext(&speedFilter, gpsInfo.speed * 100) / 100.0f;
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

        sportStatusInfo.singleDistance += dist;
        sportStatusInfo.totalDistance += dist;

        if (speedKph > sportStatusInfo.speedMaxKph)
        {
            sportStatusInfo.speedMaxKph = speedKph;
        }

        float meterPerSec = sportStatusInfo.singleDistance * 1000 / sportStatusInfo.totalTime;
        sportStatusInfo.speedAvgKph = meterPerSec * 3.6f;

        uint32_t kcal = speedKph * 65 * 1.05f * sportStatusInfo.singleTime / 1000 / 3600;
        sportStatusInfo.singleKcal += kcal;
    }

    sportStatusInfo.speedKph = speedKph;

    sportStatusInfo.lastTick = DataProc::GetTick();
    account->Publish(0, &sportStatusInfo, sizeof(sportStatusInfo));
}

Account* DataProc::SportStatus_Init(DataCenter* center)
{
    Account* account = new Account("SportStatus", center);
    account->Subscribe("GPS", SportStatus_Callback);
    sportStatusInfo.lastTick = GetTick();
    
    #define SPD_BUF_SIZE 3
    static int32_t spdBuf[SPD_BUF_SIZE];
    static int32_t spdSortBuf[SPD_BUF_SIZE];
    MedianQueueFilter_Init(&speedFilter, spdBuf, spdSortBuf, SPD_BUF_SIZE);
    
    account->SetTimerCallback(SportStatus_Update, 500);
    return account;
}
