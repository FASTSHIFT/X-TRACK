#ifndef __PAGE_H
#define __PAGE_H

#include "../Utils/PageManager/PageManager.h"
#include "../Resource/ResourcePool.h"
#include "../Utils/lv_ext/lv_obj_ext_func.h"

#include "lvgl/lvgl.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#define MAP(x,in_min,in_max,out_min,out_max) \
    (((x)-(in_min))*((out_max)-(out_min))/((in_max)-(in_min))+(out_min))

#endif
