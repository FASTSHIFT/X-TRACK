#ifndef __DATA_PROC_H
#define __DATA_PROC_H

#include "Utils/DataCenter/DataCenter.h"
#include "../HAL/HAL_Def.h"

namespace DataProc
{
    
DataCenter* Center();
void Init();
uint32_t GetTick();
uint32_t GetTickElaps(uint32_t prevTick);
const char* ConvTime(uint64_t ms, char* buf, uint16_t len);

Account* Clock_Init(DataCenter* center);
Account* GPS_Init(DataCenter* center);
Account* Power_Init(DataCenter* center);
Account* SportStatus_Init(DataCenter* center);
}

#endif
