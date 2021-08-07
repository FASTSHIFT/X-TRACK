#include "LiveMapModel.h"
#include "Config/Config.h"

using namespace Page;

LiveMapModel::LiveMapModel()
{

}

void LiveMapModel::Init()
{
    account = new Account("LiveMapModel", DataProc::Center(), 0, this);
    account->Subscribe("GPS");
    account->Subscribe("SportStatus");
    account->Subscribe("TrackFilter");
    account->Subscribe("SysConfig");
    account->SetEventCallback(onEvent);
}

void LiveMapModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }

    std::vector<TileConv::Point_t, lv_allocator<TileConv::Point_t>> vec;
    trackPoints.swap(vec);
}

void LiveMapModel::GetGPS_Info(HAL::GPS_Info_t* info)
{
    account->Pull("GPS", info, sizeof(HAL::GPS_Info_t));

    /* Default location : Tian An Men */
    if (!info->isVaild)
    {
        DataProc::SysConfig_Info_t sysConfig;
        account->Pull("SysConfig", &sysConfig, sizeof(sysConfig));
        info->longitude = sysConfig.longitudeDefault;
        info->latitude = sysConfig.latitudeDefault;
    }
}

void LiveMapModel::GetArrowTheme(char* buf, uint32_t size)
{
    DataProc::SysConfig_Info_t sysConfig;
    account->Pull("SysConfig", &sysConfig, sizeof(sysConfig));
    strncpy(buf, sysConfig.arrowTheme, size);
}

int LiveMapModel::onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_PUB_PUBLISH)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (strcmp(param->tran->ID, "SportStatus") != 0
            || param->size != sizeof(HAL::SportStatus_Info_t))
    {
        return -1;
    }

    LiveMapModel* instance = (LiveMapModel*)account->UserData;
    memcpy(&(instance->sportStatusInfo), param->data_p, param->size);

    return 0;
}

void LiveMapModel::TrackAddPoint(int32_t x, int32_t y)
{
    TileConv::Point_t point = { x, y };
    trackPoints.push_back(point);
}

void LiveMapModel::TrackReload()
{
    DataProc::TrackFilter_Info_t info;
    account->Pull("TrackFilter", &info, sizeof(info));

    if (!info.isActive)
    {
        return;
    }

    TrackReset();

    TrackPointFilter ptFilter;

    ptFilter.SetOffsetThreshold(CONFIG_TRACK_FILTER_OFFSET_THRESHOLD);

    uint32_t size = info.size;
    DataProc::TrackFilter_Point_t* points = info.points;

    for (uint32_t i = 0; i < size; i++)
    {
        int32_t mapX, mapY;
        mapConv.ConvertMapCoordinate(
            points[i].longitude, points[i].latitude,
            &mapX, &mapY
        );

        if (ptFilter.PushPoint(mapX, mapY))
        {
            TrackAddPoint(mapX, mapY);
        }
    }
}

void LiveMapModel::TrackReset()
{
    trackPoints.clear();
    pointFilter.Reset();
}

