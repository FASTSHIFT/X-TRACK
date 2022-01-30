#include "HAL.h"
#include "App/Version.h"
#include "MillisTaskManager/MillisTaskManager.h"

static MillisTaskManager taskManager;

#if CONFIG_SENSOR_ENABLE

static void HAL_Sensor_Init()
{
    if(HAL::I2C_Scan() <= 0)
    {
        Serial.println("I2C: disable sensors");
        return;
    }

#if CONFIG_SENSOR_IMU_ENABLE
    if(HAL::IMU_Init())
    {
        taskManager.Register(HAL::IMU_Update, 1000);
    }
#endif

#if CONFIG_SENSOR_MAG_ENABLE
    if(HAL::MAG_Init())
    {
        taskManager.Register(HAL::MAG_Update, 1000);
    }
#endif
}

#endif

static void HAL_TimerInterrputUpdate()
{
    HAL::Power_Update();
    HAL::Encoder_Update();
    HAL::Audio_Update();
}

void HAL::HAL_Init()
{
    Serial.begin(115200);
    Serial.println(VERSION_FIRMWARE_NAME);
    Serial.println("Version: " VERSION_SOFTWARE);
    Serial.println("Author: "  VERSION_AUTHOR_NAME);
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

#if CONFIG_WATCH_DOG_ENABLE
    uint32_t timeout = WDG_Init(CONFIG_WATCH_DOG_TIMEOUT);
    taskManager.Register(WDG_ReloadCounter, CONFIG_WATCH_DOG_TIMEOUT / 10);
    Serial.printf("WatchDog: Timeout = %dms\r\n", timeout);
#endif

    taskManager.Register(Power_EventMonitor, 100);
    taskManager.Register(GPS_Update, 200);
    taskManager.Register(SD_Update, 500);
    taskManager.Register(Memory_DumpInfo, 1000);

    Timer_SetInterrupt(CONFIG_HAL_UPDATE_TIM, 10 * 1000, HAL_TimerInterrputUpdate);
    Timer_SetEnable(CONFIG_HAL_UPDATE_TIM, true);
}

void HAL::HAL_Update()
{
    taskManager.Running(millis());
}
