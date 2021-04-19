#include "DialplateView.h"
#include <stdarg.h>
#include <stdio.h>

void DialplateView::Create(lv_obj_t* root)
{
    lv_obj_reset_style_list(root, LV_OBJ_PART_MAIN);

    BottomInfo_Create(root);
    TopInfo_Create(root);
}

void DialplateView::TopInfo_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_cont_create(par, nullptr);
    lv_obj_reset_style_list(cont, LV_OBJ_PART_MAIN);
    lv_cont_set_fit2(cont, LV_FIT_PARENT, LV_FIT_NONE);
    
    lv_obj_set_height(cont, 142);
    lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x333333));

    lv_obj_set_style_local_radius(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 27);
    lv_obj_set_y(cont, -36);
    ui.topInfo.cont = cont;

    lv_obj_t* label = lv_label_create(cont, nullptr);
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Resource.GetFont("bahnschrift_65"));
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_label_set_text(label, "00");
    lv_obj_set_auto_realign(label, true);
    lv_obj_align(label, nullptr, LV_ALIGN_IN_TOP_MID, 0, 63);
    ui.topInfo.labelSpeed = label;

    label = lv_label_create(cont, nullptr);
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Resource.GetFont("bahnschrift_17"));
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_label_set_text(label, "km/h");
    lv_obj_align(label, ui.topInfo.labelSpeed, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    ui.topInfo.labelUint = label;
}

void DialplateView::BottomInfo_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_cont_create(par, nullptr);
    lv_obj_reset_style_list(cont, LV_CONT_PART_MAIN);
    lv_cont_set_fit2(cont, LV_FIT_PARENT, LV_FIT_NONE);
    lv_obj_set_height(cont, 132);
    lv_obj_align(cont, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_cont_set_layout(cont, LV_LAYOUT_PRETTY_MID);
    lv_obj_set_style_local_pad_hor(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_pad_top(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
    ui.bottomInfo.cont = cont;

    const char* unitText[4] = {
        "AVG",
        "Time",
        "Trip",
        "Kilocalorie"
    };

    for (int i = 0; i < ARRAY_SIZE(ui.bottomInfo.infoGrp); i++)
    {
        SubInfoGrp_Create(cont, &(ui.bottomInfo.infoGrp[i]), unitText[i]);
    }

    lv_color_t btnColor[] = {
        lv_color_hex(0x666666),
        lv_color_hex(0xff931e),
        lv_color_hex(0x666666),
    };

    const char* btnImg[] = {
        "locate",
        "stop",
        "menu"
    };

    for (int i = 0; i < ARRAY_SIZE(ui.bottomInfo.btnGrp); i++)
    {
        ui.bottomInfo.btnGrp[i] = Btn_Create(cont, btnColor[i], Resource.GetImage(btnImg[i]));
    }
}

void DialplateView::SubInfoGrp_Create(lv_obj_t* par, SubInfo_t* info, const char* unitText)
{
    lv_obj_t* cont = lv_cont_create(par, nullptr);
    lv_obj_reset_style_list(cont, LV_CONT_PART_MAIN);
    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_MID);
    lv_obj_set_size(cont, 93, 39);
    
    lv_obj_t* label = lv_label_create(cont, nullptr);
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Resource.GetFont("bahnschrift_17"));
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    info->lableValue = label;

    label = lv_label_create(cont, label);
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Resource.GetFont("bahnschrift_13"));
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xb3b3b3));
    lv_label_set_text(label, unitText);
    info->lableUnit = label;

    info->cont = cont;
}

lv_obj_t* DialplateView::Btn_Create(lv_obj_t* par, lv_color_t color, const void* img_src)
{
    lv_obj_t* obj = lv_obj_create(par, nullptr);
    lv_obj_reset_style_list(obj, LV_OBJ_PART_MAIN);
    lv_obj_set_size(obj, 40, 31);

    lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, color);
    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0xbbbbbb));
    lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 9);

    lv_obj_t* img = lv_img_create(obj, nullptr);
    lv_img_set_src(img, img_src);
    lv_obj_align(img, nullptr, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_auto_realign(img, true);

    return obj;
}

void DialplateView::AppearAnimStart(bool playback)
{
#define ANIM_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out}

#define ANIM_OPA_DEF(start_time, obj) ANIM_DEF(start_time, obj, opa_scale, LV_OPA_TRANSP, LV_OPA_COVER)

     static lv_coord_t y_tar_top = lv_obj_get_y(ui.topInfo.cont);
     static lv_coord_t y_tar_bottom = lv_obj_get_y(ui.bottomInfo.cont);
     static lv_coord_t h_tar_btn = lv_obj_get_height(ui.bottomInfo.btnGrp[0]);

     lv_anim_timeline_t anim_timeline[] = {
        ANIM_DEF(0, ui.topInfo.cont, y, -lv_obj_get_height(ui.topInfo.cont), y_tar_top),

        ANIM_DEF(200, ui.bottomInfo.cont, y, -lv_obj_get_height(ui.bottomInfo.cont), y_tar_bottom),
        ANIM_OPA_DEF(200, ui.bottomInfo.cont),

        ANIM_DEF(500, ui.bottomInfo.btnGrp[0], height, 0, h_tar_btn),
        ANIM_DEF(600, ui.bottomInfo.btnGrp[1], height, 0, h_tar_btn),
        ANIM_DEF(700, ui.bottomInfo.btnGrp[2], height, 0, h_tar_btn),
    };

    lv_anim_timeline_start(anim_timeline, ARRAY_SIZE(anim_timeline), playback);
}
