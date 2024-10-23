#ifndef __STARTUP_PRESENTER_H
#define __STARTUP_PRESENTER_H

#include "StartUpView.h"
#include "StartUpModel.h"

namespace Page
{

class Startup : public PageBase
{
public:

public:
    Startup();
    virtual ~Startup();

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
    static void onTimer(lv_timer_t* timer);
    static void onEvent(lv_event_t* event);

private:
    StartupView View;
    StartupModel Model;
};

}

#endif
