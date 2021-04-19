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

        lv_obj_t* btnBack;
        lv_obj_t* sliderLevel;

        struct {
            lv_obj_t* cont;

            lv_obj_t* labelSpeed;
            lv_obj_t* labelTrip;
            lv_obj_t* labelTime;
        }sportInfo;
    }ui;

    void Create(lv_obj_t* root);
    void SetImgArrowAngle(float angle)
    {
        lv_img_set_angle(ui.map.imgArrow, angle * 10);
    }

private:
    void Map_Create(lv_obj_t* par);
    void SportInfo_Create(lv_obj_t* par);
    lv_obj_t* ImgLabel_Create(lv_obj_t* par, const void* img_src, lv_coord_t x_ofs, lv_coord_t y_ofs);
};

#endif // !__VIEW_H
