#ifndef __HAL_H
#define __HAL_H

#include <stdint.h>

#ifdef ARDUINO
#   include "Arduino.h"
#   include "Basic/SysConfig.h"
#   include "Basic/CommonMacro.h"
#endif

namespace HAL {
    
void HAL_Init();

/*Backlight*/
void Backlight_Init();
uint16_t Backlight_GetValue();
void Backlight_SetValue(int16_t val);
void Backlight_SetGradual(uint16_t target, uint16_t time = 500);
void Backlight_UpdateBKP(void);
uint16_t Backlight_GetBKP(void);

/*I2C*/
void I2C_Scan(bool startScan);

/*IMU*/
void IMU_Init();
void IMU_Update();
int16_t IMU_GetSteps();
    
/*MAG*/
void MAG_Init();
void MAG_Update();

/*SD*/
void SD_Init();

/*Power*/
void Power_Init();
void Power_HandleTimeUpdate();
void Power_SetAutoLowPowerTimeout(uint16_t sec);
uint16_t Power_GetAutoLowPowerTimeout();
void Power_SetAutoLowPowerEnable(bool en);
void Power_Shutdown();
void Power_Update();
uint8_t Power_GetBattUsage();
bool Power_GetBattIsCharging();

/*Clock*/
typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t ms;
    uint16_t year;
    uint8_t  month;
    uint8_t  date;
    uint8_t  week;
} Clock_Value_t;
void Clock_Init();
void Clock_GetValue(Clock_Value_t* clock_value);
void Clock_SetValue(const Clock_Value_t* clock_value);

/*GPS*/
typedef struct {
    double longitude;
    double latitude;
    float altitude;
    float speed;
    int16_t satellites;
    Clock_Value_t clock;
}GPS_Info_t;
void GPS_Init();
void GPS_Update();
bool GPS_GetInfo(GPS_Info_t* info);

/*Buzzer*/
void Buzz_init();
void Buzz_SetEnable(bool en);
void Buzz_Tone(uint32_t freq, int32_t duration = -1);

}

#endif
