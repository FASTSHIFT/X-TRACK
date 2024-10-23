#include "LiveMapModel.h"
#include "Config/Config.h"
#include "Utils/PointContainer/PointContainer.h"

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
    account->Subscribe("StatusBar");
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
    memset(info, 0, sizeof(HAL::GPS_Info_t));
    if(account->Pull("GPS", info, sizeof(HAL::GPS_Info_t)) != Account::RES_OK)
    {
        return;
    }

    /* Use default location */
    if (!info->isVaild)
    {
        DataProc::SysConfig_Info_t sysConfig;
        if(account->Pull("SysConfig", &sysConfig, sizeof(sysConfig)) == Account::RES_OK)
        {
            info->longitude = sysConfig.longitude;
            info->latitude = sysConfig.latitude;
        }
    }
}

void LiveMapModel::GetArrowTheme(char* buf, uint32_t size)
{
    DataProc::SysConfig_Info_t sysConfig;
    if(account->Pull("SysConfig", &sysConfig, sizeof(sysConfig)) != Account::RES_OK)
    {
        buf[0] = '\0';
        return;
    }
    strncpy(buf, sysConfig.arrowTheme, size);
    buf[size - 1] = '\0';
}

bool LiveMapModel::GetTrackFilterActive()
{
    DataProc::TrackFilter_Info_t info;
    if(account->Pull("TrackFilter", &info, sizeof(info)) != Account::RES_OK)
    {
        return false;
    }

    return info.isActive;
}

int LiveMapModel::onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_PUB_PUBLISH)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }

    if (strcmp(param->tran->ID, "SportStatus") != 0
            || param->size != sizeof(HAL::SportStatus_Info_t))
    {
        return Account::RES_PARAM_ERROR;
    }

    LiveMapModel* instance = (LiveMapModel*)account->UserData;
    memcpy(&(instance->sportStatusInfo), param->data_p, param->size);

    return Account::RES_OK;
}

void LiveMapModel::TrackReload(TrackPointFilter::Callback_t callback, void* userData)
{
    DataProc::TrackFilter_Info_t info;
    if(account->Pull("TrackFilter", &info, sizeof(info)) != Account::RES_OK)
    {
        return;
    }

    if (!info.isActive || info.pointCont == nullptr)
    {
        return;
    }

    PointContainer* pointContainer = (PointContainer*)info.pointCont;

    pointContainer->PopStart();
    pointFilter.Reset();

    TrackPointFilter ptFilter;

    ptFilter.SetOffsetThreshold(CONFIG_TRACK_FILTER_OFFSET_THRESHOLD);
    ptFilter.SetOutputPointCallback(callback);
    ptFilter.SetSecondFilterModeEnable(true);
    ptFilter.userData = userData;

    int32_t pointX, pointY;
    while (pointContainer->PopPoint(&pointX, &pointY))
    {
        int32_t mapX, mapY;
        mapConv.ConvertMapLevelPos(
            &mapX, &mapY,
            pointX, pointY,
            info.level
        );

        ptFilter.PushPoint(mapX, mapY);
    }
    ptFilter.PushEnd();
}

void LiveMapModel::SetStatusBarStyle(DataProc::StatusBar_Style_t style)
{
    DataProc::StatusBar_Info_t info;
    DATA_PROC_INIT_STRUCT(info);

    info.cmd = DataProc::STATUS_BAR_CMD_SET_STYLE;
    info.param.style = style;

    account->Notify("StatusBar", &info, sizeof(info));
}
