#include "DataProc.h"
#include "Utils/GPX/GPX.h"
#include <stdio.h>
#include "Config/Config.h"
#include "Version.h"

using namespace DataProc;

static Recorder_Info_t recInfo;
static lv_fs_file_t file;
static bool recActive = false;

static lv_fs_res_t Recorder_FileWriteString(const char* str)
{
    //LV_LOG_USER(str);
    //printf(str);

    lv_fs_res_t res = lv_fs_write(
                          &file,
                          str,
                          strlen(str),
                          nullptr
                      );

    return res;
}

static int Recorder_GetTimeConv(
    Account* account,
    const char* format,
    char* buf,
    uint32_t size)
{
    HAL::Clock_Info_t clock;
    account->Pull("Clock", &clock, sizeof(clock));

    int ret = snprintf(
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
    return ret;
}

static void Recorder_RecPoint(Account* account, GPX* gpx, HAL::GPS_Info_t* gpsInfo)
{
    //LV_LOG_USER("Track recording...");

    char timeBuf[64];
    Recorder_GetTimeConv(
        account,
        "%d-%02d-%02dT%02d:%02d:%02dZ",
        timeBuf,
        sizeof(timeBuf)
    );

    gpx->setEle(String(gpsInfo->altitude, 2));
    gpx->setTime(timeBuf);

    String gpxStr = gpx->getPt(
                        GPX_TRKPT,
                        String(gpsInfo->longitude, 6),
                        String(gpsInfo->latitude, 6)
                    );

    Recorder_FileWriteString(gpxStr.c_str());
}

static void Recorder_RecStart(Account* account, GPX* gpx, uint16_t time)
{
    LV_LOG_USER("Track record start");

    char filepath[128];
    Recorder_GetTimeConv(
        account,
        "/" CONFIG_TRACK_RECORD_FILE_DIR_NAME "/TRK_%d%02d%02d_%02d%02d%02d.gpx",
        filepath, sizeof(filepath)
    );

    lv_fs_res_t res = lv_fs_open(&file, filepath, LV_FS_MODE_WR | LV_FS_MODE_RD);

    if (res == LV_FS_RES_OK)
    {
        LV_LOG_USER("Track file %s open success", filepath);

        gpx->setMetaName(VERSION_FIRMWARE_NAME " " VERSION_SOFTWARE);
        gpx->setMetaDesc(VERSION_PROJECT_LINK);
        gpx->setName(filepath);
        gpx->setDesc("");

        Recorder_FileWriteString(gpx->getOpen().c_str());
        Recorder_FileWriteString(gpx->getMetaData().c_str());
        Recorder_FileWriteString(gpx->getTrakOpen().c_str());
        Recorder_FileWriteString(gpx->getInfo().c_str());
        Recorder_FileWriteString(gpx->getTrakSegOpen().c_str());

        recActive = true;
    }
    else
    {
        LV_LOG_USER("Track file open error!");
    }
}

static void Recorder_RecStop(Account* account, GPX* gpx)
{
    recActive = false;

    Recorder_FileWriteString(gpx->getTrakSegClose().c_str());;
    Recorder_FileWriteString(gpx->getTrakClose().c_str());
    Recorder_FileWriteString(gpx->getClose().c_str());
    lv_fs_close(&file);

    LV_LOG_USER("Track record end");
}

static int onNotify(Account* account, Recorder_Info_t* info)
{
    int retval = 0;

    GPX* gpx = (GPX*)account->UserData;

    switch (info->cmd)
    {
    case RECORDER_CMD_START:
        Recorder_RecStart(account, gpx, info->time);
        break;
    case RECORDER_CMD_PAUSE:
        recActive = false;
        LV_LOG_USER("Track record pause");
        break;
    case RECORDER_CMD_CONTINUE:
        LV_LOG_USER("Track record continue");
        recActive = true;
        break;
    case RECORDER_CMD_STOP:
        Recorder_RecStop(account, gpx);
        break;
    }

    TrackFilter_Info_t tfInfo =
    {
        .cmd = (TrackFilter_Cmd_t)info->cmd
    };
    account->Notify("TrackFilter", &tfInfo, sizeof(tfInfo));

    return retval;
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    int retval = Account::ERROR_UNKNOW;

    switch (param->event)
    {
    case Account::EVENT_PUB_PUBLISH:
        if (param->size == sizeof(HAL::GPS_Info_t))
        {
            if (recActive)
            {
                GPX* gpx = (GPX*)account->UserData;
                Recorder_RecPoint(account, gpx, (HAL::GPS_Info_t*)param->data_p);
            }
            retval = Account::ERROR_NONE;
        }
        else
        {
            retval = Account::ERROR_SIZE_MISMATCH;
        }
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
            retval = onNotify(account, (Recorder_Info_t*)param->data_p);
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
    static GPX gpx;
    account->UserData = &gpx;

    account->Subscribe("GPS");
    account->Subscribe("Clock");
    account->Subscribe("TrackFilter");
    account->SetEventCallback(onEvent);
}
