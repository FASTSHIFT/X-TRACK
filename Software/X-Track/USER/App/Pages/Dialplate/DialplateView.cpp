#include "DialplateView.h"
#include <stdarg.h>
#include <stdio.h>

void DialplateView::Create(lv_obj_t* par)
{
    lv_obj_t* label = lv_label_create(par, NULL);
    lv_obj_set_auto_realign(label, true);
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(
        label,
        LV_LABEL_PART_MAIN,
        LV_STATE_DEFAULT,
        &lv_font_montserrat_48
    );
    lv_label_set_text(label, "00");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    ui.labelSpeed = label;

    label = lv_label_create(par, NULL);
    lv_label_set_text(label, "km/h");
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
    ui.labelUint = label;
}
