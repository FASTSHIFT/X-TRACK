#include "HAL.h"
#include "App/Version.h"

static void HAL_InterrputUpdate()
{
    HAL::Power_Update();
    HAL::Encoder_Update();
    HAL::Audio_Update();
}

#if CONFIG_SENSOR_ENABLE
static void HAL_Sensor_Init()
{
    HAL::I2C_Scan(true);

#if CONFIG_SENSOR_IMU_ENABLE
    HAL::IMU_Init();
#endif

#if CONFIG_SENSOR_MAG_ENABLE
    HAL::MAG_Init();
#endif
}
#endif

void HAL::HAL_Init()
{
    Serial.begin(115200);
    Serial.println(VERSION_FIRMWARE_NAME);
    Serial.println("Version: " VERSION_SOFTWARE);
    Serial.println("Author: " VERSION_AUTHOR_NAME);
    Serial.println("Project: " VERSION_PROJECT_LINK);
    
    FaultHandle_Init();
    
    Memory_DumpInfo();

    Power_Init();
    Backlight_Init();
    Encoder_Init();
    Clock_Init();
    Buzz_init();
    GPS_Init();
#if CONFIG_SENSOR_ENABLE
    HAL_Sensor_Init();
#endif
    Audio_Init();
    SD_Init();

    Display_Init();

    Timer_SetInterrupt(CONFIG_HAL_UPDATE_TIM, 10 * 1000, HAL_InterrputUpdate);
    TIM_Cmd(CONFIG_HAL_UPDATE_TIM, ENABLE);
}

#if CONFIG_SENSOR_ENABLE
static void HAL_SensorUpdate()
{
#if CONFIG_SENSOR_IMU_ENABLE
    HAL::IMU_Update();
#endif

#if CONFIG_SENSOR_MAG_ENABLE
    HAL::MAG_Update();
#endif
}
#endif

void HAL::HAL_Update()
{
    __IntervalExecute(SD_Update(), 500);
#if CONFIG_SENSOR_ENABLE
    __IntervalExecute(HAL_SensorUpdate(), 1000);
#endif
    __IntervalExecute(GPS_Update(), 200);
    __IntervalExecute(Memory_DumpInfo(), 1000);
    Power_EventMonitor();
}
