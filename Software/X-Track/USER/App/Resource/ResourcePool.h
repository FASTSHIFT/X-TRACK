#include "../utils/PageManager/ResourceManager.h"

class ResourcePool{
public:
    void Init();
    lv_font_t* GetFont(const char* name)
    {
        return (lv_font_t*)Font.GetResource(name);
    }
    const void* GetImage(const char* name)
    {
        return Image.GetResource(name);
    }
private:
    ResourceManager Font;
    ResourceManager Image;
};

extern ResourcePool Resource;
