#include "Startup.h"

using namespace Page;

Startup::Startup()
{
}

Startup::~Startup()
{

}

void Startup::onCustomAttrConfig()
{
    SetCustomCacheEnable(false);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE);
}

void Startup::onViewLoad()
{
    Model.Init();
    Model.SetEncoderEnable(false);
    View.Create(root);
    lv_timer_t* timer = lv_timer_create(onTimer, 2000, this);
    lv_timer_set_repeat_count(timer, 1);
}

void Startup::onViewDidLoad()
{
    lv_obj_fade_out(root, 500, 1500);
}

void Startup::onViewWillAppear()
{
    Model.PlayMusic("Startup");
    lv_anim_timeline_start(View.ui.anim_timeline);
}

void Startup::onViewDidAppear()
{

}

void Startup::onViewWillDisappear()
{

}

void Startup::onViewDidDisappear()
{
    Model.SetStatusBarAppear(true);
}

void Startup::onViewDidUnload()
{
    View.Delete();
    Model.SetEncoderEnable(true);
    Model.Deinit();
}

void Startup::onTimer(lv_timer_t* timer)
{
    Startup* instance = (Startup*)timer->user_data;

    instance->Manager->Push("Pages/Dialplate");
}

void Startup::onEvent(lv_event_t* event)
{
    Startup* instance = (Startup*)lv_event_get_user_data(event);
    LV_ASSERT_NULL(instance);

    lv_obj_t* obj = lv_event_get_current_target(event);
    lv_event_code_t code = lv_event_get_code(event);

    if (obj == instance->root)
    {
        if (code == LV_EVENT_LEAVE)
        {
            //instance->Manager->Pop();
        }
    }
}
