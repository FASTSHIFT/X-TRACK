#include "HAL.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lvgl/lvgl.h"
#include "Utils/GPX_Parser/GPX_Parser.h"
#include "Config/Config.h"

typedef struct
{
    lv_fs_file_t file;
    uint32_t size;
}FileInfo_t;

static int Parser_FileReadByte(GPX_Parser* parser)
{
    FileInfo_t* info = (FileInfo_t*)parser->userData;
    uint8_t data = 0;
    lv_fs_read(&info->file, &data, 1, nullptr);
    return data;
}

static int Parser_FileAvaliable(GPX_Parser* parser)
{
    FileInfo_t* info = (FileInfo_t*)parser->userData;
    uint32_t cur = 0;
    lv_fs_tell(&info->file, &cur);
    return (info->size - cur);
}

static bool Parser_Init(GPX_Parser* parser, FileInfo_t* info)
{
    bool retval = false;
    lv_fs_res_t res = lv_fs_open(&info->file, CONFIG_TRACK_VIRTUAL_GPX_FILE_PATH, LV_FS_MODE_RD);

    if (res == LV_FS_RES_OK)
    {
        uint32_t cur = 0;
        lv_fs_tell(&info->file, &cur);

        lv_fs_seek(&info->file, 0L, LV_FS_SEEK_END);

        lv_fs_tell(&info->file, &info->size);

        /*Restore file pointer*/
        lv_fs_seek(&info->file, 0L, LV_FS_SEEK_SET);

        parser->SetCallback(Parser_FileAvaliable, Parser_FileReadByte);

        parser->userData = info;

        retval = true;
    }
    return retval;
}

bool HAL::GPS_GetInfo(GPS_Info_t* info)
{
    static bool isInit = false;
    static bool isFileOpen = false;
    static GPX_Parser gpxParser;
    static FileInfo_t fileInfo;

    memset(info, 0, sizeof(GPS_Info_t));

    info->isVaild = true;
    info->satellites = 10;

    if (!isInit)
    {
        isFileOpen = Parser_Init(&gpxParser, &fileInfo);
        isInit = true;
    }

    if (isFileOpen)
    {
        GPX_Parser::Point_t point;
        if (gpxParser.ReadNext(&point))
        {
            info->longitude = point.longitude;
            info->latitude = point.latitude;
        }
        else
        {
            lv_fs_seek(&fileInfo.file, 0, LV_FS_SEEK_SET);
        }  
    }
    else
    {
        info->longitude = CONFIG_LIVE_MAP_LNG_DEFAULT;
        info->latitude = CONFIG_LIVE_MAP_LAT_DEFAULT;
    }

    Clock_GetInfo(&info->clock);

    info->speed = (rand() % 600) / 10.0f;
    info->altitude = (rand() % 1000) / 10.0f;

    return true;
}

void HAL::GPS_Update()
{

}

double HAL::GPS_GetDistanceOffset(GPS_Info_t* info, double preLong, double preLat)
{
    return 10;
}
