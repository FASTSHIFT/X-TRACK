#include "HAL.h"

static void HAL_InterrputUpdate()
{
    HAL::Power_Update();
    HAL::Encoder_Update();
    HAL::Audio_Update();
}

void HAL::HAL_Init()
{
    Serial.begin(115200);
    Power_Init();
    Backlight_Init();
    Encoder_Init();
    Clock_Init();
    Buzz_init();
    GPS_Init();
    I2C_Scan(true);
    IMU_Init();
    MAG_Init();
    Audio_Init();
    SD_Init();

    Timer_SetInterrupt(CONFIG_HAL_UPDATE_TIM, 10 * 1000, HAL_InterrputUpdate);
    TIM_Cmd(CONFIG_HAL_UPDATE_TIM, ENABLE);
}

static void HAL_SensorUpdate()
{
    HAL::IMU_Update();
    HAL::MAG_Update();
}

void HAL::HAL_Update()
{
    __IntervalExecute(SD_Update(), 500);
    __IntervalExecute(HAL_SensorUpdate(), 1000);
    __IntervalExecute(GPS_Update(), 200);
    Power_EventMonitor();
}
