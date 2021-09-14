#ifndef LV_MONKEY_H
#define LV_MONKEY_H

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _lv_monkey;
typedef struct _lv_monkey lv_monkey_t;

typedef struct {
	int32_t min;
	int32_t max;
} lv_monkey_range_t;

typedef struct {
	/**< Input device type*/
	lv_indev_type_t type;
	lv_monkey_range_t time;
	lv_monkey_range_t input_range;
}lv_monkey_config_t;

void lv_monkey_config_init(lv_monkey_config_t * config);
lv_monkey_t * lv_monkey_create(const lv_monkey_config_t * config);
void lv_monkey_del(lv_monkey_t* monkey);

#ifdef __cplusplus
}
#endif

#endif
