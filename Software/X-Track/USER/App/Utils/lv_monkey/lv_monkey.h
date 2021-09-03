#ifndef LV_MONKEY_H
#define LV_MONKEY_H

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _lv_monkey;
typedef struct _lv_monkey lv_monkey_t;

lv_monkey_t * lv_monkey_create(lv_indev_type_t type, uint32_t interval_min, uint32_t interval_max);
void lv_monkey_del(lv_monkey_t* monkey);

#ifdef __cplusplus
}
#endif

#endif
