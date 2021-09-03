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
}

void LiveMapModel::GetGPS_Info(HAL::GPS_Info_t* info)
{
    account->Pull("GPS", info, sizeof(HAL::GPS_Info_t));

    /* Default location : Tian An Men */
    if (!info->isVaild)
    {
        DataProc::SysConfig_Info_t sysConfig;
        account->Pull("SysConfig", &sysConfig, sizeof(sysConfig));
        info->longitude = sysConfig.longitude;
        info->latitude = sysConfig.latitude;
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

void LiveMapModel::TrackReload(TrackPointFilter::Callback_t callback, void* userData)
{
    DataProc::TrackFilter_Info_t info;
    account->Pull("TrackFilter", &info, sizeof(info));

    if (!info.isActive)
    {
        return;
    }

    pointFilter.Reset();

    TrackPointFilter ptFilter;

    ptFilter.SetOffsetThreshold(CONFIG_TRACK_FILTER_OFFSET_THRESHOLD);
    ptFilter.SetOutputPointCallback(callback);
    ptFilter.SetSecondFilterModeEnable(true);
    ptFilter.userData = userData;

    uint32_t size = info.size;
    DataProc::TrackFilter_Point_t* points = info.points;

    for (uint32_t i = 0; i < size; i++)
    {
        int32_t mapX, mapY;
        mapConv.ConvertMapPos(
            &mapX, &mapY,
            points[i].x, points[i].y, info.level
        );

        ptFilter.PushPoint(mapX, mapY);
    }
    ptFilter.PushEnd();
}
