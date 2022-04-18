#include <stdio.h>
#include "DataProc.h"
#include "Utils/GPX/GPX.h"
#include "Config/Config.h"
#include "Version.h"

using namespace DataProc;

#define RECORDER_GPX_TIME_FMT    "%d-%02d-%02dT%02d:%02d:%02dZ"
#define RECORDER_GPX_FILE_NAME   "/" CONFIG_TRACK_RECORD_FILE_DIR_NAME "/TRK_%d%02d%02d_%02d%02d%02d.gpx"
#define RECORDER_GPX_META_NAME   VERSION_FIRMWARE_NAME " " VERSION_SOFTWARE
#define RECORDER_GPX_META_DESC   VERSION_PROJECT_LINK

typedef struct
{
    GPX gpx;
    Recorder_Info_t recInfo;
    lv_fs_file_t file;
    bool active;
    Account* account;
} Recorder_t;

static lv_fs_res_t Recorder_FileWriteString(lv_fs_file_t* file_p, const char* str)
{
    //LV_LOG_USER(str);

    lv_fs_res_t res = lv_fs_write(
                          file_p,
                          str,
                          (uint32_t)strlen(str),
                          nullptr
                      );

    return res;
}

static int Recorder_GetTimeConv(
    Recorder_t* recorder,
    const char* format,
    char* buf,
    uint32_t size)
{
    HAL::Clock_Info_t clock;
    int retval = -1;
    if (recorder->account->Pull("Clock", &clock, sizeof(clock)) == Account::RES_OK)
    {
        retval = snprintf(
            buf,
            size,
            format,
            clock.year,
            clock.month,
            clock.day,
            clock.hour,
            clock.minute,
            clock.second
        );
    }

    return retval;
}

static void Recorder_RecPoint(Recorder_t* recorder, HAL::GPS_Info_t* gpsInfo)
{
    //LV_LOG_USER("Track recording...");

    char timeBuf[64];
    int ret = Recorder_GetTimeConv(
        recorder,
        RECORDER_GPX_TIME_FMT,
        timeBuf,
        sizeof(timeBuf)
    );

    if (ret < 0)
    {
        LV_LOG_WARN("cant't get time");
        return;
    }

    recorder->gpx.setEle(String(gpsInfo->altitude, 2));
    recorder->gpx.setTime(timeBuf);

    String gpxStr = recorder->gpx.getPt(
                        GPX_TRKPT,
                        String(gpsInfo->longitude, 6),
                        String(gpsInfo->latitude, 6)
                    );

    Recorder_FileWriteString(&(recorder->file), gpxStr.c_str());
}

static void Recorder_RecStart(Recorder_t* recorder, uint16_t time)
{
    LV_LOG_USER("Track record start");

    char filepath[128];
    int ret = Recorder_GetTimeConv(
        recorder,
        RECORDER_GPX_FILE_NAME,
        filepath, sizeof(filepath)
    );

    if (ret < 0)
    {
        LV_LOG_WARN("cant't get time");
        return;
    }

    lv_fs_res_t res = lv_fs_open(&(recorder->file), filepath, LV_FS_MODE_WR | LV_FS_MODE_RD);

    if (res == LV_FS_RES_OK)
    {
        LV_LOG_USER("Track file %s open success", filepath);

        GPX* gpx = &(recorder->gpx);
        lv_fs_file_t* file_p = &(recorder->file);

        gpx->setMetaName(RECORDER_GPX_META_NAME);
        gpx->setMetaDesc(RECORDER_GPX_META_DESC);
        gpx->setName(filepath);
        gpx->setDesc("");

        Recorder_FileWriteString(file_p, gpx->getOpen().c_str());
        Recorder_FileWriteString(file_p, gpx->getMetaData().c_str());
        Recorder_FileWriteString(file_p, gpx->getTrakOpen().c_str());
        Recorder_FileWriteString(file_p, gpx->getInfo().c_str());
        Recorder_FileWriteString(file_p, gpx->getTrakSegOpen().c_str());

        recorder->active = true;
    }
    else
    {
        LV_LOG_ERROR("Track file open error!");
    }
}

static void Recorder_RecStop(Recorder_t* recorder)
{
    recorder->active = false;
    GPX* gpx = &(recorder->gpx);
    lv_fs_file_t* file_p = &(recorder->file);

    Recorder_FileWriteString(file_p, gpx->getTrakSegClose().c_str());;
    Recorder_FileWriteString(file_p, gpx->getTrakClose().c_str());
    Recorder_FileWriteString(file_p, gpx->getClose().c_str());
    lv_fs_close(file_p);

    LV_LOG_USER("Track record end");
}

static int onNotify(Recorder_t* recorder, Recorder_Info_t* info)
{
    switch (info->cmd)
    {
    case RECORDER_CMD_START:
        Recorder_RecStart(recorder, info->time);
        break;
    case RECORDER_CMD_PAUSE:
        recorder->active = false;
        LV_LOG_USER("Track record pause");
        break;
    case RECORDER_CMD_CONTINUE:
        LV_LOG_USER("Track record continue");
        recorder->active = true;
        break;
    case RECORDER_CMD_STOP:
        Recorder_RecStop(recorder);
        break;
    }

    TrackFilter_Info_t tfInfo;
    DATA_PROC_INIT_STRUCT(tfInfo);
    tfInfo.cmd = (TrackFilter_Cmd_t)info->cmd;

    return recorder->account->Notify("TrackFilter", &tfInfo, sizeof(tfInfo));
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    Account::ResCode_t res = Account::RES_UNKNOW;
    Recorder_t* recorder = (Recorder_t*)account->UserData;;

    switch (param->event)
    {
    case Account::EVENT_PUB_PUBLISH:
        if (param->size == sizeof(HAL::GPS_Info_t))
        {
            if (recorder->active)
            {
                Recorder_RecPoint(recorder, (HAL::GPS_Info_t*)param->data_p);
            }
            res = Account::RES_OK;
        }
        else
        {
            res = Account::RES_SIZE_MISMATCH;
        }
        break;

    case Account::EVENT_SUB_PULL:
        if (param->size == sizeof(Recorder_Info_t))
        {
            memcpy(param->data_p, &(recorder->recInfo), param->size);
        }
        else
        {
            res = Account::RES_SIZE_MISMATCH;
        }
        break;

    case Account::EVENT_NOTIFY:
        if (param->size == sizeof(Recorder_Info_t))
        {
            onNotify(recorder, (Recorder_Info_t*)param->data_p);
            res = Account::RES_OK;
        }
        else
        {
            res = Account::RES_SIZE_MISMATCH;
        }
        break;

    default:
        break;
    }

    return res;
}

DATA_PROC_INIT_DEF(Recorder)
{
    static Recorder_t recorder;
    memset(&recorder.recInfo, 0, sizeof(recorder.recInfo));
    memset(&recorder.file, 0, sizeof(recorder.file));
    recorder.active = false;
    recorder.account = account;
    account->UserData = &recorder;

    account->Subscribe("GPS");
    account->Subscribe("Clock");
    account->Subscribe("TrackFilter");
    account->SetEventCallback(onEvent);
}
