/*
 * Copyright (c) 2021 HanfG
 */
#ifndef __SUN_RISE_CALC_H__
#define __SUN_RISE_CALC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculate the sunrise and sunset time
 * @param timeZone Time zone
 * @param year Year
 * @param month Month
 * @param day Day
 * @param longitude Longitude
 * @param latitude Latitude
 * @param sunriseHour Sunrise hour
 * @param sunriseMinute Sunrise minute
 * @param sunsetHour Sunset hour
 * @param sunsetMinute Sunset minute
 */
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
    uint8_t* sunsetMinute);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SUN_RISE_CALC_H__ */
