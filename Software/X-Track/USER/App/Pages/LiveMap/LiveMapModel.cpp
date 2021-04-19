#include "LiveMapModel.h"

void LiveMapModel::Init()
{
    account = new Account("LiveMapModel", DataProc::Center(), this);
    account->Subscribe("GPS", nullptr);
    account->Subscribe("SportStatus", Callback);
}

void LiveMapModel::Deinit()
{
    delete account;
}

int LiveMapModel::Callback(
    Account* pub,
    Account* sub,
    int msgType,
    void* data_p,
    uint32_t size
)
{
    LiveMapModel* instance = (LiveMapModel*)sub->UserData;

    if (size != sizeof(HAL::SportStatus_Info_t))
    {
        return -1;
    }
    memcpy(&(instance->sportStatusInfo), data_p, size);


    return 0;
}
