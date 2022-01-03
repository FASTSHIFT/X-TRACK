#include "HAL.h"
#if (CONFIG_IMU_MODEL_LSM6DSM + CONFIG_IMU_MODEL_MPU6050) != 1
#error Specificated too many IMU Sensor Model
#endif


#if CONFIG_IMU_MODEL_LSM6DSM
#include "LSM6DSM/LSM6DSM.h"
static LSM6DSM imu;
#elif CONFIG_IMU_MODEL_MPU6050
#include "MPU6050/MPU6050.h"
static MPU6050 imu;
#else
#error Unspecificate IMU Sensor Model
#endif


static HAL::CommitFunc_t CommitFunc = nullptr;
static void* UserData = nullptr;

bool HAL::IMU_Init()
{
    Serial.print("IMU: init...");
    bool success = false;
#if CONFIG_IMU_MODEL_LSM6DSM
    success = imu.Init();
#elif CONFIG_IMU_MODEL_MPU6050
    imu.initialize();
    success = imu.testConnection();
#endif
    Serial.println(success ? "success" : "failed");

    return success;
}

void HAL::IMU_SetCommitCallback(CommitFunc_t func, void* userData)
{
    CommitFunc = func;
    UserData = userData;
}

void HAL::IMU_Update()
{
    IMU_Info_t imuInfo;
#if CONFIG_IMU_MODEL_LSM6DSM
    imu.GetMotion6(
        &imuInfo.ax, &imuInfo.ay, &imuInfo.az,
        &imuInfo.gx, &imuInfo.gy, &imuInfo.gz
    );
    imuInfo.steps = imu.GetCurrentStep();
#elif CONFIG_IMU_MODEL_MPU6050
    imu.getMotion6(
        &imuInfo.ax, &imuInfo.ay, &imuInfo.az,
        &imuInfo.gx, &imuInfo.gy, &imuInfo.gz
    );
    imuInfo.steps = 0; // mpu6050 unsupport count step
#endif
//    Serial.printf(
//        "ax = %d, ay = %d, az = %d, gx = %d, gy = %d, gz = %d\r\n",
//        imuInfo.ax, imuInfo.ay, imuInfo.az, imuInfo.gx, imuInfo.gy, imuInfo.gz
//    );

    if(CommitFunc)
    {
        CommitFunc(&imuInfo, UserData);
    }
}
