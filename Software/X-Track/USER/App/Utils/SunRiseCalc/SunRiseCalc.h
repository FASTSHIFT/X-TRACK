#ifndef __SUN_RISE_CALC_H__
#define __SUN_RISE_CALC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
);

#ifdef __cplusplus
}// extern "C"
#endif

#endif
