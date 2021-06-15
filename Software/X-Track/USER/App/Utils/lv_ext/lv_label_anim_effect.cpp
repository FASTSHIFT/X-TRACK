/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "lv_label_anim_effect.h"
#include <stdlib.h>

void lv_label_anim_effect_init(
    lv_label_anim_effect_t * effect,
    lv_obj_t * cont,
    lv_obj_t * label_copy,
    uint16_t anim_time
)
{
    lv_obj_t * label = lv_label_create(cont);
    effect->y_offset = (lv_obj_get_height(cont) - lv_obj_get_height(label_copy)) / 2 + 1;
    lv_obj_align_to(label, label_copy, LV_ALIGN_OUT_BOTTOM_MID, 0, effect->y_offset);
    effect->label_1 = label_copy;
    effect->label_2 = label;

    effect->value_last = 0;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_time(&a, anim_time);
    lv_anim_set_delay(&a, 0);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);

    effect->anim_now = a;
    effect->anim_next = a;
}

void lv_label_anim_effect_check_value(lv_label_anim_effect_t * effect, uint8_t value, lv_anim_enable_t anim_enable)
{
    /*如果值相等则不切换*/
    if(value == effect->value_last)
        return;

    if(anim_enable == LV_ANIM_ON)
    {
        /*标签对象*/
        lv_obj_t * next_label;
        lv_obj_t * now_label;
        /*判断两个标签的相对位置，确定谁是下一个标签*/
        if(lv_obj_get_y(effect->label_2) > lv_obj_get_y(effect->label_1))
        {
            now_label = effect->label_1;
            next_label = effect->label_2;
        }
        else
        {
            now_label = effect->label_2;
            next_label = effect->label_1;
        }

        lv_label_set_text_fmt(now_label, "%d", effect->value_last);
        lv_label_set_text_fmt(next_label, "%d", value);
        effect->value_last = value;
        /*对齐*/
        lv_obj_align_to(next_label, now_label, LV_ALIGN_OUT_TOP_MID, 0, -effect->y_offset);
        /*计算需要的Y偏移量*/
        lv_coord_t y_offset = abs(lv_obj_get_y(now_label) - lv_obj_get_y(next_label));

        /*滑动动画*/
        lv_anim_t* a;
        a = &(effect->anim_now);
        lv_anim_set_var(a, now_label);
        lv_anim_set_values(a, lv_obj_get_y(now_label), lv_obj_get_y(now_label) + y_offset);
        lv_anim_start(a);

        a = &(effect->anim_next);
        lv_anim_set_var(a, next_label);
        lv_anim_set_values(a, lv_obj_get_y(next_label), lv_obj_get_y(next_label) + y_offset);
        lv_anim_start(a);
    }
    else
    {
        lv_label_set_text_fmt(effect->label_1, "%d", value);
        lv_label_set_text_fmt(effect->label_2, "%d", value);
        effect->value_last = value;
    }
}
