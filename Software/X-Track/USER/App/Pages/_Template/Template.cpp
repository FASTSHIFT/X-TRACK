#include "Template.h"

using namespace Page;

Template::Template()
{
}

Template::~Template()
{

}

void Template::onCustomAttrConfig()
{
    SetCustomCacheEnable(true);
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_OVER_BOTTOM, 1000, lv_anim_path_bounce);
}

void Template::onViewLoad()
{
    View.Create(root);
    lv_label_set_text(View.ui.labelTitle, Name);

    AttachEvent(root);

    Model.TickSave = Model.GetData();
}

void Template::onViewDidLoad()
{

}

void Template::onViewWillAppear()
{
    Param_t param;
    param.color = lv_color_white();
    param.time = 1000;

    PAGE_STASH_POP(param);

    lv_obj_set_style_bg_color(root, param.color, LV_PART_MAIN);
    timer = lv_timer_create(onTimerUpdate, param.time, this);
}

void Template::onViewDidAppear()
{

}

void Template::onViewWillDisappear()
{

}

void Template::onViewDidDisappear()
{
    lv_timer_del(timer);
}

void Template::onViewDidUnload()
{

}

void Template::AttachEvent(lv_obj_t* obj)
{
    lv_obj_set_user_data(obj, this);
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

void Template::Update()
{
    lv_label_set_text_fmt(View.ui.labelTick, "tick = %d save = %d", Model.GetData(), Model.TickSave);
}

void Template::onTimerUpdate(lv_timer_t* timer)
{
    Template* instance = (Template*)timer->user_data;

    instance->Update();
}

void Template::onEvent(lv_event_t* event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    Template* instance = (Template*)lv_obj_get_user_data(obj);

    if (obj == instance->root)
    {
        if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LEAVE)
        {
            instance->Manager->Pop();
        }
    }
}
