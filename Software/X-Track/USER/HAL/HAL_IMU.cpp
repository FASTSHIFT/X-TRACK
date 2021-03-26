#include "HAL.h"
#include "LSM6DSM/LSM6DSM.h"

static LSM6DSM imu;

void HAL::IMU_Init()
{
    Serial.print("IMU init...");
    Serial.println(imu.Init() ? "success" : "failed");
}

void HAL::IMU_Update()
{
    int16_t ax, ay, az, gx, gy, gz;
    imu.GetMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    //Serial.printf("imu: step = %d ", Steps);
    //Serial.printf("ax = %d, ay = %d, az = %d, gx = %d, gy = %d, gz = %d\r\n", ax, ay, az, gx, gy, gz);
}

int16_t HAL::IMU_GetSteps()
{
    return imu.GetCurrentStep();
}
