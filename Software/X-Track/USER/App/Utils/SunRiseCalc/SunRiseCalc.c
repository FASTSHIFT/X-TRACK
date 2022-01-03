#include <math.h>

#include <stdlib.h>
#include "SunRiseCalc.h"

#define PI 3.1415926f
#define ACOS acos
#define COS  cos
#define TAN  tan

const static uint8_t daysOfMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30};

static uint8_t isLeapYear(uint16_t year){
    if (year % 4 != 0)
        return 0;
    if (year % 100 != 0)
        return 1;
    if (year % 400 == 0)
        return 1;
    return 0;
}

static uint16_t dayOfYear(uint16_t year, uint8_t month, uint8_t day){
    uint16_t days = 0;
    if (month > 2 && isLeapYear(year)){
        days += 1;
    }
    for (int i = 0; i < month; i ++){
        days += daysOfMonth[i];
    }
    return days;
	}


void SunRiseSunSetCalculator(
    int8_t timeZone,        // 时区
    uint16_t year,          // 年
    uint8_t month,          // 月
    uint8_t day,            // 日
    float longitude,        // 经度
    float latitude,         // 纬度
    uint8_t *sunriseHour,   // 日出时
    uint8_t *sunriseMinute, // 日出分
    uint8_t *sunsetHour,    // 日落时
    uint8_t *sunsetMinute   // 日落分
){
    float sunriseFloat = 24 * (
        180 + timeZone * 15 - longitude - ACOS(
            - TAN(-23.4f * COS(2 * PI * (dayOfYear(year, month, day) + 9) / 365) * PI / 180)
            * TAN(latitude * PI / 180)
        ) * 180 / PI
    ) / 360;
    float sunsetFloat = 24 * (1 + (timeZone * 15 - longitude) / 180) - sunriseFloat;

    *sunriseHour = (uint8_t)sunriseFloat;
    *sunriseMinute = (uint8_t)((sunriseFloat - *sunriseHour) * 60);
    *sunsetHour = (uint8_t)sunsetFloat;
    *sunsetMinute = (uint8_t)((sunsetFloat - *sunsetHour) * 60);
}
