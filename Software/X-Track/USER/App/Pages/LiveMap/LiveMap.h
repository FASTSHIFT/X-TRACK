#ifndef __LIVEMAP_PRESENTER_H
#define __LIVEMAP_PRESENTER_H

#include "LiveMapView.h"
#include "LiveMapModel.h"

namespace Page
{

class LiveMap : public PageBase
{
public:
    LiveMap();
    virtual ~LiveMap();

    virtual void onCustomAttrConfig();
    virtual void onViewLoad();
    virtual void onViewDidLoad();
    virtual void onViewWillAppear();
    virtual void onViewDidAppear();
    virtual void onViewWillDisappear();
    virtual void onViewDidDisappear();
    virtual void onViewDidUnload();

private:
    LiveMapView View;
    LiveMapModel Model;

    struct
    {
        uint32_t lastMapUpdateTime;
        uint32_t lastContShowTime;
        lv_timer_t* timer;
        lv_group_t* group;
        TileConv::Point_t lastTileContOriPoint;
        bool isTrackAvtive;
        bool trackReloadReq;
    } priv;

    static uint16_t mapLevel;

private:
    void Update();
    void MapUpdate();
    void TrackViewLineUpdate(int32_t x, int32_t y);
    void TrackViewLineReload();
    bool TrackCheckTileContReload();
    void MapUpdateWait(uint32_t ms);
    void SportInfoUpdate();
    void AttachEvent(lv_obj_t* obj);

    static void onEvent(lv_event_t* event);
};

}

#endif
