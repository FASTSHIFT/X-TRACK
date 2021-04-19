#include "DataProc.h"
#include "../HAL/HAL.h"

static DataCenter dataCenter("Bilibili");

DataCenter* DataProc::Center()
{
    return &dataCenter;
}

void DataProc::Init()
{
    Clock_Init(&dataCenter);
    GPS_Init(&dataCenter);
    Power_Init(&dataCenter);
    SportStatus_Init(&dataCenter);
}
