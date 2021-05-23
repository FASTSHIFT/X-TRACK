#include "DataProc.h"
#include "../HAL/HAL.h"

static Account* account;

void DP_MAG_Commit(const void* data, uint32_t size)
{
    account->Commit(data, size);
}

void DP_MAG_Register(DataCenter* center)
{
    account = new Account("MAG", DataProc::Center(), sizeof(HAL::MAG_Info_t));
}
