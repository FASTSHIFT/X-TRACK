#ifndef __DIALPLATE_VIEW_H
#define __DIALPLATE_VIEW_H

#include "../Page.h"

class DialplateView {

public:
    typedef struct
    {
        lv_obj_t* cont;
        lv_obj_t* lableValue;
        lv_obj_t* lableUnit;
    }SubInfo_t;

public:
    struct {
        struct {
            lv_obj_t* cont;
            lv_obj_t* labelSpeed;
            lv_obj_t* labelUint;
        }topInfo;
        
        struct {
            lv_obj_t* cont;
            SubInfo_t infoGrp[4];
            lv_obj_t* btnGrp[3];
        }bottomInfo;
    }ui;

    void Create(lv_obj_t* root);
    void AppearAnimStart(bool playback = false);

private:
    void TopInfo_Create(lv_obj_t* par);
    void BottomInfo_Create(lv_obj_t* par);
    void SubInfoGrp_Create(lv_obj_t* par, SubInfo_t* info, const char* unitText);
    lv_obj_t* Btn_Create(lv_obj_t* par, lv_color_t color, const void* img_src);
};

#endif // !__VIEW_H
