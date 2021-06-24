#ifndef __LIVEMAP_PRESENTER_H
#define __LIVEMAP_PRESENTER_H

#include "LiveMapView.h"
#include "LiveMapModel.h"

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
        int lastMapLevel;
    } priv;

private:
    void Update();
    void MapUpdate();
    void TrackLineUpdate(int32_t x, int32_t y);
    void MapUpdateWait(uint32_t ms);
    void SportInfoUpdate();
    void AttachEvent(lv_obj_t* obj);
    void CheckTileContOriPoint();

    static void onTimerUpdate(lv_timer_t* timer);
    static void onEvent(lv_event_t* event);
    static void onFilterOutput(TrackPointFilter* filter, const TrackPointFilter::Point_t* point);
};

#endif
