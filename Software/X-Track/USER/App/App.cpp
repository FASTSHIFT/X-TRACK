#include "Utils/PageManager/PageManager.h"
#include "Resource/ResourcePool.h"
#include "Pages/AppFactory.h"

static AppFactory factory;
static PageManager manager(&factory);

void App_Init(void)
{
    Resource.Init();

    manager.Install("Template",  "TemplatePage");
    manager.Install("LiveMap",   "LiveMapPage");
    manager.Install("Dialplate", "DialplatePage");
    manager.SetGlobalLoadAnimType(PageManager::LOAD_ANIM_OVER_TOP);
    
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    manager.Push("DialplatePage");
    
}
