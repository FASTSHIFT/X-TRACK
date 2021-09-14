#include "HAL.h"
#include "LSM6DSM/LSM6DSM.h"

static LSM6DSM imu;
static HAL::CommitFunc_t CommitFunc;
static void* UserData;

void HAL::IMU_Init()
{
    Serial.print("IMU: init...");
    Serial.println(imu.Init() ? "success" : "failed");
}

void HAL::IMU_SetCommitCallback(CommitFunc_t func, void* userData)
{
    CommitFunc = func;
    UserData = userData;
}

void HAL::IMU_Update()
{
    IMU_Info_t imuInfo;
    imu.GetMotion6(
        &imuInfo.ax, &imuInfo.ay, &imuInfo.az,
        &imuInfo.gx, &imuInfo.gy, &imuInfo.gz
    );
//    Serial.printf(
//        "ax = %d, ay = %d, az = %d, gx = %d, gy = %d, gz = %d\r\n",
//        imuInfo.ax, imuInfo.ay, imuInfo.az, imuInfo.gx, imuInfo.gy, imuInfo.gz
//    );

    imuInfo.steps = imu.GetCurrentStep();
    
    if(CommitFunc)
    {
        CommitFunc(&imuInfo, UserData);
    }
}
