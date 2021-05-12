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

int LiveMapModel::onEvent(Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_PUB_PUBLISH)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    LiveMapModel* instance = (LiveMapModel*)param->recv->UserData;

    if (param->size != sizeof(HAL::SportStatus_Info_t))
    {
        return -1;
    }
    memcpy(&(instance->sportStatusInfo), param->data_p, param->size);

    return 0;
}
