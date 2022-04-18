#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Config/Config.h"

typedef enum
{
    GPS_STATUS_DISCONNECT,
    GPS_STATUS_UNSTABLE,
    GPS_STATUS_CONNECT,
} GPS_Status_t;

static void onTimer(Account* account)
{
    HAL::GPS_Info_t gpsInfo;
    HAL::GPS_GetInfo(&gpsInfo);

    int satellites = gpsInfo.satellites;

    static GPS_Status_t nowStatus = GPS_STATUS_DISCONNECT;
    static GPS_Status_t lastStatus = GPS_STATUS_DISCONNECT;

    if (satellites > 7)
    {
        nowStatus = GPS_STATUS_CONNECT;
    }
    else if (satellites < 5 && satellites >= 3)
    {
        nowStatus = GPS_STATUS_UNSTABLE;
    }
    else if (satellites == 0)
    {
        nowStatus = GPS_STATUS_DISCONNECT;
    }

    if (nowStatus != lastStatus)
    {
        const char* music[] =
        {
            "Disconnect",
            "UnstableConnect",
            "Connect"
        };

        DataProc::MusicPlayer_Info_t info;
        DATA_PROC_INIT_STRUCT(info);
        info.music = music[nowStatus];
        account->Notify("MusicPlayer", &info, sizeof(info));
        lastStatus = nowStatus;
    }

    if (satellites >= 3)
    {
        account->Commit(&gpsInfo, sizeof(gpsInfo));
        account->Publish();
    }
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

    if (param->size != sizeof(HAL::GPS_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    HAL::GPS_GetInfo((HAL::GPS_Info_t*)param->data_p);

    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(GPS)
{
    account->Subscribe("MusicPlayer");

    account->SetEventCallback(onEvent);
    account->SetTimerPeriod(CONFIG_GPS_REFR_PERIOD);
}
