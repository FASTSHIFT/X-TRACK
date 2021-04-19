#ifndef __TEMPLATE_CONTROLLER_H
#define __TEMPLATE_CONTROLLER_H

#include "TemplateView.h"
#include "TemplateModel.h"

class Template : public PageBase
{
public:
    typedef struct
    {
        uint16_t time;
        lv_color_t color;
    } Param_t;

public:
    Template();
    virtual ~Template();

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
    TemplateView View;
    TemplateModel Model;
    lv_task_t* task;
};

#endif
