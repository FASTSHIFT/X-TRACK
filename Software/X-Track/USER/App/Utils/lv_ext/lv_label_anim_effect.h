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
#ifndef __LV_LABEL_ANIM_EFFECT_H
#define __LV_LABEL_ANIM_EFFECT_H

#include "lvgl/lvgl.h"

typedef struct{
    lv_obj_t * label_1;
    lv_obj_t * label_2;
    lv_anim_t anim_now;
    lv_anim_t anim_next;
    lv_coord_t y_offset;
    uint8_t value_last;
}lv_label_anim_effect_t;

void lv_label_anim_effect_init(
    lv_label_anim_effect_t * effect, 
    lv_obj_t * cont, 
    lv_obj_t * label_copy,
    uint16_t anim_time = 200
);
void lv_label_anim_effect_check_value(
    lv_label_anim_effect_t * effect, 
    uint8_t value,
    lv_anim_enable_t anim_enable = LV_ANIM_ON
);

#endif
