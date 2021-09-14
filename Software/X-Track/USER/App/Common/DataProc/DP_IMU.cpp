#include "DataProc.h"
#include "../HAL/HAL.h"

DATA_PROC_INIT_DEF(IMU)
{
    HAL::IMU_SetCommitCallback([](void* info, void* userData){
        Account* account = (Account*)userData;
        return account->Commit(info, sizeof(HAL::IMU_Info_t));
    }, account);
}
