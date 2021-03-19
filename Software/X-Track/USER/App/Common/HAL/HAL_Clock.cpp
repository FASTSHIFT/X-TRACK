#include "HAL.h"
#include <time.h>

void HAL::Clock_GetValue(Clock_Value_t* clock_value)
{
    struct tm* t;
    time_t tt;
    time(&tt);
    t = localtime(&tt);

    clock_value->year = t->tm_year + 1900;
    clock_value->month = t->tm_mon + 1;
    clock_value->date = t->tm_mday;
    clock_value->week = t->tm_wday;
    clock_value->hour = t->tm_hour;
    clock_value->min = t->tm_min;
    clock_value->sec = t->tm_sec;
    clock_value->ms = t->tm_sec;
}

void HAL::Clock_SetValue(const Clock_Value_t* clock_value)
{

}
