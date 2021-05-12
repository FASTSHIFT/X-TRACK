#include "ResourcePool.h"

/* Global resource manager */
ResourcePool Resource;

extern "C" {
#define IMPORT_FONT(name) do{\
    LV_FONT_DECLARE(font_##name)\
    Resource.Font_.AddResource(#name, (void*)&font_##name);\
}while(0)

#define IMPORT_IMG(name) do{\
    LV_IMG_DECLARE(img_src_##name)\
    Resource.Image_.AddResource(#name, (void*)&img_src_##name);\
}while (0)

static void Resource_Init()
{
    /* Import Fonts */
    IMPORT_FONT(bahnschrift_13);
    IMPORT_FONT(bahnschrift_17);
    IMPORT_FONT(bahnschrift_32);
    IMPORT_FONT(bahnschrift_65);

    /* Import Images */
    IMPORT_IMG(alarm);
    IMPORT_IMG(battery);
    IMPORT_IMG(gps_arrow);
    IMPORT_IMG(gps_pin);
    IMPORT_IMG(locate);
    IMPORT_IMG(menu);
    IMPORT_IMG(origin_point);
    IMPORT_IMG(satellite);
    IMPORT_IMG(trip);
    IMPORT_IMG(start);
    IMPORT_IMG(pause);
    IMPORT_IMG(stop);
}

} /* extern "C" */

void ResourcePool::Init()
{
    Font_.SetDefault((void*)&lv_font_montserrat_14);

    Resource_Init();
}