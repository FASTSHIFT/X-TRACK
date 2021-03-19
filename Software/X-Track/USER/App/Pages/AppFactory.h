#include "../utils/PageManager/PageManager.h"

class AppFactory : public PageFactory
{
public:
    virtual PageBase* CreatePage(const char* name);
private:

};

