#ifndef __LIVEMAP_CONTROLLER_H
#define __LIVEMAP_CONTROLLER_H

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

    struct {
        uint32_t localX, localY;
        uint32_t focusX, focusY;
        int32_t contLastX, contLastY;
    }mapPos;  

    uint32_t lastMapUpdateTime;

    lv_timer_t* timer;
    lv_group_t* group;

private:
    void Update();
    void MapUpdate();
    void MapUpdateWait(uint32_t ms);
    void SportInfoUpdate();
    void AttachEvent(lv_obj_t* obj);
    void LoadMap(uint32_t x, uint32_t y);
    void FocusMap(uint32_t x, uint32_t y);
    void ReCalibrateMapCont();

    static void onTimerUpdate(lv_timer_t* timer);
    static void onEvent(lv_event_t* event);
};

#endif
