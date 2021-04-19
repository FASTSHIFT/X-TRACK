#ifndef __RESOURCE_POOL
#define __RESOURCE_POOL

#include "../utils/PageManager/ResourceManager.h"

class ResourcePool{
public:
    void Init();
    lv_font_t* GetFont(const char* name)
    {
        return (lv_font_t*)Font_.GetResource(name);
    }
    const void* GetImage(const char* name)
    {
        return Image_.GetResource(name);
    }

    ResourceManager Font_;
    ResourceManager Image_;
};

extern ResourcePool Resource;

#endif
