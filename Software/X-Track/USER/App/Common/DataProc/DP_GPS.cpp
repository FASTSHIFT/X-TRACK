#include "DataProc.h"
#include "../HAL/HAL.h"

typedef enum{
    GPS_STATUS_DISCONNECT,
    GPS_STATUS_UNSTABLE,
    GPS_STATUS_CONNECT,
}GPS_Status_t;

static int GPS_Callback(
    Account* pub,
    Account* sub,
    int msgType,
    void* data_p,
    uint32_t size
)
{
    if (size != sizeof(HAL::GPS_Info_t))
    {
        return -1;
    }

    HAL::GPS_GetInfo((HAL::GPS_Info_t*)data_p);

    return 0;
}

static void GPS_Update(Account* account)
{
    HAL::GPS_Info_t gpsInfo;
    bool isValid = GPS_GetInfo(&gpsInfo);
    
    static bool RTC_IsCalibrate = false;
    if(!RTC_IsCalibrate)
    {
        if(isValid)
        {
            Clock_SetInfo(&gpsInfo.clock);
            RTC_IsCalibrate = true;
        }
    }
    
    int satellites = gpsInfo.satellites;

    static GPS_Status_t lastStatus = GPS_STATUS_DISCONNECT;
    
    GPS_Status_t nowStatus;

    if(satellites > 6)
    {
        nowStatus = GPS_STATUS_CONNECT;
    }
    else if(satellites >= 3)
    {
        nowStatus = GPS_STATUS_UNSTABLE;
    }
    else
    {
        nowStatus = GPS_STATUS_DISCONNECT;
    }
    
    if(nowStatus != lastStatus)
    {
        const char* music[] = {
            "Disconnect",
            "UnstableConnect",
            "Connect"
        };
        
        HAL::Audio_PlayMusic(music[nowStatus]);
        lastStatus = nowStatus;
    }
}

Account* DataProc::GPS_Init(DataCenter* center)
{
    Account* account = new Account("GPS", center);
    account->SetPullCallback(GPS_Callback);
    account->SetTimerCallback(GPS_Update, 500);
    return account;
}
