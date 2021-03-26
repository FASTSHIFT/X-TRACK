#include "HAL.h"
#include "Display/Display.h"

/*上一次操作时间(ms)*/
static uint32_t Power_LastHandleTime = 0;

/*自动关机时间(秒)*/
static uint16_t Power_AutoLowPowerTimeout = 60;

/*自动关机功能使能*/
static bool Power_AutoLowPowerEnable = false;

static uint16_t Power_ADCValue = 0;

#define BATT_MAX_VOLTAGE 4100
#define BATT_MIN_VOLTAGE 3300

/**
  * @brief  电源初始化
  * @param  无
  * @retval 无
  */
void HAL::Power_Init()
{
    /*电源使能保持*/
    pinMode(POWER_EN_PIN, OUTPUT);
    delay(1000);
    digitalWrite(POWER_EN_PIN, HIGH);
    
    /*电池检测*/
    pinMode(BAT_DET_PIN, INPUT_ANALOG_DMA);
    pinMode(BAT_CHG_DET_PIN, INPUT_PULLUP);
    
//    Power_SetAutoLowPowerTimeout(5 * 60);
//    Power_HandleTimeUpdate();
    Power_SetAutoLowPowerEnable(false);
}

/**
  * @brief  更新操作时间
  * @param  无
  * @retval 无
  */
void HAL::Power_HandleTimeUpdate()
{
    Power_LastHandleTime = millis();
}

/**
  * @brief  设置自动关机时间
  * @param  sec:时间(秒)
  * @retval 无
  */
void HAL::Power_SetAutoLowPowerTimeout(uint16_t sec)
{
    Power_AutoLowPowerTimeout = sec;
}

/**
  * @brief  获取自动关机时间
  * @param  无
  * @retval sec:时间(秒)
  */
uint16_t HAL::Power_GetAutoLowPowerTimeout()
{
    return Power_AutoLowPowerTimeout;
}

/**
  * @brief  设置自动关机功能使能
  * @param  en:使能
  * @retval 无
  */
void HAL::Power_SetAutoLowPowerEnable(bool en)
{
    Power_AutoLowPowerEnable = en;
    Power_HandleTimeUpdate();
}

/**
  * @brief  执行关机
  * @param  无
  * @retval 无
  */
void HAL::Power_Shutdown()
{
    Backlight_SetGradual(0, 500);
    digitalWrite(POWER_EN_PIN, LOW);
}

/**
  * @brief  自动关机监控
  * @param  无
  * @retval 无
  */
void HAL::Power_Update()
{
    Power_ADCValue = analogRead_DMA(BAT_DET_PIN);
    
    if(!Power_AutoLowPowerEnable)
        return;
    
    if(Power_AutoLowPowerTimeout == 0)
        return;
    
    if(millis() - Power_LastHandleTime >= (Power_AutoLowPowerTimeout * 1000))
    {
        Power_Shutdown();
    }
}

uint8_t HAL::Power_GetBattUsage()
{
    int voltage = map(
        Power_ADCValue,
        0, 4095,
        0, 3300
    );
    
    voltage *= 2;
    
    __LimitValue(voltage, BATT_MIN_VOLTAGE, BATT_MAX_VOLTAGE);
    
    int usage = map(
        voltage,
        BATT_MIN_VOLTAGE, BATT_MAX_VOLTAGE,
        0, 100
    );
    
    return usage;
}

bool HAL::Power_GetBattIsCharging()
{
    return !digitalRead(BAT_CHG_DET_PIN);
}
