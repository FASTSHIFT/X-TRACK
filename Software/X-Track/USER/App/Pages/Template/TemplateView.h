#ifndef __TEMPLATE_VIEW_H
#define __TEMPLATE_VIEW_H

#include "../Page.h"

class TemplateView {
public:
    struct {
        lv_obj_t* labelTitle;
        lv_obj_t* labelTick;
    }ui;

    void Create(lv_obj_t* par);

private:
    
};

#endif // !__VIEW_H
