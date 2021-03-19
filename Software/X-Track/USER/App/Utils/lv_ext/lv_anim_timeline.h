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
