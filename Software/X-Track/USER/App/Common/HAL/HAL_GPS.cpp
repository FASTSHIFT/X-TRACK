#include "HAL.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lvgl/lvgl.h"

typedef struct {
    char time[32];
    float longitude;
    float latitude;
    float altitude;
}TrackPoint_t;

static lv_fs_file_t file;
static TrackPoint_t point = {
    .longitude = 116.391332,
    .latitude = 39.907415,
    .altitude = 53.0f
};

static bool ReadFileUntil(lv_fs_file_t* f, char c, char* buf, uint32_t len)
{
    bool retval = false;
    uint32_t index = 0;
    while (1)
    {
        if (index >= len)
        {
            break;
        }

        char data;
        lv_fs_res_t res = lv_fs_read(f, &data, 1, nullptr);

        if (res != LV_FS_RES_OK)
        {
            buf[index] = '\0';
            break;
        }

        if (data == c)
        {
            buf[index] = '\0';
            retval = true;
            break;
        }

        buf[index] = data;

        index++;
    }
    return retval;
}

static bool TrackGetNext(lv_fs_file_t* f, TrackPoint_t* point)
{
    char buf[64];
    bool ret = ReadFileUntil(f, '\n', buf, sizeof(buf));

    if (ret)
    {
        sscanf(
            buf, "%[^,],%f,%f,%f",
            point->time,
            &point->longitude,
            &point->latitude,
            &point->altitude
        );
    }

    return ret;
}

static void TrackUpdate(lv_timer_t* timer)
{
    bool ret = TrackGetNext(&file, &point);
    if(!ret)
    {
        lv_fs_seek(&file, 0, LV_FS_SEEK_SET);
    }
}

bool HAL::GPS_GetInfo(GPS_Info_t* info)
{
    static int angle;
    memset(info, 0, sizeof(GPS_Info_t));

    info->isVaild = true;
    info->satellites = 10;

    static bool fileIsOpen = false;

    if (!fileIsOpen)
    {
       lv_fs_res_t res = lv_fs_open(&file, "/Track.csv", LV_FS_MODE_RD);
       fileIsOpen = true;

       if (res == LV_FS_RES_OK)
       {
           lv_timer_create(TrackUpdate, 1000, nullptr);
       }
    }

    info->longitude = point.longitude;
    info->latitude = point.latitude;
    info->altitude = point.altitude;

    /*info->longitude = 116.391332;
    info->latitude = 39.907415;
    info->altitude = 53.0f;*/

    Clock_GetInfo(&info->clock);

    info->speed = (rand() % 600) / 10.0f;

    return true;
}

void HAL::GPS_Update()
{

}

double HAL::GPS_GetDistanceOffset(GPS_Info_t* info, double preLong, double preLat)
{
    return 10;
}
