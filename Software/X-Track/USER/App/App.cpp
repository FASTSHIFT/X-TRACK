#include "Utils/PageManager/PageManager.h"
#include "Resource/ResourcePool.h"
#include "Pages/AppFactory.h"

static AppFactory factory;
static PageManager manager(&factory);

void App_Init(void)
{
    Resource.Init();

    manager.Install("Template", "TemplatePage");
    manager.Install("LiveMap", "LiveMapPage");

    manager.SetGlobalLoadAnimType(PageManager::LOAD_ANIM_OVER_TOP);
    manager.Push("LiveMapPage");
}
