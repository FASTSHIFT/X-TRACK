#include "DataProc.h"
#include "Utils/MapConv/MapConv.h"
#include "Utils/TrackFilter/TrackFilter.h"
#include "Config/Config.h"

#include <vector>
#include "Utils/lv_allocator/lv_allocator.h"

using namespace DataProc;

static MapConv mapConv;

static TrackPointFilter pointFilter;

static bool filterStarted = false;
static bool filterActive = false;

typedef std::vector<TrackFilter_Point_t, lv_allocator<TrackFilter_Point_t>> PointVector_t;

static PointVector_t* veclocationPoints;

static int onNotify(Account* account, TrackFilter_Info_t* info)
{
    int retval = 0;

    switch (info->cmd)
    {
    case TRACK_FILTER_CMD_START:
        pointFilter.Reset();
        filterActive = true;
        filterStarted = true;
        LV_LOG_USER("Track filter start");
        break;
    case TRACK_FILTER_CMD_PAUSE:
        filterActive = false;
        LV_LOG_USER("Track filter pause");
        break;
    case TRACK_FILTER_CMD_CONTINUE:
        filterActive = true;
        LV_LOG_USER("Track filter continue");
        break;
    case TRACK_FILTER_CMD_STOP:
    {
        filterStarted = false;
        filterActive = false;

        PointVector_t vec;
        veclocationPoints->swap(vec);

        uint32_t sum = 0, output = 0;
        pointFilter.GetCounts(&sum, &output);
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

    return retval;
}

static void onPublish(Account* account, HAL::GPS_Info_t* gps)
{
    int32_t mapX, mapY;
    mapConv.ConvertMapCoordinate(
        gps->longitude,
        gps->latitude,
        &mapX,
        &mapY
    );

    if (pointFilter.PushPoint(mapX, mapY))
    {
        const TrackFilter_Point_t point = { mapX, mapY };
        veclocationPoints->push_back(point);
    }
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    int retval = Account::ERROR_UNKNOW;

    if (param->event == Account::EVENT_PUB_PUBLISH
     && param->size == sizeof(HAL::GPS_Info_t))
    {
        if (filterActive)
        {
            onPublish(account, (HAL::GPS_Info_t*)param->data_p);
        }
        
        return 0;
    }

    if (param->size != sizeof(TrackFilter_Info_t))
    {
        return Account::ERROR_SIZE_MISMATCH;
    }

    switch (param->event)
    {
    case Account::EVENT_SUB_PULL:
    {
        TrackFilter_Info_t* info = (TrackFilter_Info_t*)param->data_p;
        info->points = veclocationPoints->size() ? &((*veclocationPoints)[0]) : nullptr;
        info->size = veclocationPoints->size();
        info->level = (uint8_t)mapConv.GetLevel();
        info->isActive = filterStarted;
        break;
    }
    case Account::EVENT_NOTIFY:
        retval = onNotify(account, (TrackFilter_Info_t*)param->data_p);
        break;

    default:
        break;
    }

    return retval;
}

DATA_PROC_INIT_DEF(TrackFilter)
{
    account->Subscribe("GPS");
    account->SetEventCallback(onEvent);

    mapConv.SetLevel(CONFIG_LIVE_MAP_LEVEL_DEFAULT);

    static PointVector_t vec;
    veclocationPoints = &vec;

    pointFilter.SetOffsetThreshold(CONFIG_TRACK_FILTER_OFFSET_THRESHOLD);
}
