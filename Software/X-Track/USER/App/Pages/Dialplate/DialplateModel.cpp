#include "DialplateModel.h"

void DialplateModel::Init()
{
    account = new Account("DialplateModel", DataProc::Center(), this);
    account->Subscribe("SportStatus", Callback);
}

void DialplateModel::Deinit()
{
    delete account;
}

int DialplateModel::Callback(
    Account* pub,
    Account* sub,
    int msgType,
    void* data_p,
    uint32_t size
)
{
    if (size != sizeof(HAL::SportStatus_Info_t))
    {
        return -1;
    }

    DialplateModel* instance = (DialplateModel*)sub->UserData;
    memcpy(&(instance->sportStatusInfo), data_p, size);

    return 0;
}
