#include "DataProc.h"
#include "../HAL/HAL.h"

typedef enum{
    GPS_STATUS_DISCONNECT,
    GPS_STATUS_UNSTABLE,
    GPS_STATUS_CONNECT,
}GPS_Status_t;

static int GPS_Callback(Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_SUB_PULL)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(HAL::GPS_Info_t))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    HAL::GPS_GetInfo((HAL::GPS_Info_t*)param->data_p);

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

void DP_GPS_Init(Account* account)
{
    account->SetEventCallback(GPS_Callback);
    account->SetTimerCallback(GPS_Update, 500);
}
