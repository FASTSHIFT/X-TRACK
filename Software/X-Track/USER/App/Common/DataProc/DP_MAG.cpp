#include "DataProc.h"
#include "../HAL/HAL.h"

static Account* actMAG;

void DataProc::MAG_Commit(HAL::MAG_Info_t* info)
{
    actMAG->Commit(info, sizeof(HAL::MAG_Info_t));
}

DATA_PROC_INIT_DEF(MAG)
{
    actMAG = account;
}
