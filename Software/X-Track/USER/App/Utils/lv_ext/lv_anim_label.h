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
#ifndef LV_ANIM_LABEL_H
#define LV_ANIM_LABEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

typedef struct lv_anim_label_s {
    lv_obj_t* obj;
    lv_obj_t* label_1;
    lv_obj_t* label_2;
    lv_obj_t* label_act;
    lv_dir_t dir;
    uint32_t duration;
    lv_anim_path_cb_t path_cb;
    lv_anim_t a_enter;
    lv_anim_t a_exit;
}lv_anim_label_t;

lv_anim_label_t* lv_anim_label_create(lv_obj_t * parent);

void lv_anim_label_set_dir(lv_anim_label_t * alabel, lv_dir_t dir);

void lv_anim_label_set_style(lv_anim_label_t* alabel, lv_style_t * style);

void lv_anim_label_set_size(lv_anim_label_t* alabel, lv_coord_t width, lv_coord_t height);

void lv_anim_label_set_time(lv_anim_label_t* alabel, uint32_t duration);

void lv_anim_label_set_path(lv_anim_label_t* alabel, lv_anim_path_cb_t path_cb);

void lv_anim_label_set_custom_anin(lv_anim_label_t * alabel, const lv_anim_t* a_enter, const lv_anim_t* a_exit);

void lv_anim_label_push_text(lv_anim_label_t* alabel, const char* txt);

void lv_anim_label_del(lv_anim_label_t * alabel);

#ifdef __cplusplus
}
#endif

#endif
