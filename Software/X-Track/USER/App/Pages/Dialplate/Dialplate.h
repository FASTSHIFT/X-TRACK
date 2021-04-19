#ifndef __DIALPLATE_CONTROLLER_H
#define __DIALPLATE_CONTROLLER_H

#include "DialplateView.h"
#include "DialplateModel.h"

class Dialplate : public PageBase
{
public:
    Dialplate();
    virtual ~Dialplate();

    virtual void onCustomAttrConfig();
    virtual void onViewLoad();
    virtual void onViewDidLoad();
    virtual void onViewWillAppear();
    virtual void onViewDidAppear();
    virtual void onViewWillDisappear();
    virtual void onViewDidDisappear();
    virtual void onViewDidUnload();

private:
    void Update();
    void AttachEvent(lv_obj_t* obj);
    static void TaskUpdate(lv_task_t* task);
    static void EventHandler(lv_obj_t* obj, lv_event_t event);

private:
    DialplateView View;
    DialplateModel Model;
    lv_group_t* group;
    lv_task_t* task;  
};

#endif
