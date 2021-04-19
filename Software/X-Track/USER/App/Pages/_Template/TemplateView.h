#ifndef __TEMPLATE_VIEW_H
#define __TEMPLATE_VIEW_H

#include "../Page.h"

class TemplateView {
public:
    void Create(lv_obj_t* par);

public:
    struct {
        lv_obj_t* labelTitle;
        lv_obj_t* labelTick;
    }ui;

private:
    
};

#endif // !__VIEW_H
