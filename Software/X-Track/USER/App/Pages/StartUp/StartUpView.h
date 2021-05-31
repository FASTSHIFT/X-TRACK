#ifndef __START_UP_VIEW_H
#define __START_UP_VIEW_H

#include "../Page.h"

class StartUpView {
public:
    void Create(lv_obj_t* root);

public:
    struct {
        lv_obj_t* labelLogo;
        lv_obj_t* bar;
    }ui;

private:
    static void Bar_SetValue(lv_obj_t* bar, int32_t value);
};

#endif // !__VIEW_H
