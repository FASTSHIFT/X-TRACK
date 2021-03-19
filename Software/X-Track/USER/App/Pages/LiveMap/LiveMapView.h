#ifndef __LIVEMAP_VIEW_H
#define __LIVEMAP_VIEW_H

#include "../Page.h"

class LiveMapView {
public:
    struct {
        struct {
            lv_obj_t* cont;
            lv_obj_t* imgMap[9];
            lv_obj_t* imgArrow;
        }map;

        lv_obj_t* labelInfo;
        lv_obj_t* sliderLevel;
    }ui;

    void Create(lv_obj_t* par);

private:
    
};

#endif // !__VIEW_H
