#ifndef __LIVEMAP_VIEW_H
#define __LIVEMAP_VIEW_H

#include "../Page.h"

class LiveMapView
{
public:
    struct
    {
        lv_obj_t* labelInfo;

        lv_style_t styleCont;
        lv_style_t styleLabel;

        struct
        {
            lv_obj_t* cont;
            lv_obj_t* imgMap[9];
            lv_obj_t* imgArrow;
        } map;

        struct
        {
            lv_obj_t* cont;
            lv_obj_t* labelInfo;
            lv_obj_t* slider;
        } zoom;

        struct
        {
            lv_obj_t* cont;
        } move;

        struct
        {
            lv_obj_t* cont;

            lv_obj_t* labelSpeed;
            lv_obj_t* labelTrip;
            lv_obj_t* labelTime;
        } sportInfo;
    } ui;

    void Create(lv_obj_t* root);
    void SetImgArrowAngle(float angle)
    {
        lv_img_set_angle(ui.map.imgArrow, int16_t(angle * 10));
    }

private:
    void Style_Create();
    void Map_Create(lv_obj_t* par);
    void ZoomCtrl_Create(lv_obj_t* par);
    void SportInfo_Create(lv_obj_t* par);
    lv_obj_t* ImgLabel_Create(lv_obj_t* par, const void* img_src, lv_coord_t x_ofs, lv_coord_t y_ofs);
};

#endif // !__VIEW_H
