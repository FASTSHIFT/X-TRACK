#include "Dialplate.h"

Dialplate::Dialplate()
{  
}

Dialplate::~Dialplate()
{
}

void Dialplate::onCustomAttrConfig()
{
    SetCustomLoadAnimType(PageManager::LOAD_ANIM_NONE);
}

void Dialplate::onViewLoad()
{
    Model.Init();
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
    StatusBar_SetStyle(STATUS_BAR_STYLE_TRANSP);
    lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), group);
    lv_group_focus_obj(root);
    lv_group_set_editing(group, true);
    task = lv_task_create(TaskUpdate, 500, LV_TASK_PRIO_MID, this);
    lv_task_ready(task);

    View.AppearAnimStart();
}

void Dialplate::onViewDidAppear()
{

}

void Dialplate::onViewWillDisappear()
{
    View.AppearAnimStart(true);
}

void Dialplate::onViewDidDisappear()
{
    lv_task_del(task); 
}

void Dialplate::onViewDidUnload()
{
    Model.Deinit();
    lv_group_del(group);
}

void Dialplate::AttachEvent(lv_obj_t* obj)
{
    lv_obj_set_user_data(obj, this);
    lv_obj_set_event_cb(obj, EventHandler);
}

void Dialplate::Update()
{
    char buf[16];
    lv_label_set_text_fmt(View.ui.topInfo.labelSpeed, "%02d", (int)Model.GetSpeed());

    lv_label_set_text_fmt(View.ui.bottomInfo.infoGrp[0].lableValue, "%0.1f km/h", Model.GetAvgSpeed());
    lv_label_set_text(
        View.ui.bottomInfo.infoGrp[1].lableValue, 
        DataProc::ConvTime(Model.sportStatusInfo.singleTime, buf, sizeof(buf))
    );
    lv_label_set_text_fmt(View.ui.bottomInfo.infoGrp[2].lableValue, "%0.1f km", Model.sportStatusInfo.singleDistance / 1000);
    lv_label_set_text_fmt(View.ui.bottomInfo.infoGrp[3].lableValue, "%d Kcal", Model.sportStatusInfo.singleKcal / 1000);
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
            instance->Manager->Push("Pages/LiveMap");
        }
    }
}
