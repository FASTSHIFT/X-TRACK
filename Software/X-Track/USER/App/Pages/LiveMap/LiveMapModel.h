#ifndef __LIVEMAP_MODEL_H
#define __LIVEMAP_MODEL_H

#include "lvgl/lvgl.h"
#include "Utils/MapConv/MapConv.h"
#include "Utils/TileConv/TileConv.h"
#include "Utils/TrackFilter/TrackPointFilter.h"
#include "Utils/lv_allocator/lv_allocator.h"
#include "Common/DataProc/DataProc.h"
#include <vector>

class LiveMapModel
{
public:
    MapConv mapConv;
    TileConv tileConv;
    TrackPointFilter pointFilter;
    
    HAL::SportStatus_Info_t sportStatusInfo;

public:
    void Init();
    void Deinit();
    void GetGPS_Info(HAL::GPS_Info_t* info)
    {
        account->Pull("GPS", info, sizeof(HAL::GPS_Info_t));

        /* Default location : Tian An Men */
        if (!info->isVaild)
        {
            info->longitude = 116.391332;
            info->latitude = 39.907415;
            info->altitude = 53.0f;
        }
    }
 
    void TrackAddPoint(int32_t x, int32_t y);

    void TrackAddLocationPoints(float lng, float lat)
    {
        LocationPoint_t point = { lng, lat };
        locationPoints.push_back(point);
    }

    void TrackRecalcAll(int level)
    {
        TrackReset();

        TrackPointFilter ptFilter;

        int lvl = mapConv.GetLevel();

        mapConv.SetLevel(level);

        uint32_t size = locationPoints.size();
        for (uint32_t i = 0; i < size; i++)
        {
            uint32_t mapX, mapY;
            mapConv.ConvertMapCoordinate(
                locationPoints[i].longitude, locationPoints[i].latitude, 
                &mapX, &mapY
            );

            bool isOutput = ptFilter.PushPoint(mapX, mapY);

            if (isOutput)
            {
                TrackAddPoint(mapX, mapY);
            }
        }

        mapConv.SetLevel(lvl);
    }

    void TrackReset();
    uint32_t TrackGetCnt()
    {
        return trackPoints.size();
    }

    TileConv::Point_t* TrackGetPoints()
    {
        return trackPoints.size() ? &trackPoints[0] : nullptr;
    }

private:
    typedef struct
    {
        float longitude;
        float latitude;
    } LocationPoint_t;
    
private:
    Account* account;

    std::vector<TileConv::Point_t, lv_allocator<TileConv::Point_t>> trackPoints;
    std::vector<LocationPoint_t, lv_allocator<LocationPoint_t>> locationPoints;

private:
    static int onEvent(Account* account, Account::EventParam_t* param);
};

#endif
