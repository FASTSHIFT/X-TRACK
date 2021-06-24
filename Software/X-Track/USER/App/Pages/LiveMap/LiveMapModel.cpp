#include "LiveMapModel.h"

void LiveMapModel::Init()
{
    mapConv.SetMapCalibration(0.00610, 0.00130);
    //mapConv.SetMapCalibration(0.00495, -0.0033);

    account = new Account("LiveMapModel", DataProc::Center(), 0, this);
    account->Subscribe("GPS");
    account->Subscribe("SportStatus");
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

void LiveMapModel::TrackReset()
{
    trackPoints.clear();
    pointFilter.Reset();
}

