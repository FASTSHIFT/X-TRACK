#include "TemplateView.h"
#include <stdarg.h>
#include <stdio.h>

void TemplateView::Create(lv_obj_t* par)
{
    lv_obj_t* label = lv_label_create(par, NULL);
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_obj_set_auto_realign(label, true);
    lv_obj_set_style_local_text_font(
        label,
        LV_LABEL_PART_MAIN,
        LV_STATE_DEFAULT,
        Resource.GetFont("Font_MicrosoftYaHei_16")
    );
    lv_label_set_text(label, "");
    ui.labelTitle = label;

    label = lv_label_create(par, label);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    ui.labelTick = label;
}
