/*
 * Copyright (c) 2021 HanfG
 */
#include "SunRiseCalc.h"
#include <math.h>
#include <stdbool.h>

#define PI 3.1415926f
#define ACOS acosf
#define COS cosf
#define TAN tanf

static bool isLeapYear(uint16_t year)
{
    if (year % 4 != 0)
        return false;
    if (year % 100 != 0)
        return true;
    if (year % 400 == 0)
        return true;
    return false;
}

static uint16_t dayOfYear(uint16_t year, uint8_t month, uint8_t day)
{
    uint16_t days = 0;
    if (month > 2 && isLeapYear(year)) {
        days += 1;
    }
    for (uint8_t i = 0; i < month; i++) {
        static const uint8_t daysOfMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 };
        days += daysOfMonth[i];
    }
    return days;
}

void SunRiseSunSetCalculator(
    int8_t timeZone,
    uint16_t year,
    uint8_t month,
    uint8_t day,
    float longitude,
    float latitude,
    uint8_t* sunriseHour,
    uint8_t* sunriseMinute,
    uint8_t* sunsetHour,
    uint8_t* sunsetMinute)
{
    float sunriseFloat = 24 * (180 + timeZone * 15 - longitude - ACOS(-TAN(-23.4f * COS(2 * PI * (dayOfYear(year, month, day) + 9) / 365) * PI / 180) * TAN(latitude * PI / 180)) * 180 / PI) / 360;
    float sunsetFloat = 24 * (1 + (timeZone * 15 - longitude) / 180) - sunriseFloat;

    *sunriseHour = (uint8_t)sunriseFloat;
    *sunriseMinute = (uint8_t)((sunriseFloat - *sunriseHour) * 60);
    *sunsetHour = (uint8_t)sunsetFloat;
    *sunsetMinute = (uint8_t)((sunsetFloat - *sunsetHour) * 60);
}
