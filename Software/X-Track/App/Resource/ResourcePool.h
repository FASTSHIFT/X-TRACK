#ifndef __RESOURCE_POOL
#define __RESOURCE_POOL

#include "lvgl/lvgl.h"

namespace ResourcePool
{

void Init();
lv_font_t* GetFont(const char* name);
const void* GetImage(const char* name);

}

#endif
