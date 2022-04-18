#include "DataProc.h"
#include "Utils/MapConv/MapConv.h"
#include "Utils/TrackFilter/TrackFilter.h"
#include "Utils/PointContainer/PointContainer.h"
#include "Config/Config.h"

#include <vector>

using namespace DataProc;

typedef struct
{
    MapConv mapConv;
    TrackPointFilter pointFilter;
    PointContainer* pointContainer;
    bool isStarted;
    bool isActive;
} TrackFilter_t;

static TrackFilter_t trackFilter;

static void onNotify(Account* account, TrackFilter_Info_t* info)
{
    switch (info->cmd)
    {
    case TRACK_FILTER_CMD_START:
        trackFilter.pointContainer = new PointContainer;
        trackFilter.pointFilter.Reset();
        trackFilter.isActive = true;
        trackFilter.isStarted = true;
        LV_LOG_USER("Track filter start");
        break;
    case TRACK_FILTER_CMD_PAUSE:
        trackFilter.isActive = false;
        LV_LOG_USER("Track filter pause");
        break;
    case TRACK_FILTER_CMD_CONTINUE:
        trackFilter.isActive = true;
        LV_LOG_USER("Track filter continue");
        break;
    case TRACK_FILTER_CMD_STOP:
    {
        trackFilter.isStarted = false;
        trackFilter.isActive = false;

        if (trackFilter.pointContainer)
        {
            delete trackFilter.pointContainer;
            trackFilter.pointContainer = nullptr;
        }

        uint32_t sum = 0, output = 0;
        trackFilter.pointFilter.GetCounts(&sum, &output);
        LV_LOG_USER(
            "Track filter stop, filted(%d%%): sum = %d, output = %d",
            sum ? (100 - output * 100 / sum) : 0,
            sum,
            output
        );
        break;
    }
    default:
        break;
    }
}

static void onPublish(Account* account, HAL::GPS_Info_t* gps)
{
    int32_t mapX, mapY;
    trackFilter.mapConv.ConvertMapCoordinate(
        gps->longitude,
        gps->latitude,
        &mapX,
        &mapY
    );

    if (trackFilter.pointFilter.PushPoint(mapX, mapY))
    {
        trackFilter.pointContainer->PushPoint(mapX, mapY);
    }
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event == Account::EVENT_PUB_PUBLISH
            && param->size == sizeof(HAL::GPS_Info_t))
    {
        if (trackFilter.isActive)
        {
            onPublish(account, (HAL::GPS_Info_t*)param->data_p);
        }

        return Account::RES_OK;
    }

    if (param->size != sizeof(TrackFilter_Info_t))
    {
        return Account::RES_SIZE_MISMATCH;
    }

    switch (param->event)
    {
    case Account::EVENT_SUB_PULL:
    {
        TrackFilter_Info_t* info = (TrackFilter_Info_t*)param->data_p;
        info->pointCont = trackFilter.pointContainer;
        info->level = (uint8_t)trackFilter.mapConv.GetLevel();
        info->isActive = trackFilter.isStarted;
        break;
    }
    case Account::EVENT_NOTIFY:
        onNotify(account, (TrackFilter_Info_t*)param->data_p);
        break;

    default:
        break;
    }

    return Account::RES_OK;
}

DATA_PROC_INIT_DEF(TrackFilter)
{
    account->Subscribe("GPS");
    account->SetEventCallback(onEvent);

    trackFilter.pointContainer = nullptr;
    trackFilter.isActive = false;
    trackFilter.isStarted = false;

    trackFilter.mapConv.SetLevel(CONFIG_LIVE_MAP_LEVEL_DEFAULT);

    trackFilter.pointFilter.SetOffsetThreshold(CONFIG_TRACK_FILTER_OFFSET_THRESHOLD);
}
