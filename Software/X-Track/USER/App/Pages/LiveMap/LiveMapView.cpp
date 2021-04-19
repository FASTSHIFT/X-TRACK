#include "LiveMapView.h"
#include <stdarg.h>
#include <stdio.h>

#define MAP_TILE_SIZE 256

void LiveMapView::Create(lv_obj_t* root)
{
    lv_obj_reset_style_list(root, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_color(root, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_bg_opa(root, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_value_str(root, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "LOADING...");

    Map_Create(root);
    SportInfo_Create(root);
}

void LiveMapView::Map_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_cont_create(par, nullptr);
    lv_obj_set_size(cont, MAP_TILE_SIZE * 3, MAP_TILE_SIZE * 3);
    lv_obj_set_drag(cont, true);
    lv_obj_align(cont, nullptr, LV_ALIGN_CENTER, 0, 0);
    ui.map.cont = cont;

    for (int i = 0; i < ARRAY_SIZE(ui.map.imgMap); i++)
    {
        lv_obj_t* img = lv_img_create(cont, nullptr);
        lv_img_set_auto_size(img, false);
        lv_obj_set_pos(img, i % 3 * MAP_TILE_SIZE, i / 3 * MAP_TILE_SIZE);
        lv_obj_set_size(img, MAP_TILE_SIZE, MAP_TILE_SIZE);
        lv_obj_set_drag_parent(img, true);

        ui.map.imgMap[i] = img;
    }

    lv_obj_t* img = lv_img_create(cont, nullptr);
    lv_img_set_src(img, Resource.GetImage("gps_arrow"));
    ui.map.imgArrow = img;

    lv_obj_t* slider = lv_slider_create(par, nullptr);
    lv_obj_set_size(slider, 5, 100);
    lv_slider_set_range(slider, 3, 15);
    lv_obj_align(slider, nullptr, LV_ALIGN_IN_LEFT_MID, 10, 0);
    ui.sliderLevel = slider;

    lv_obj_t* btn = lv_btn_create(par, nullptr);
    lv_obj_set_size(btn, 30, 20);
    lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_DOWN);
    lv_obj_align(btn, nullptr, LV_ALIGN_IN_BOTTOM_RIGHT, -5, -5);
    ui.btnBack = btn;
}

void LiveMapView::SportInfo_Create(lv_obj_t* par)
{
    /* cont */
    lv_obj_t* obj = lv_obj_create(par, nullptr);
    lv_obj_reset_style_list(obj, LV_OBJ_PART_MAIN);
    lv_obj_set_size(obj, 159, 66);
    lv_obj_align(obj, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, -10, 10);

    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_60);
    lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_shadow_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_shadow_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    ui.sportInfo.cont = obj;

    /* speed */
    lv_obj_t* label = lv_label_create(obj, nullptr);
    lv_label_set_text(label, "00");
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Resource.GetFont("bahnschrift_32"));
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(label, nullptr, LV_ALIGN_IN_LEFT_MID, 20, -10);
    ui.sportInfo.labelSpeed = label;

    label = lv_label_create(obj, nullptr);
    lv_label_set_text(label, "km/h");
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Resource.GetFont("bahnschrift_13"));
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(label, ui.sportInfo.labelSpeed, LV_ALIGN_OUT_BOTTOM_MID, 0, 3);

    ui.sportInfo.labelTrip = ImgLabel_Create(obj, Resource.GetImage("trip"), 5, 10);
    ui.sportInfo.labelTime = ImgLabel_Create(obj, Resource.GetImage("alarm"), 5, 30);
}

lv_obj_t* LiveMapView::ImgLabel_Create(lv_obj_t* par, const void* img_src, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    lv_obj_t* img = lv_img_create(par, nullptr);
    lv_img_set_src(img, img_src);

    lv_obj_align(img, nullptr, LV_ALIGN_IN_TOP_MID, 0, y_ofs);

    lv_obj_t* label = lv_label_create(par, nullptr);
    lv_label_set_text(label, "--");
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Resource.GetFont("bahnschrift_17"));
    lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_align(label, img, LV_ALIGN_OUT_RIGHT_MID, x_ofs, 0);
    return label;
}
