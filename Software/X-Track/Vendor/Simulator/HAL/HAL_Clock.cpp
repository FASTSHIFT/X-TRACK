#include "HAL.h"
#include "rtc.h"

void HAL::Clock_Init()
{
    RTC_Init();

    Clock_Info_t info;
    Clock_GetInfo(&info);
    Serial.printf(
        "Clock: %04d-%02d-%02d %s %02d:%02d:%02d\r\n",
        info.year,
        info.month,
        info.day,
        Clock_GetWeekString(info.week),
        info.hour,
        info.minute,
        info.second
    );
}

void HAL::Clock_GetInfo(Clock_Info_t* info)
{
    RTC_Calendar_TypeDef calendar;
    RTC_GetCalendar(&calendar);
    info->year = calendar.year;
    info->month = calendar.month;
    info->day = calendar.day;
    info->week = calendar.week;
    info->hour = calendar.hour;
    info->minute = calendar.min;
    info->second = calendar.sec;
    info->millisecond = 0;
}

void HAL::Clock_SetInfo(const Clock_Info_t* info)
{
    RTC_SetTime(
        info->year,
        info->month,
        info->day,
        info->hour,
        info->minute,
        info->second
    );
}

const char* HAL::Clock_GetWeekString(uint8_t week)
{
    const char* week_str[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
    return week < 7 ? week_str[week] : "";
}
