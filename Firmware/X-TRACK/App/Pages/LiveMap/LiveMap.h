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
    virtual void onViewUnload();
    virtual void onViewDidUnload();

private:
    LiveMapView View;
    LiveMapModel Model;

    struct
    {
        uint32_t lastMapUpdateTime;
        uint32_t lastContShowTime;
        lv_timer_t* timer;
        TileConv::Point_t lastTileContOriPoint;
        bool isTrackAvtive;
    } priv;

    static uint16_t mapLevelCurrent;

private:
    typedef  TrackLineFilter::Area_t Area_t;

private:
    void Update();
    void UpdateDelay(uint32_t ms);
    void CheckPosition();

    /* SportInfo */
    void SportInfoUpdate();

    /* MapTileCont */
    bool GetIsMapTileContChanged();
    void onMapTileContRefresh(const Area_t* area, int32_t x, int32_t y);
    void MapTileContUpdate(int32_t mapX, int32_t mapY, float course);
    void MapTileContReload();
    
    /* TrackLine */
    void TrackLineReload(const Area_t* area, int32_t x, int32_t y);
    void TrackLineAppend(int32_t x, int32_t y);
    void TrackLineAppendToEnd(int32_t x, int32_t y);
    static void onTrackLineEvent(TrackLineFilter* filter, TrackLineFilter::Event_t* event);
    
    void AttachEvent(lv_obj_t* obj);
    static void onEvent(lv_event_t* event);
};

}

#endif
