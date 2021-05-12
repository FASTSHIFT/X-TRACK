#include "DialplateModel.h"

void DialplateModel::Init()
{
    account = new Account("DialplateModel", DataProc::Center(), 0, this);
    account->Subscribe("SportStatus");
    account->Subscribe("Recorder");
    account->SetEventCallback(onEvent);
}

void DialplateModel::Deinit()
{
    if (account)
    {
        delete account;
        account = nullptr;
    }
}

int DialplateModel::onEvent(Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_PUB_PUBLISH)
    {
        return Account::ERROR_UNSUPPORTED_REQUEST;
    }

    if (param->size != sizeof(HAL::SportStatus_Info_t))
    {
        return -1;
    }

    DialplateModel* instance = (DialplateModel*)param->recv->UserData;
    memcpy(&(instance->sportStatusInfo), param->data_p, param->size);

    return 0;
}

void DialplateModel::RecorderCtrl(RecCtrl_t ctrl)
{
    DataProc::Recorder_Info_t info;
    info.state = (DataProc::Recorder_State_t)ctrl;
    info.time = 1000;
    account->Notify("Recorder", &info, sizeof(info));
}
