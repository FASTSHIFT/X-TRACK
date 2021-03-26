#include "Dialplate.h"

Dialplate::Dialplate()
{
}

Dialplate::~Dialplate()
{
}

void Dialplate::onViewLoad()
{
    lv_obj_set_style_local_bg_color(root, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_border_width(root, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_radius(root, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    View.Create(root);

    group = lv_group_create();
    lv_group_add_obj(group, root);

    AttachEvent(root);
}

void Dialplate::onViewDidLoad()
{

}

void Dialplate::onViewWillAppear()
{
    lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), group);
    lv_group_focus_obj(root);
    lv_group_set_editing(group, true);
    task = lv_task_create(TaskUpdate, 500, LV_TASK_PRIO_MID, this);
}

void Dialplate::onViewDidAppear()
{

}

void Dialplate::onViewWillDisappear()
{

}

void Dialplate::onViewDidDisappear()
{
    lv_task_del(task); 
}

void Dialplate::onViewDidUnload()
{
    lv_group_del(group);
}

void Dialplate::AttachEvent(lv_obj_t* obj)
{
    lv_obj_set_user_data(obj, this);
    lv_obj_set_event_cb(obj, EventHandler);
}

void Dialplate::Update()
{
    lv_label_set_text_fmt(View.ui.labelSpeed, "%02d", (int)Model.GetSpeed());
}

void Dialplate::TaskUpdate(lv_task_t* task)
{
    Dialplate* instance = (Dialplate*)task->user_data;

    instance->Update();
}

void Dialplate::EventHandler(lv_obj_t* obj, lv_event_t event)
{
    Dialplate* instance = (Dialplate*)obj->user_data;

    if (event == LV_EVENT_SHORT_CLICKED)
    {
        if (obj == instance->root)
        {
            instance->Manager->Push("LiveMapPage");
        }
    }
}
