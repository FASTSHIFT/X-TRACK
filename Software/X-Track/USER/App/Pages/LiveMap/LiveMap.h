#ifndef __LIVEMAP_CONTROLLER_H
#define __LIVEMAP_CONTROLLER_H

#include "LiveMapView.h"
#include "LiveMapModel.h"

class LiveMap : public PageBase
{
public:
    LiveMap();
    virtual ~LiveMap();

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

    uint32_t mapLocalX, mapLocalY;
    uint32_t mapFocusX, mapFocusY;
    int32_t contLastX, contLastY;

    void Update();
    void MapUpdate();
    void AttachEvent(lv_obj_t* obj);

    void LoadMap(uint32_t x, uint32_t y);
    void FocusMap(uint32_t x, uint32_t y);

    lv_task_t* taskUpdate;
    lv_group_t* group;
    static void TaskUpdate(lv_task_t* task);
    static void EventHandler(lv_obj_t* obj, lv_event_t event);
};

#endif
