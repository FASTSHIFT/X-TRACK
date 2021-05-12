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
    typedef enum {
        RECORD_STATE_READY,
        RECORD_STATE_RUN,
        RECORD_STATE_PAUSE,
    }RecordState_t;

private:
    void Update();
    void AttachEvent(lv_obj_t* obj);
    static void onTimerUpdate(lv_timer_t* timer);
    static void onEvent(lv_event_t* event);
    void onBtnClicked(lv_obj_t* btn);
    void onRecord();
    void updateBtnRec();

private:
    DialplateView View;
    DialplateModel Model;
    lv_group_t* group;
    lv_timer_t* timer;  
    RecordState_t recState;
};

#endif
