#include "DataProc.h"
#include "Utils/GPX/GPX.h"
#include <stdio.h>

using namespace DataProc;

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

static void onTimer(Account* account)
{
    LV_LOG_USER("Track recording...");
    HAL::GPS_Info_t gps;
    account->Pull("GPS", &gps, sizeof(gps));

    String gpxStr = gpx.getPt(
        GPX_TRKPT,
        String(gps.longitude, 6),
        String(gps.latitude, 6)
    );

    Recorder_FileWriteString(gpxStr.c_str());
}

static int Recorder_GetFileName(Account* account, char* buf, uint32_t size)
{
    HAL::Clock_Info_t clock;
    account->Pull("Clock", &clock, sizeof(clock));

    int ret = snprintf(
        buf,
        size,
        "/Track/TRK_%04d%02d%02d_%02d%02d%02d.gpx",
        clock.year,
        clock.month,
        clock.day,
        clock.hour,
        clock.minute,
        clock.second
    );
    return ret;
}

static void Recorder_RecStart(Account* account, uint16_t time)
{
    LV_LOG_USER("Track record start");

    char filename[128];
    Recorder_GetFileName(account, filename, sizeof(filename));

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

        account->SetTimerPeriod(time);
    }
    else
    {
        LV_LOG_USER("Track file open error!");
    }
}

static void Recorder_RecStop(Account* account)
{
    account->SetTimerPeriod(0);

    Recorder_FileWriteString(gpx.getTrakSegClose().c_str());;
    Recorder_FileWriteString(gpx.getTrakClose().c_str());
    Recorder_FileWriteString(gpx.getClose().c_str());
    lv_fs_close(&file);

    LV_LOG_USER("Track record end");
}

static int onSignal(Account* account, Recorder_Info_t* info)
{
    int retval = 0;

    switch (info->cmd)
    {
    case RECORDER_CMD_START:
        Recorder_RecStart(account, info->time);
        break;
    case RECORDER_CMD_PAUSE:
        LV_LOG_USER("Track record pause");
        account->SetTimerEnable(false);
        break;
    case RECORDER_CMD_CONTINUE:
        LV_LOG_USER("Track record continue");
        account->SetTimerEnable(true);
        break;
    case RECORDER_CMD_STOP:
        Recorder_RecStop(account);
        break;
    }

    return retval;
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event == Account::EVENT_TIMER)
    {
        onTimer(account);
        return 0;
    }

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
            retval = onSignal(account, (Recorder_Info_t*)param->data_p);
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

DATA_PROC_INIT_DEF(Recorder)
{
    act->Subscribe("GPS");
    act->Subscribe("Clock");
    act->SetEventCallback(onEvent);
}
