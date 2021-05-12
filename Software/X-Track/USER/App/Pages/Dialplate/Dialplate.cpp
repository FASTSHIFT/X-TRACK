#include "Dialplate.h"

Dialplate::Dialplate()
    : recState(RECORD_STATE_READY)
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
    
    lv_group_add_obj(group, View.ui.btnCont.btnMap);
    lv_group_add_obj(group, View.ui.btnCont.btnRec);
    lv_group_add_obj(group, View.ui.btnCont.btnMenu);

    lv_group_focus_obj(View.ui.btnCont.btnRec);

    AttachEvent(View.ui.btnCont.btnMap);
    AttachEvent(View.ui.btnCont.btnRec);
    AttachEvent(View.ui.btnCont.btnMenu);
}

void Dialplate::onViewDidLoad()
{

}

void Dialplate::onViewWillAppear()
{
    StatusBar::SetStyle(StatusBar::STYLE_TRANSP);

    lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), group);

    View.AppearAnimStart();
}

void Dialplate::onViewDidAppear()
{
    timer = lv_timer_create(onTimerUpdate, 1000, this);
    lv_timer_ready(timer);
}

void Dialplate::onViewWillDisappear()
{
    lv_timer_del(timer);
    //View.AppearAnimStart(true);
}

void Dialplate::onViewDidDisappear()
{
}

void Dialplate::onViewDidUnload()
{
    Model.Deinit();
    lv_group_del(group);
}

void Dialplate::AttachEvent(lv_obj_t* obj)
{
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

void Dialplate::Update()
{
    char buf[16];
    lv_label_set_text_fmt(View.ui.topInfo.labelSpeed, "%02d", (int)Model.GetSpeed());

    lv_label_set_text_fmt(View.ui.bottomInfo.labelInfoGrp[0].lableValue, "%0.1f km/h", Model.GetAvgSpeed());
    lv_label_set_text(
        View.ui.bottomInfo.labelInfoGrp[1].lableValue,
        DataProc::ConvTime(Model.sportStatusInfo.singleTime, buf, sizeof(buf))
    );
    lv_label_set_text_fmt(
        View.ui.bottomInfo.labelInfoGrp[2].lableValue, 
        "%0.1f km", 
        Model.sportStatusInfo.singleDistance / 1000
    );
    lv_label_set_text_fmt(
        View.ui.bottomInfo.labelInfoGrp[3].lableValue, 
        "%d cal", 
        int(Model.sportStatusInfo.singleCalorie)
    );
}

void Dialplate::onTimerUpdate(lv_timer_t* timer)
{
    Dialplate* instance = (Dialplate*)timer->user_data;

    instance->Update();
}

void Dialplate::onBtnClicked(lv_obj_t* btn)
{
    if (btn == View.ui.btnCont.btnMap)
    {
        Manager->Push("Pages/LiveMap");
    }
    else if (btn == View.ui.btnCont.btnRec)
    {
        switch (recState)
        {
        case RECORD_STATE_READY:
            Model.RecorderCtrl(Model.REC_START);
            recState = RECORD_STATE_RUN;
            break;
        case RECORD_STATE_RUN:
            Model.RecorderCtrl(Model.REC_PAUSE);
            recState = RECORD_STATE_READY;
            break;
        case RECORD_STATE_PAUSE:           
            recState = RECORD_STATE_RUN;
            break;
        default:
            break;
        }
        updateBtnRec();
    }
    else if (btn == View.ui.btnCont.btnMenu)
    {
        Manager->Push("Pages/MainMenu");
    }
}

void Dialplate::onRecord()
{
    switch (recState)
    {
    case RECORD_STATE_RUN:
        recState = RECORD_STATE_PAUSE;
        break;
    case RECORD_STATE_PAUSE:
        Model.RecorderCtrl(Model.REC_STOP);
        recState = RECORD_STATE_READY;
        break;
    default:
        break;
    }
    updateBtnRec();
}

void Dialplate::updateBtnRec()
{
    const char* res[] = {
        "start",
        "pause",
        "stop"
    };
    lv_obj_set_style_bg_img_src(View.ui.btnCont.btnRec, Resource.GetImage(res[recState]), 0);
}

void Dialplate::onEvent(lv_event_t* event)
{
    Dialplate* instance = (Dialplate*)lv_event_get_user_data(event);

    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);

    if (code == LV_EVENT_SHORT_CLICKED)
    {
        instance->onBtnClicked(obj);
    }

    if (code == LV_EVENT_LONG_PRESSED)
    {
        if (obj == instance->View.ui.btnCont.btnRec)
        {
            instance->onRecord();
        }
    }
}
