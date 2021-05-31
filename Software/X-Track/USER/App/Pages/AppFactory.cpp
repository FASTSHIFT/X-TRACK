#include "AppFactory.h"
#include "_Template/Template.h"
#include "LiveMap/LiveMap.h"
#include "Dialplate/Dialplate.h"
#include "SystemInfos/SystemInfos.h"
#include "StartUp/StartUp.h"

#define APP_CLASS_MATCH(className)\
do{\
    if (strcmp(name, #className) == 0)\
    {\
        return new className;\
    }\
}while(0)

PageBase* AppFactory::CreatePage(const char* name)
{
    APP_CLASS_MATCH(Template);
    APP_CLASS_MATCH(LiveMap);
    APP_CLASS_MATCH(Dialplate);
    APP_CLASS_MATCH(SystemInfos);
    APP_CLASS_MATCH(StartUp);

    return nullptr;
}
