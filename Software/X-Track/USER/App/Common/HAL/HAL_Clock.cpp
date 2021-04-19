#include "HAL.h"
#include <time.h>

void HAL::Clock_GetInfo(Clock_Info_t* info)
{
    struct tm* t;
    time_t tt;
    time(&tt);
    t = localtime(&tt);

    info->year = t->tm_year + 1900;
    info->month = t->tm_mon + 1;
    info->day = t->tm_mday;
    info->week = t->tm_wday;
    info->hour = t->tm_hour;
    info->min = t->tm_min;
    info->sec = t->tm_sec;
    info->ms = t->tm_sec;
}

void HAL::Clock_SetInfo(const Clock_Info_t* info)
{

}
