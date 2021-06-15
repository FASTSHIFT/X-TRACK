#include "DataProc.h"
#include "../HAL/HAL.h"

static Account* actIMU;

void DataProc::IMU_Commit(HAL::IMU_Info_t* info)
{
    actIMU->Commit(info, sizeof(HAL::IMU_Info_t));
}

DATA_PROC_INIT_DEF(IMU)
{
    actIMU = account;
}
