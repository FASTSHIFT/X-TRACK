#ifndef __LIVEMAP_VIEW_H
#define __LIVEMAP_VIEW_H

#include "../Page.h"
#include <vector>
#include "Utils/lv_allocator/lv_allocator.h"

namespace Page
{

class LiveMapView
{
public:
    struct
    {
        lv_obj_t* labelInfo;

        lv_style_t styleCont;
        lv_style_t styleLabel;
        lv_style_t styleLine;

        struct
        {
            lv_obj_t* cont;
            lv_obj_t* imgArrow;

            lv_obj_t** imgTiles;
            uint32_t tileNum;

            lv_obj_t* lineTrack;
            lv_obj_t* lineActive;
            lv_point_t pointActive[2];
            std::vector<lv_point_t, lv_allocator<lv_point_t>> trackPoints;
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

    void Create(lv_obj_t* root, uint32_t tileNum);
    void Delete();
    void SetImgArrowState(lv_coord_t x, lv_coord_t y, float angle)
    {
        lv_obj_t* img = ui.map.imgArrow;
        lv_obj_set_pos(img, x, y);
        lv_img_set_angle(img, int16_t(angle * 10));
    }
    void SetMapTile(uint32_t tileSize, uint32_t widthCnt);
    void SetArrowTheme(const char* theme);
    void TrackAddPoint(lv_coord_t x, lv_coord_t y);
    void TrackReset();
    void TrackSetActivePoint(lv_coord_t x, lv_coord_t y);

private:
    void Style_Create();
    void Map_Create(lv_obj_t* par, uint32_t tileNum);
    void ZoomCtrl_Create(lv_obj_t* par);
    void SportInfo_Create(lv_obj_t* par);
    lv_obj_t* ImgLabel_Create(lv_obj_t* par, const void* img_src, lv_coord_t x_ofs, lv_coord_t y_ofs);
    void TrackLine_Create(lv_obj_t* par);
};

}

#endif // !__VIEW_H
