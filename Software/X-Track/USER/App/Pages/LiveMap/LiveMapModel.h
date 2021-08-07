#ifndef __LIVEMAP_MODEL_H
#define __LIVEMAP_MODEL_H

#include "lvgl/lvgl.h"
#include "Utils/MapConv/MapConv.h"
#include "Utils/TileConv/TileConv.h"
#include "Utils/TrackFilter/TrackFilter.h"
#include "Utils/lv_allocator/lv_allocator.h"
#include "Common/DataProc/DataProc.h"
#include <vector>

namespace Page
{

class LiveMapModel
{
public:
    MapConv mapConv;
    TileConv tileConv;
    TrackPointFilter pointFilter;
    
    HAL::SportStatus_Info_t sportStatusInfo;

public:
    LiveMapModel();
    ~LiveMapModel() {}
    void Init();
    void Deinit();
    void GetGPS_Info(HAL::GPS_Info_t* info);
    void GetArrowTheme(char* buf, uint32_t size);

    bool TrackGetFilterActive()
    {
        DataProc::TrackFilter_Info_t info;
        account->Pull("TrackFilter", &info, sizeof(info));
        return info.isActive;
    }
 
    void TrackAddPoint(int32_t x, int32_t y);
    void TrackReload();
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
    
private:
    Account* account;

    std::vector<TileConv::Point_t, lv_allocator<TileConv::Point_t>> trackPoints;

private:
    static int onEvent(Account* account, Account::EventParam_t* param);

private:
};

}

#endif
