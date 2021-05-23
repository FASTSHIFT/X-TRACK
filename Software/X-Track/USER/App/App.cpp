#include "Utils/PageManager/PageManager.h"
#include "Common/DataProc/DataProc.h"
#include "Resource/ResourcePool.h"
#include "Pages/AppFactory.h"
#include "Pages/StatusBar/StatusBar.h"

static AppFactory factory;
static PageManager manager(&factory);

void App_Init(void)
{
    lv_obj_remove_style_all(lv_scr_act());
    lv_disp_set_bg_color(lv_disp_get_default(), lv_color_black());

    Resource.Init();
    DataProc_Init();

    StatusBar::Init(lv_layer_top());

    manager.Install("Template",  "Pages/_Template");
    manager.Install("LiveMap",   "Pages/LiveMap");
    manager.Install("Dialplate", "Pages/Dialplate");
    manager.Install("SystemInfos", "Pages/SystemInfos");
    
    manager.SetGlobalLoadAnimType(PageManager::LOAD_ANIM_OVER_TOP, 500);
    
    manager.Push("Pages/Dialplate");

    StatusBar::AppearAnimStart();
}
