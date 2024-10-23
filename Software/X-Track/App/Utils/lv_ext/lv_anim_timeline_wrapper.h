#ifndef LV_ANIM_TIMELINE_WRAPPER_H
#define LV_ANIM_TIMELINE_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

/*Data of anim_timeline*/
typedef struct {
    uint32_t start_time;
    lv_obj_t* obj;
    lv_anim_exec_xcb_t exec_cb;
    int32_t start;
    int32_t end;
    uint16_t duration;
    lv_anim_path_cb_t path_cb;
    bool early_apply;
} lv_anim_timeline_wrapper_t;

/**********************
* GLOBAL PROTOTYPES
**********************/

/**
 * Start animation according to the timeline
 * @param anim_timeline  timeline array address
 * @param playback       whether to play in reverse
 * @return timeline total time spent
 */
void lv_anim_timeline_add_wrapper(lv_anim_timeline_t* at, const lv_anim_timeline_wrapper_t* wrapper);

/**********************
*      MACROS
**********************/

#define LV_ANIM_TIMELINE_WRAPPER_END {0, NULL}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIM_TIMELINE_wrapper_H*/
