#include "HAL.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <cmath>
#include "lvgl/lvgl.h"
#include "Utils/GPX_Parser/GPX_Parser.h"
#include "Config/Config.h"

#define CONFIG_TRACK_VIRTUAL_GPX_FILE_PATH    "/TRK_EXAMPLE.gpx"

#define PI 3.1415926535897932384626433832795f
#define HALF_PI 1.5707963267948966192313216916398f
#define TWO_PI 6.283185307179586476925286766559f
#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f
#define EULER 2.718281828459045235360287471352f
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

typedef struct
{
    lv_fs_file_t file;
    uint32_t size;
}FileInfo_t;

static HAL::GPS_Info_t gpsInfo;
static GPX_Parser gpxParser;
static FileInfo_t fileInfo;

static double distanceBetween(double lat1, double long1, double lat2, double long2)
{
    // returns distance in meters between two positions, both specified
    // as signed decimal-degrees latitude and longitude. Uses great-circle
    // distance computation for hypothetical sphere of radius 6372795 meters.
    // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
    // Courtesy of Maarten Lamers
    double delta = radians(long1 - long2);
    double sdlong = sin(delta);
    double cdlong = cos(delta);
    lat1 = radians(lat1);
    lat2 = radians(lat2);
    double slat1 = sin(lat1);
    double clat1 = cos(lat1);
    double slat2 = sin(lat2);
    double clat2 = cos(lat2);
    delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
    delta = sq(delta);
    delta += sq(clat2 * sdlong);
    delta = sqrt(delta);
    double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
    delta = atan2(delta, denom);
    return delta * 6372795;
}

static double courseTo(double lat1, double long1, double lat2, double long2)
{
    // returns course in degrees (North=0, West=270) from position 1 to position 2,
    // both specified as signed decimal-degrees latitude and longitude.
    // Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
    // Courtesy of Maarten Lamers
    double dlon = radians(long2 - long1);
    lat1 = radians(lat1);
    lat2 = radians(lat2);
    double a1 = sin(dlon) * cos(lat2);
    double a2 = sin(lat1) * cos(lat2) * cos(dlon);
    a2 = cos(lat1) * sin(lat2) - a2;
    a2 = atan2(a1, a2);
    if (a2 < 0.0)
    {
        a2 += TWO_PI;
    }
    return degrees(a2);
}

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
    Clock_GetInfo(&gpsInfo.clock);
    *info = gpsInfo;
    return true;
}

void HAL::GPS_Init()
{
    gpsInfo.longitude = CONFIG_GPS_LONGITUDE_DEFAULT;
    gpsInfo.latitude = CONFIG_GPS_LATITUDE_DEFAULT;
    gpsInfo.isVaild = Parser_Init(&gpxParser, &fileInfo);

    if (gpsInfo.isVaild)
    {
        gpsInfo.satellites = 10;
        lv_timer_create(
            [](lv_timer_t* timer) {
                GPS_Update();
            },
            CONFIG_GPS_REFR_PERIOD,
            nullptr
        );
    }
}

static time_t Clock_MakeTime(GPX_Parser::Time_t* time)
{
    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_year = time->year - 1900;
    t.tm_mon = time->month;
    t.tm_mday = time->day;
    t.tm_hour = time->hour;
    t.tm_min = time->minute;
    t.tm_sec = time->second;

    return mktime(&t);
}

static double Clock_GetDiffTime(GPX_Parser::Time_t* time1, GPX_Parser::Time_t* time2)
{
    time_t t1 = Clock_MakeTime(time1);
    time_t t2 = Clock_MakeTime(time2);
    return difftime(t1, t2);
}

void HAL::GPS_Update()
{
    static GPX_Parser::Point_t prePoint;
    static bool isReset = false;

    GPX_Parser::Point_t point;
    memset(&point, 0, sizeof(point));

    int parserFlag = gpxParser.ReadNext(&point);

    if (parserFlag & GPX_Parser::PARSER_FLAG_LAT && parserFlag & GPX_Parser::PARSER_FLAG_LNG)
    {
        if (!isReset)
        {
            gpsInfo.longitude = point.longitude;
            gpsInfo.latitude = point.latitude;
            gpsInfo.altitude = point.altitude;

            prePoint = point;
            isReset = true;
            return;
        }

        double distance = GPS_GetDistanceOffset(&gpsInfo, point.longitude, point.latitude);
        double diffTime = CONFIG_GPS_REFR_PERIOD / 1000.0;

        if (parserFlag & GPX_Parser::PARSER_FLAG_TIME)
        {
            diffTime = Clock_GetDiffTime(&point.time, &prePoint.time);
        }

        if (std::abs(diffTime) >= 0.0001)
        {
            gpsInfo.speed = (float)(distance / diffTime) * 3.6f;
        }

        gpsInfo.course = (float)courseTo(
            gpsInfo.latitude,
            gpsInfo.longitude,
            point.latitude,
            point.longitude
        );

        gpsInfo.longitude = point.longitude;
        gpsInfo.latitude = point.latitude;
        gpsInfo.altitude = point.altitude;
        prePoint = point;
    }
    else if (parserFlag & GPX_Parser::PARSER_FLAG_EOF)
    {
        lv_fs_seek(&fileInfo.file, 0, LV_FS_SEEK_SET);
        isReset = false;
    }
}

double HAL::GPS_GetDistanceOffset(GPS_Info_t* info, double preLong, double preLat)
{
    return distanceBetween(info->latitude, info->longitude, preLat, preLong);
}
