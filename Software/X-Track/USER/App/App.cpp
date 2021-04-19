#include "Utils/PageManager/PageManager.h"
#include "Resource/ResourcePool.h"
#include "Pages/AppFactory.h"
#include "Pages/StatusBar/StatusBar.h"
#include "Common/DataProc/DataProc.h"

static AppFactory factory;
static PageManager manager(&factory);

void App_Init(void)
{
    lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    Resource.Init();
    DataProc::Init();

    StatusBar_Init(lv_layer_top());

    manager.Install("Template",  "Pages/_Template");
    manager.Install("LiveMap",   "Pages/LiveMap");
    manager.Install("Dialplate", "Pages/Dialplate");
    manager.SetGlobalLoadAnimType(PageManager::LOAD_ANIM_OVER_TOP);
    manager.Push("Pages/Dialplate");

    StatusBar_AppearAnimStart();
}
