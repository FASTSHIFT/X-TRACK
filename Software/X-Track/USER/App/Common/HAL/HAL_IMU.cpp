#include "HAL.h"
#include <stdlib.h>

extern void DP_IMU_Commit(const void* data, uint32_t size);

void HAL::IMU_Update()
{
    static int16_t steps;
    steps++;
    if (steps > 9999)
    {
        steps = 0;
    }

    IMU_Info_t imu;
    imu.steps = steps;

    imu.ax = rand() % 1000 - 500;
    imu.ay = rand() % 1000 - 500;
    imu.az = rand() % 1000 - 500;
    imu.gx = rand() % 1000 - 500;
    imu.gy = rand() % 1000 - 500;
    imu.gz = rand() % 1000 - 500;

    DP_IMU_Commit(&imu, sizeof(imu));
}
