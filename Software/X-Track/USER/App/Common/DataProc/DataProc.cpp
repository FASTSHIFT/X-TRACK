#include "DataProc.h"
#include "../HAL/HAL.h"

static DataCenter dataCenter("Bilibili");

#define DP_REG(NodeName) \
do{\
    extern void DP_##NodeName##_Register(DataCenter* center);\
    DP_##NodeName##_Register(&dataCenter);\
}while(0)

DataCenter* DataProc::Center()
{
    return &dataCenter;
}

void DataProc_Init()
{
    DP_REG(Clock);
    DP_REG(GPS);
    DP_REG(Power);
    DP_REG(SportStatus);
    DP_REG(Recorder);
    DP_REG(IMU);
    DP_REG(MAG);
}
