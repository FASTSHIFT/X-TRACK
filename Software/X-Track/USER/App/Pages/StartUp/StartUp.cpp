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
    lv_timer_t* timer = lv_timer_create(onTimer, 2000, this);
    lv_timer_set_repeat_count(timer, 1);
}

void StartUp::onViewDidLoad()
{
    lv_obj_fade_out(root, 500, 1500);
}

void StartUp::onViewWillAppear()
{
    Model.PlayMusic("StartUp");
    lv_anim_timeline_start(View.ui.anim_timeline);
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
    View.Delete();
    Model.SetEncoderEnable(true);
    Model.Deinit();
}

void StartUp::onTimer(lv_timer_t* timer)
{
    StartUp* instance = (StartUp*)timer->user_data;

    instance->Manager->Push("Pages/Dialplate");
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
