#include "DataProc.h"
#include "../HAL/HAL.h"
#include "Utils/SunRiseCalc/SunRiseCalc.h"
#include "Arduino.h"
#include "HAL/HAL_Config.h"

using namespace DataProc;

#define MINUTE_OF_DAY(hour, minute) (hour * 60 + minute)

// check Config
#if CONFIG_BACKLIGHT_CTRL_RANGE < 1 || CONFIG_BACKLIGHT_CTRL_RANGE > 120
#error invalid CONFIG_BACKLIGHT_CTRL_RANGE 
#endif
#if CONFIG_BACKLIGHT_MIN < 0 || CONFIG_BACKLIGHT_MIN > 1000
#error invalid CONFIG_BACKLIGHT_MIN 
#endif
#if CONFIG_BACKLIGHT_MAX < 0 || CONFIG_BACKLIGHT_MAX > 1000
#error invalid CONFIG_BACKLIGHT_MAX 
#endif

#define LIGHT_CTRL_MINUTE_RANGE  CONFIG_BACKLIGHT_CTRL_RANGE
#define MIN_LIGHT_LEVEL          CONFIG_BACKLIGHT_MIN
#define MAX_LIGHT_LEVEL          CONFIG_BACKLIGHT_MAX

static bool alreadyGetSysConfig = false;
static HAL::Clock_Info_t clockInfo;
static HAL::GPS_Info_t gpsInfo;
static SysConfig_Info_t sysConfig;
static uint8_t sunriseHour = 0, sunriseMinute = 0, sunsetHour = 0, sunsetMinute = 0;
static uint8_t dateChecksum = 0;
static bool calculatedSunriseTime = false;

/**
 * | ------------------------------------------------------------------------------------ |
 * | SrceenLight                 _________________________                     .          |
 * |       .                   _C .                     . D_                   .          |
 * |       .                 _/   .                     .   \_                 .          |
 * |       .               _/     .                     .     \_               .          |
 * |   N   .             _/       .         DAY         .       \_             . N        |
 * |   I   .           _/         .                     .         \_           . I        |
 * |   G   .         _B           .                     .           E_         . G        |
 * |   H   .       _/             .                     .             \_       . H        |
 * |   T   .     _/               .                     .               \_     . T        |
 * |       .   _/                 .                     .                 \_   .          |
 * |       . _/                   .                     .                   \_ .          |
 * | _______A                     .                     .                     F__________ |
 * | 00:00------------------------------- Time -------------------------------------23:59 |
 * 
 * A: (SunriseTime - CONFIG_BACKLIGHT_CTRL_RANGE, CONFIG_BACKLIGHT_MIN)
 * B: (SunriseTime, (CONFIG_BACKLIGHT_MIN + CONFIG_BACKLIGHT_MAX) / 2)
 * C: (SunriseTime + CONFIG_BACKLIGHT_CTRL_RANGE, CONFIG_BACKLIGHT_MAX)
 * D: (SunsetTime - CONFIG_BACKLIGHT_CTRL_RANGE, CONFIG_BACKLIGHT_MAX)
 * E: (SunsetTime, (CONFIG_BACKLIGHT_MIN + CONFIG_BACKLIGHT_MAX) / 2)
 * F: (SunsetTime + CONFIG_BACKLIGHT_CTRL_RANGE, CONFIG_BACKLIGHT_MIN)
 */
static int16_t calcBacklightLevel(uint8_t hour, uint8_t minute){
    int16_t nowMinutes = MINUTE_OF_DAY(hour, minute);
    int16_t sunriseMinutes = MINUTE_OF_DAY(sunriseHour, sunriseMinute);
    int16_t sunsetMinutes = MINUTE_OF_DAY(sunsetHour, sunsetMinute);
    uint16_t min = 0, max = 0;
    // min 
    if (nowMinutes <= sunriseMinutes - CONFIG_BACKLIGHT_CTRL_RANGE || sunsetMinutes + CONFIG_BACKLIGHT_CTRL_RANGE <= nowMinutes){
        return MIN_LIGHT_LEVEL;
    }
    // max
    if (sunriseMinutes + CONFIG_BACKLIGHT_CTRL_RANGE <= nowMinutes && nowMinutes <= sunsetMinutes - CONFIG_BACKLIGHT_CTRL_RANGE){
        return MAX_LIGHT_LEVEL;
    }
    if (sunriseMinutes - CONFIG_BACKLIGHT_CTRL_RANGE <= nowMinutes && nowMinutes <= sunriseMinutes + CONFIG_BACKLIGHT_CTRL_RANGE){
        min = sunriseMinutes - CONFIG_BACKLIGHT_CTRL_RANGE;
        max = sunriseMinutes + CONFIG_BACKLIGHT_CTRL_RANGE;
    }else{
        min = sunsetMinutes + CONFIG_BACKLIGHT_CTRL_RANGE;
        max = sunsetMinutes - CONFIG_BACKLIGHT_CTRL_RANGE;
    }
    return map(nowMinutes, min, max, MIN_LIGHT_LEVEL, MAX_LIGHT_LEVEL);
}

static int onEvent(Account* account, Account::EventParam_t* param)
{
    if (param->event != Account::EVENT_TIMER)
    {
        return Account::RES_UNSUPPORTED_REQUEST;
    }
    if (!alreadyGetSysConfig){
        account->Pull("SysConfig", &sysConfig, sizeof(sysConfig)); // 获取系统配置
        account->Unsubscribe("SysConfig");
        alreadyGetSysConfig = true;
    }
    account->Pull("GPS", &gpsInfo, sizeof(gpsInfo)); // 获取GPS信息
    HAL::Clock_GetInfo(&clockInfo); // 获取RTC时间
    uint8_t tempChceksum = 0xFF - (gpsInfo.isVaild + clockInfo.month + clockInfo.day) & 0xFF;
    if (!calculatedSunriseTime || dateChecksum != tempChceksum){
        if (gpsInfo.isVaild){
            SunRiseSunSetCalculator(
                sysConfig.timeZone,
                clockInfo.year,
                clockInfo.month,
                clockInfo.day,
                gpsInfo.longitude,
                gpsInfo.latitude,
                &sunriseHour,
                &sunriseMinute,
                &sunsetHour,
                &sunsetMinute
            );
        }else{
            SunRiseSunSetCalculator(
                sysConfig.timeZone,
                clockInfo.year,
                clockInfo.month,
                clockInfo.day,
                sysConfig.longitude,
                sysConfig.latitude,
                &sunriseHour,
                &sunriseMinute,
                &sunsetHour,
                &sunsetMinute
            );
        }
        dateChecksum = tempChceksum;
        calculatedSunriseTime = true;
    }
    HAL::Backlight_SetGradual(calcBacklightLevel(clockInfo.hour, clockInfo.minute), 500);
    // Serial.printf(
    //     "[backlight ctrl] level: %d, sunrise: %d:%d, sunset: %d:%d\r\n",
    //     calcBacklightLevel(clockInfo.hour, clockInfo.minute),
    //     sunriseHour,
    //     sunriseMinute,
    //     sunsetHour,
    //     sunsetMinute
    // );
    return 0;
}



DATA_PROC_INIT_DEF(BacklightCtrl)
{
    account->Subscribe("SysConfig");
    account->Subscribe("GPS");
    account->SetEventCallback(onEvent);
    account->SetTimerPeriod(5 * 1000);
}
