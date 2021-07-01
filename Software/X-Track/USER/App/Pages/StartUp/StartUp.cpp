#include "StartUp.h"

using namespace Page;

StartUp::StartUp()
{
}

StartUp::~StartUp()
{

}

void StartUp::onCustomAttrConfig()
{
    SetCustomCacheEnable(false);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE);
}

void StartUp::onViewLoad()
{
    Model.Init();
    Model.SetEncoderEnable(false);
    View.Create(root);
    lv_timer_create(onTimer, 2000, this);
}

void StartUp::onViewDidLoad()
{
    lv_obj_fade_out(root, 500, 1500);
}

void StartUp::onViewWillAppear()
{
    Model.PlayMusic("StartUp");
}

void StartUp::onViewDidAppear()
{

}

void StartUp::onViewWillDisappear()
{

}

void StartUp::onViewDidDisappear()
{
    StatusBar::Appear(true);
}

void StartUp::onViewDidUnload()
{
    Model.SetEncoderEnable(true);
    Model.Deinit();
}

void StartUp::onTimer(lv_timer_t* timer)
{
    StartUp* instance = (StartUp*)timer->user_data;

    instance->Manager->Push("Pages/Dialplate");

    lv_timer_del(timer);
}

void StartUp::onEvent(lv_event_t* event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    StartUp* instance = (StartUp*)lv_obj_get_user_data(obj);

    if (obj == instance->root)
    {
        if (code == LV_EVENT_LEAVE)
        {
            //instance->Manager->Pop();
        }
    }
}
