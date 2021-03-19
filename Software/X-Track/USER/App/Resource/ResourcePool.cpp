#include "ResourcePool.h"

ResourcePool Resource;

/* Import Fonts */
extern "C" {

}
#define ADD_FONT(font) Font.AddResource(#font, &font)

/* Import Images */
extern "C" {
    LV_IMG_DECLARE(img_src_gps_arrow);
    LV_IMG_DECLARE(img_src_gps_pin);
}
#define ADD_IMG(image) Image.AddResource(#image, (void*)&image)

void ResourcePool::Init()
{
    Font.SetDefault(&lv_font_montserrat_14);
    
    ADD_IMG(img_src_gps_arrow);
    ADD_IMG(img_src_gps_pin);
}
