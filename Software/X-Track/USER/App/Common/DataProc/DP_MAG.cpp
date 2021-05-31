#include "DataProc.h"
#include "../HAL/HAL.h"

static Account* account;

void DataProc::MAG_Commit(HAL::MAG_Info_t* info)
{
    account->Commit(info, sizeof(HAL::MAG_Info_t));
}

void DP_MAG_Init(Account* act)
{
    account = act;
}
