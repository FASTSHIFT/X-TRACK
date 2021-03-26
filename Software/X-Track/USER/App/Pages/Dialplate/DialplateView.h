#ifndef __DIALPLATE_VIEW_H
#define __DIALPLATE_VIEW_H

#include "../Page.h"

class DialplateView {
public:
    struct {
        lv_obj_t* labelSpeed;
        lv_obj_t* labelUint;
    }ui;

    void Create(lv_obj_t* par);

private:
    
};

#endif // !__VIEW_H
