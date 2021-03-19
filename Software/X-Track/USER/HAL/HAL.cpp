#include "HAL.h"

void HAL::HAL_Init()
{
    HAL::Power_Init();
    HAL::Clock_Init();
    HAL::Buzz_init();
    HAL::GPS_Init();
    HAL::I2C_Scan(true);
    HAL::IMU_Init();
    HAL::MAG_Init();
}
