#include "DataProc.h"
#include "../HAL/HAL.h"

static Account* account;

void DP_IMU_Commit(const void* data, uint32_t size)
{
    account->Commit(data, size);
}

void DP_IMU_Register(DataCenter* center)
{
    account = new Account("IMU", DataProc::Center(), sizeof(HAL::IMU_Info_t));
}
