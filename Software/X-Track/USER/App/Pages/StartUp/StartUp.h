#ifndef __START_UP_PRESENTER_H
#define __START_UP_PRESENTER_H

#include "StartUpView.h"
#include "StartUpModel.h"

namespace Page
{

class StartUp : public PageBase
{
public:

public:
    StartUp();
    virtual ~StartUp();

    virtual void onCustomAttrConfig();
    virtual void onViewLoad();
    virtual void onViewDidLoad();
    virtual void onViewWillAppear();
    virtual void onViewDidAppear();
    virtual void onViewWillDisappear();
    virtual void onViewDidDisappear();
    virtual void onViewDidUnload();

private:
    static void onTimer(lv_timer_t* timer);
    static void onEvent(lv_event_t* event);

private:
    StartUpView View;
    StartUpModel Model;
};

}

#endif
