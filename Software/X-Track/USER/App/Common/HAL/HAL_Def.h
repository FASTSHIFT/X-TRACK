#ifndef __HAL_DEF_H
#define __HAL_DEF_H

#include <stdint.h>

namespace HAL {
    
/*Clock*/
typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t ms;
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  week;
} Clock_Info_t;

/*GPS*/
typedef struct {
    double longitude;
    double latitude;
    float altitude;
    float compass;
    float speed;
    int16_t satellites;
    Clock_Info_t clock;
}GPS_Info_t;

/*AHRS*/
typedef struct{
    float pitch;
    float row;
    float yaw;
}AHRS_Info_t;

/*SportStatus*/
typedef struct {
    float speedKph;
    float speedMaxKph;
    float speedAvgKph;
    uint64_t totalTime;
    float totalDistance;
    uint64_t singleTime;
    float singleDistance;
    float singleCalorie;
    uint32_t lastTick;
}SportStatus_Info_t;

/*Power*/
typedef struct {
    int voltage;
    uint8_t usage;
    bool isCharging;
}Power_Info_t;

}

#endif
