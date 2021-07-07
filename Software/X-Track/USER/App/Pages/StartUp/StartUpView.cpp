#include "StartUpView.h"

using namespace Page;

#define COLOR_GREY      lv_color_hex(0x333333)
#define COLOR_ORANGE    lv_color_hex(0xff931e)

void StartUpView::Create(lv_obj_t* root)
{
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);

    lv_obj_t* label = lv_label_create(root);
    lv_obj_set_style_text_font(label, Resource.GetFont("agencyb_36"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_text(label, "X-TRACK");
    lv_obj_center(label);
    ui.labelLogo = label;

    lv_obj_t* bar = lv_bar_create(root);
    lv_obj_remove_style_all(bar);

    lv_obj_set_size(bar, 100, 4);
    lv_obj_align_to(bar, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    lv_obj_set_style_bg_color(bar, COLOR_GREY, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(bar, LV_RADIUS_CIRCLE, 0);

    lv_obj_set_style_bg_color(bar, COLOR_ORANGE, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(bar, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_bar_set_range(bar, 0, 1000);
    ui.bar = bar;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, 0, 1000);
    lv_anim_set_exec_cb(&a, [](void* bar, int32_t value) {
        lv_bar_set_value((lv_obj_t*)bar, value, LV_ANIM_OFF);
    });
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_time(&a, 1000);
    lv_anim_start(&a);
}
