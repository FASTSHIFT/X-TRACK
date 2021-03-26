#include "Template.h"

Template::Template()
{
}

Template::~Template()
{

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
    param.color = LV_COLOR_WHITE;
    param.time = 1000;

    PAGE_STASH_POP(param);

    lv_obj_set_style_local_bg_color(root, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, param.color);
    task = lv_task_create(TaskUpdate, param.time, LV_TASK_PRIO_MID, this);
}

void Template::onViewDidAppear()
{

}

void Template::onViewWillDisappear()
{

}

void Template::onViewDidDisappear()
{
    lv_task_del(task);
}

void Template::onViewDidUnload()
{

}

void Template::AttachEvent(lv_obj_t* obj)
{
    lv_obj_set_user_data(obj, this);
    lv_obj_set_event_cb(obj, EventHandler);
}

void Template::Update()
{
    lv_label_set_text_fmt(View.ui.labelTick, "tick = %d\nsave = %d", Model.GetData(), Model.TickSave);
}

void Template::TaskUpdate(lv_task_t* task)
{
    Template* instance = (Template*)task->user_data;

    instance->Update();
}

void Template::EventHandler(lv_obj_t* obj, lv_event_t event)
{
    Template* instance = (Template*)obj->user_data;

    if (event == LV_EVENT_CLICKED || event == LV_EVENT_LEAVE)
    {
        if (obj == instance->root)
        {
            instance->Manager->Pop();
        }
    }
}
