#include "ResourcePool.h"
#include "Utils/ResourceManager/ResourceManager.h"

static ResourceManager Font_;
static ResourceManager Image_;

extern "C" {
#define IMPORT_FONT(name) \
do{\
    LV_FONT_DECLARE(font_##name)\
    Font_.AddResource(#name, (void*)&font_##name);\
}while(0)

#define IMPORT_IMG(name) \
do{\
    LV_IMG_DECLARE(img_src_##name)\
    Image_.AddResource(#name, (void*)&img_src_##name);\
}while (0)

    static void Resource_Init()
    {
        /* Import Fonts */
        IMPORT_FONT(bahnschrift_13);
        IMPORT_FONT(bahnschrift_17);
        IMPORT_FONT(bahnschrift_32);
        IMPORT_FONT(bahnschrift_65);
        IMPORT_FONT(agencyb_36);

        /* Import Images */
        IMPORT_IMG(alarm);
        IMPORT_IMG(battery);
        IMPORT_IMG(battery_info);
        IMPORT_IMG(bicycle);
        IMPORT_IMG(compass);
        IMPORT_IMG(gps_arrow_default);
        IMPORT_IMG(gps_arrow_dark);
        IMPORT_IMG(gps_arrow_light);
        IMPORT_IMG(gyroscope);
        IMPORT_IMG(locate);
        IMPORT_IMG(map_location);
        IMPORT_IMG(menu);
        IMPORT_IMG(origin_point);
        IMPORT_IMG(pause);
        IMPORT_IMG(satellite);
        IMPORT_IMG(sd_card);
        IMPORT_IMG(start);
        IMPORT_IMG(stop);
        IMPORT_IMG(storage);
        IMPORT_IMG(system_info);
        IMPORT_IMG(time_info);
        IMPORT_IMG(trip);
    }

} /* extern "C" */

void ResourcePool::Init()
{
    Resource_Init();
    Font_.SetDefault((void*)LV_FONT_DEFAULT);
}

lv_font_t* ResourcePool::GetFont(const char* name)
{
    return (lv_font_t*)Font_.GetResource(name);
}
const void* ResourcePool::GetImage(const char* name)
{
    return Image_.GetResource(name);
}
