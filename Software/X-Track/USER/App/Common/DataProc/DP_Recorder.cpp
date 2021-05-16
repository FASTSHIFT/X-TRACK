#include "DataProc.h"
#include "Utils/GPX/GPX.h"
#include <stdio.h>

using namespace DataProc;

static Account* actRecorder;
static GPX gpx;
static Recorder_Info_t recInfo;
static lv_fs_file_t file;

static void Recorder_FileWriteString(const char* str)
{
    //LV_LOG_USER(str);
    //printf(str);
    lv_fs_write(
        &file,
        str,
        strlen(str),
        nullptr
    );
}

static void Recorder_Update(Account* account)
{
    HAL::GPS_Info_t gps;
    account->Pull("GPS", &gps, sizeof(gps));

    String gpxStr = gpx.getPt(
        GPX_TRKPT,
        String(gps.longitude, 6),
        String(gps.latitude, 6)
    );

    Recorder_FileWriteString(gpxStr.c_str());
}

static int Recorder_GetFileName(char* buf, uint32_t size)
{
    HAL::Clock_Info_t clock;
    actRecorder->Pull("Clock", &clock, sizeof(clock));

    int ret = snprintf(
        buf,
        size,
        "/Track/TRACK_%04d-%02d-%02d_%02d-%02d-%02d.gpx",
        clock.year,
        clock.month,
        clock.day,
        clock.hour,
        clock.minute,
        clock.second
    );
    return ret;
}

static Recorder_State_t Recorder_RecStart(uint16_t time)
{
    LV_LOG_USER("Track record start");

    char filename[128];
    Recorder_GetFileName(filename, sizeof(filename));

    lv_fs_res_t res = lv_fs_open(&file, filename, LV_FS_MODE_WR | LV_FS_MODE_RD);

    if (res == LV_FS_RES_OK)
    {
        LV_LOG_USER("Track file %s open success", filename);

        gpx.setMetaDesc("X-TRACK RECORDER");
        gpx.setName(filename);
        gpx.setDesc("X-TRACK " __TIMESTAMP__ " Build");
        gpx.setSrc("SUP500Ff");

        Recorder_FileWriteString(gpx.getOpen().c_str());
        Recorder_FileWriteString(gpx.getMetaData().c_str());
        Recorder_FileWriteString(gpx.getMetaData().c_str());
        Recorder_FileWriteString(gpx.getTrakOpen().c_str());
        Recorder_FileWriteString(gpx.getInfo().c_str());
        Recorder_FileWriteString(gpx.getTrakSegOpen().c_str());

        actRecorder->SetTimerCallback(Recorder_Update, time);
    }
    else
    {
        LV_LOG_USER("Track file open error!");
    }

    return RECORDER_STOP;
}

static Recorder_State_t Recorder_RecStop()
{
    actRecorder->SetTimerCallback(nullptr, 0);

    Recorder_FileWriteString(gpx.getTrakSegClose().c_str());;
    Recorder_FileWriteString(gpx.getTrakClose().c_str());
    Recorder_FileWriteString(gpx.getClose().c_str());
    lv_fs_close(&file);

    LV_LOG_USER("Track record end");

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
    account->Subscribe("Clock");
    account->SetEventCallback(onEvent);
    actRecorder = account;
}
