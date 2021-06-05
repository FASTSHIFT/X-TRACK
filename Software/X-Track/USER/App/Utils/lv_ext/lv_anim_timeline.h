/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the follo18wing conditions:
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
#ifndef __LV_ANIM_TIMELINE_H
#define __LV_ANIM_TIMELINE_H

#include "lvgl/lvgl.h"

typedef struct {
    uint32_t start_time;
    lv_obj_t* obj;
    
    lv_anim_exec_xcb_t exec_cb;
    int32_t start;
    int32_t end;
    uint16_t duration;
    lv_anim_path_cb_t path_cb;
}lv_anim_timeline_t;

#define LV_ANIM_TIMELINE_ELEM_DEF(obj, start_time, attr, end, duration)\
{\
    (start_time),\
    (obj), \
    (lv_anim_exec_xcb_t)lv_obj_set_##attr,\
    lv_obj_get_##attr(obj),\
    (end),\
    (duration),\
    lv_anim_path_ease_in_out\
}

uint32_t lv_anim_timeline_start(const lv_anim_timeline_t* anim_timeline, uint32_t len, bool playback = false);
void     lv_anim_timeline_del(const lv_anim_timeline_t* anim_timeline, uint32_t len);
uint32_t lv_anim_timeline_get_playtime(const lv_anim_timeline_t* anim_timeline, uint32_t len);

#endif // ! __LV_ANIM_TIMELINE_H
