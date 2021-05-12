#include "DataProc.h"
#include <stdio.h>

using namespace DataProc;

static Account* actRecorder;

static Recorder_Info_t recInfo;

static lv_fs_file_t file;

static void Recorder_Update(Account* account)
{
    HAL::GPS_Info_t gps;
    account->Pull("GPS", &gps, sizeof(gps));

    char buf[64];
    int len = snprintf(
        buf, sizeof(buf),
        "%02d:%02d:%02d,%lf,%lf,%f\n",
        gps.clock.hour,
        gps.clock.min,
        gps.clock.sec,
        gps.longitude,
        gps.latitude,
        gps.altitude
    );

    LV_LOG_USER(buf);

    uint32_t bw;
    lv_fs_write(&file, buf, len + 1, &bw);
}

static Recorder_State_t Recorder_RecStart(uint16_t time)
{
    lv_fs_open(&file, "/Track.csv", LV_FS_MODE_WR | LV_FS_MODE_RD);
    lv_fs_seek(&file, 0, LV_FS_SEEK_END);

    actRecorder->SetTimerCallback(Recorder_Update, time);

    return RECORDER_STOP;
}

static Recorder_State_t Recorder_RecStop()
{
    actRecorder->SetTimerCallback(nullptr, 0);

    lv_fs_close(&file);

    return RECORDER_START;
}

static int onSignal(Recorder_Info_t* info)
{
    int retval = 0;

    switch (info->state)
    {
    case RECORDER_START:
        recInfo.state = Recorder_RecStart(info->time);
        break;
    case RECORDER_PAUSE:
    case RECORDER_STOP:
        recInfo.state = Recorder_RecStop();
        break;
    }

    return retval;
}

static int onEvent(Account::EventParam_t* param)
{
    int retval = Account::ERROR_UNKNOW;

    switch (param->event)
    {
    case Account::EVENT_PUB_PUBLISH:       
        break;

    case Account::EVENT_SUB_PULL:
        if (param->size == sizeof(Recorder_Info_t))
        {
            memcpy(param->data_p, &recInfo, param->size);
        }
        else
        {
            retval = Account::ERROR_SIZE_MISMATCH;
        }
        break;

    case Account::EVENT_NOTIFY:
        if (param->size == sizeof(Recorder_Info_t))
        {
            retval = onSignal((Recorder_Info_t*)param->data_p);
        }
        else
        {
            retval = Account::ERROR_SIZE_MISMATCH;
        }
        break;

    default:
        break;
    }

    return retval;
}

void DP_Recorder_Register(DataCenter* center)
{
    Account* account = new Account("Recorder", center);
    account->Subscribe("GPS");
    account->SetEventCallback(onEvent);
    actRecorder = account;
}
