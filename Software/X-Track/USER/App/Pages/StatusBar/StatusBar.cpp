#include "StatusBar.h"
#include "Common/DataProc/DataProc.h"

static Account* actStatusBar;

struct
{
    lv_obj_t* cont;

    struct
    {
        lv_obj_t* img;
        lv_obj_t* label;
    } satellite;

    lv_obj_t* labelClock;

    struct
    {
        lv_obj_t* img;
        lv_obj_t* objUsage;
        lv_obj_t* label;
    } battery;

} StatusBar;

#define BATT_USAGE_HEIGHT (lv_obj_get_height(StatusBar.battery.img) - 6)
#define BATT_USAGE_WIDTH  (lv_obj_get_width(StatusBar.battery.img) - 4)

static void StatusBar_Update(lv_task_t* task)
{
    /* satellite */
    HAL::GPS_Info_t gpsInfo;
    actStatusBar->Pull("GPS", 0, &gpsInfo, sizeof(gpsInfo));
    lv_label_set_text_fmt(StatusBar.satellite.label, "%d", gpsInfo.satellites);

    /* clock */
    HAL::Clock_Info_t clock;
    actStatusBar->Pull("Clock", 0, &clock, sizeof(clock));
    lv_label_set_text_fmt(StatusBar.labelClock, "%02d:%02d", clock.hour, clock.min);

    /* battery */
    HAL::Power_Info_t power;
    actStatusBar->Pull("Power", 0, &power, sizeof(power));
    lv_label_set_text_fmt(StatusBar.battery.label, "%d%%", power.usage);
    lv_coord_t h = MAP(power.usage, 0, 100, 0, BATT_USAGE_HEIGHT);
    lv_obj_set_height(StatusBar.battery.objUsage, h);
}

static lv_obj_t* StatusBar_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_cont_create(par, nullptr);
    lv_obj_reset_style_list(cont, LV_CONT_PART_MAIN);
    lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_cont_set_fit2(cont, LV_FIT_PARENT, LV_FIT_NONE);
    lv_obj_set_height(cont, 23);
    StatusBar.cont = cont;

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&style, LV_STATE_DEFAULT, Resource.GetFont("bahnschrift_13"));

    /* satellite */
    lv_obj_t* img = lv_img_create(cont, nullptr);
    lv_img_set_src(img, Resource.GetImage("satellite"));
    lv_obj_align(img, nullptr, LV_ALIGN_IN_LEFT_MID, 14, 0);
    StatusBar.satellite.img = img;

    lv_obj_t* label = lv_label_create(cont, nullptr);
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style);
    lv_obj_align(label, StatusBar.satellite.img, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_label_set_text(label, "0");
    StatusBar.satellite.label = label;

    /* clock */
    label = lv_label_create(cont, nullptr);
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style);
    lv_label_set_text(label, "00:00");
    lv_obj_align(label, nullptr, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_auto_realign(label, true);
    StatusBar.labelClock = label;

    /* battery */
    img = lv_img_create(cont, nullptr);
    lv_img_set_src(img, Resource.GetImage("battery"));
    lv_obj_align(img, nullptr, LV_ALIGN_IN_RIGHT_MID, -40, 0);
    StatusBar.battery.img = img;

    lv_obj_t* obj = lv_obj_create(img, nullptr);
    lv_obj_reset_style_list(obj, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_size(obj, BATT_USAGE_WIDTH, BATT_USAGE_HEIGHT);
    lv_obj_align(obj, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, -2);
    lv_obj_set_auto_realign(obj, true);
    StatusBar.battery.objUsage = obj;

    label = lv_label_create(cont, nullptr);
    lv_obj_add_style(label, LV_LABEL_PART_MAIN, &style);
    lv_obj_align(label, StatusBar.battery.img, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_label_set_text(label, "100%");
    StatusBar.battery.label = label;

    StatusBar_SetStyle(STATUS_BAR_STYLE_TRANSP);

    lv_task_t* task = lv_task_create(StatusBar_Update, 1000, LV_TASK_PRIO_MID, nullptr);
    lv_task_ready(task);

    return StatusBar.cont;
}

void StatusBar_SetStyle(StatusBar_Style_t style)
{
    lv_obj_t* cont = StatusBar.cont;
    if (style == STATUS_BAR_STYLE_TRANSP)
    {
        lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
        lv_obj_set_style_local_shadow_width(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
    }
    else if (style == STATUS_BAR_STYLE_BLACK)
    {
        lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_60);
        lv_obj_set_style_local_shadow_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        lv_obj_set_style_local_shadow_width(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
    }
    else
    {
        return;
    }
}

void StatusBar_Init(lv_obj_t* par)
{
    StatusBar_Create(par);

    actStatusBar = new Account("StatusBar", DataProc::Center());
    actStatusBar->Subscribe("GPS", nullptr);
    actStatusBar->Subscribe("Power", nullptr);
    actStatusBar->Subscribe("Clock", nullptr);
}

void StatusBar_AppearAnimStart(bool playback)
{
#define ANIM_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out}

    lv_anim_timeline_t anim_timeline[] = {
        ANIM_DEF(1000, StatusBar.cont, y, -lv_obj_get_height(StatusBar.cont), 0),
    };

    lv_anim_timeline_start(anim_timeline, ARRAY_SIZE(anim_timeline), playback);
}
