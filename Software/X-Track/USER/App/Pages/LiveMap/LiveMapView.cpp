#include "LiveMapView.h"
#include <stdarg.h>
#include <stdio.h>

#define MAP_TILE_SIZE 256

void LiveMapView::Create(lv_obj_t* root)
{
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(root, lv_color_white(),0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);

    lv_obj_t* label = lv_label_create(root);
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, Resource.GetFont("bahnschrift_17"), 0);
    lv_label_set_text(label, "LOADING...");
    ui.labelInfo = label;

    Style_Create();
    Map_Create(root);
    ZoomCtrl_Create(root);
    SportInfo_Create(root);
}

void LiveMapView::Style_Create()
{
    static bool isInit = false;

    if (isInit)
    {
        return;
    }

    lv_style_init(&ui.styleNor);
    lv_style_set_bg_color(&ui.styleNor, lv_color_hex(0x1a1a1a));
    lv_style_set_bg_opa(&ui.styleNor, LV_OPA_60);
    lv_style_set_radius(&ui.styleNor, 6);

    lv_style_init(&ui.stylePr);
    lv_style_set_bg_opa(&ui.stylePr, LV_OPA_30);

    lv_style_init(&ui.styleFocus);
    lv_style_set_bg_color(&ui.styleFocus, lv_palette_main(LV_PALETTE_BLUE));

    lv_style_init(&ui.styleLabel);
    lv_style_set_text_font(&ui.styleLabel, Resource.GetFont("bahnschrift_17"));
    lv_style_set_text_color(&ui.styleLabel, lv_color_white());

    isInit = true;
}

void LiveMapView::Map_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, MAP_TILE_SIZE * 3, MAP_TILE_SIZE * 3);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 0);
    ui.map.cont = cont;

    for (int i = 0; i < ARRAY_SIZE(ui.map.imgMap); i++)
    {
        lv_obj_t* img = lv_img_create(cont);
        lv_obj_set_pos(img, i % 3 * MAP_TILE_SIZE, i / 3 * MAP_TILE_SIZE);
        lv_obj_set_size(img, MAP_TILE_SIZE, MAP_TILE_SIZE);

        ui.map.imgMap[i] = img;
    }

    lv_obj_t* img = lv_img_create(cont);
    lv_img_set_src(img, Resource.GetImage("gps_arrow"));
    ui.map.imgArrow = img;
}

void LiveMapView::ZoomCtrl_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_size(cont, 23, 100);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(
        cont,
        LV_FLEX_ALIGN_SPACE_EVENLY, 
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_SPACE_EVENLY
    );
    lv_obj_align(cont, LV_ALIGN_TOP_LEFT, 10, 30);
    ui.zoom.cont = cont;

    lv_obj_t* btn = lv_btn_create(cont);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, 23, 23);
    lv_obj_add_style(btn, &ui.styleNor, 0);
    lv_obj_add_style(btn, &ui.stylePr, LV_STATE_PRESSED);
    lv_obj_add_style(btn, &ui.styleFocus, LV_STATE_FOCUSED);
    lv_obj_t* label = lv_label_create(btn);
    lv_label_set_text(label, "i");
    lv_obj_add_style(label, &ui.styleLabel, 0);
    lv_obj_center(label);
    ui.zoom.btnInfo = btn;

    lv_obj_t* slider = lv_slider_create(cont);
    lv_slider_set_range(slider, 3, 15);
    lv_slider_set_value(slider, 15, LV_ANIM_OFF);
    lv_obj_remove_style_all(slider);
    lv_obj_set_size(slider, 10, 40);
    lv_obj_add_style(slider, &ui.styleNor, 0);
    
    lv_obj_set_style_bg_color(slider, lv_color_black(), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider, LV_OPA_70, LV_PART_KNOB);
    lv_obj_set_style_radius(slider, 5, LV_PART_KNOB);
    lv_obj_set_style_pad_hor(slider, 5, LV_PART_KNOB);

    lv_obj_add_style(slider, &ui.stylePr, LV_PART_KNOB | LV_STATE_PRESSED);
    lv_obj_add_style(slider, &ui.styleFocus, LV_PART_KNOB | LV_STATE_FOCUSED);

    ui.zoom.slider = slider;
}

void LiveMapView::SportInfo_Create(lv_obj_t* par)
{
    /* cont */
    lv_obj_t* obj = lv_obj_create(par);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 159, 66);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_LEFT, -10, 10);

    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_60, 0);
    lv_obj_set_style_radius(obj, 10, 0);
    lv_obj_set_style_shadow_width(obj, 10, 0);
    lv_obj_set_style_shadow_color(obj, lv_color_black(), 0);
    ui.sportInfo.cont = obj;

    /* speed */
    lv_obj_t* label = lv_label_create(obj);
    lv_label_set_text(label, "00");
    lv_obj_set_style_text_font(label, Resource.GetFont("bahnschrift_32"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 20, -10);
    ui.sportInfo.labelSpeed = label;

    label = lv_label_create(obj);
    lv_label_set_text(label, "km/h");
    lv_obj_set_style_text_font(label, Resource.GetFont("bahnschrift_13"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align_to(label, ui.sportInfo.labelSpeed, LV_ALIGN_OUT_BOTTOM_MID, 0, 3);

    ui.sportInfo.labelTrip = ImgLabel_Create(obj, Resource.GetImage("trip"), 5, 10);
    ui.sportInfo.labelTime = ImgLabel_Create(obj, Resource.GetImage("alarm"), 5, 30);
}

lv_obj_t* LiveMapView::ImgLabel_Create(lv_obj_t* par, const void* img_src, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    lv_obj_t* img = lv_img_create(par);
    lv_img_set_src(img, img_src);

    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, y_ofs);

    lv_obj_t* label = lv_label_create(par);
    lv_label_set_text(label, "--");
    lv_obj_add_style(label, &ui.styleLabel, 0);
    lv_obj_align_to(label, img, LV_ALIGN_OUT_RIGHT_MID, x_ofs, 0);
    return label;
}
