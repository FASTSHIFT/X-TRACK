#ifndef __START_UP_VIEW_H
#define __START_UP_VIEW_H

#include "../Page.h"

namespace Page
{

class StartUpView
{
public:
    void Create(lv_obj_t* root);

public:
    struct
    {
        lv_obj_t* labelLogo;
        lv_obj_t* bar;
    } ui;

private:
};

}

#endif // !__VIEW_H
