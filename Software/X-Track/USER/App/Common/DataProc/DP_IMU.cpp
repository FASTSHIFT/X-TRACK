#include "DataProc.h"
#include "../HAL/HAL.h"

static Account* account;

void DataProc::IMU_Commit(HAL::IMU_Info_t* info)
{
    account->Commit(info, sizeof(HAL::IMU_Info_t));
}

void DP_IMU_Init(Account* act)
{
    account = act;
}
