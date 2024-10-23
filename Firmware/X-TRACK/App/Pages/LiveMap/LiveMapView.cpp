#include "LiveMapView.h"
#include "Config/Config.h"
#include <stdarg.h>
#include <stdio.h>

using namespace Page;

#if CONFIG_MAP_IMG_PNG_ENABLE
#include "Utils/lv_img_png/lv_img_png.h"
#  define TILE_IMG_CREATE  lv_img_png_create
#  define TILE_IMG_SET_SRC lv_img_png_set_src
#else
#  define TILE_IMG_CREATE  lv_img_create
#  define TILE_IMG_SET_SRC lv_img_set_src
#endif

void LiveMapView::Create(lv_obj_t* root, uint32_t tileNum)
{
    lv_obj_set_style_bg_color(root, lv_color_white(), 0);

    lv_obj_t* label = lv_label_create(root);
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("bahnschrift_17"), 0);
    lv_label_set_text(label, "LOADING...");
    ui.labelInfo = label;

    Style_Create();
    Map_Create(root, tileNum);
    ZoomCtrl_Create(root);
    SportInfo_Create(root);
}

void LiveMapView::Delete()
{
    if (ui.track.lineTrack)
    {
        delete ui.track.lineTrack;
        ui.track.lineTrack = nullptr;
    }

    if (ui.map.imgTiles)
    {
        lv_mem_free(ui.map.imgTiles);
        ui.map.imgTiles = nullptr;
    }

    lv_style_reset(&ui.styleCont);
    lv_style_reset(&ui.styleLabel);
    lv_style_reset(&ui.styleLine);
}

void LiveMapView::Style_Create()
{
    lv_style_init(&ui.styleCont);
    lv_style_set_bg_color(&ui.styleCont, lv_color_black());
    lv_style_set_bg_opa(&ui.styleCont, LV_OPA_60);
    lv_style_set_radius(&ui.styleCont, 6);
    lv_style_set_shadow_width(&ui.styleCont, 10);
    lv_style_set_shadow_color(&ui.styleCont, lv_color_black());

    lv_style_init(&ui.styleLabel);
    lv_style_set_text_font(&ui.styleLabel, ResourcePool::GetFont("bahnschrift_17"));
    lv_style_set_text_color(&ui.styleLabel, lv_color_white());

    lv_style_init(&ui.styleLine);
    lv_style_set_line_color(&ui.styleLine, lv_color_hex(0xff931e));
    lv_style_set_line_width(&ui.styleLine, 5);
    lv_style_set_line_opa(&ui.styleLine, LV_OPA_COVER);
    lv_style_set_line_rounded(&ui.styleLine, true);
}

void LiveMapView::Map_Create(lv_obj_t* par, uint32_t tileNum)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
#if CONFIG_LIVE_MAP_DEBUG_ENABLE
    lv_obj_set_style_outline_color(cont, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_outline_width(cont, 2, 0);
#endif
    ui.map.cont = cont;

    ui.map.imgTiles = (lv_obj_t**)lv_mem_alloc(tileNum * sizeof(lv_obj_t*));
    ui.map.tileNum = tileNum;

    for (uint32_t i = 0; i < tileNum; i++)
    {
        lv_obj_t* img = TILE_IMG_CREATE(cont);
        lv_obj_remove_style_all(img);
        ui.map.imgTiles[i] = img;
    }

    Track_Create(cont);

    lv_obj_t* img = lv_img_create(cont);
    lv_img_set_src(img, ResourcePool::GetImage("gps_arrow_dark"));

    lv_img_t* imgOri = (lv_img_t*)img;
    lv_obj_set_pos(img, -imgOri->w, -imgOri->h);
    ui.map.imgArrow = img;
}

void LiveMapView::SetMapTile(uint32_t tileSize, uint32_t widthCnt)
{
    uint32_t tileNum = ui.map.tileNum;

    lv_coord_t width = (lv_coord_t)(tileSize * widthCnt);
    lv_coord_t height = (lv_coord_t)(tileSize * (ui.map.tileNum / widthCnt));

    lv_obj_set_size(ui.map.cont, width, height);

    for (uint32_t i = 0; i < tileNum; i++)
    {
        lv_obj_t* img = ui.map.imgTiles[i];

        lv_obj_set_size(img, tileSize, tileSize);

        lv_coord_t x = (i % widthCnt) * tileSize;
        lv_coord_t y = (i / widthCnt) * tileSize;
        lv_obj_set_pos(img, x, y);
    }
}

void LiveMapView::SetMapTileSrc(uint32_t index, const char* src)
{
    if (index >= ui.map.tileNum)
    {
        return;
    }

    TILE_IMG_SET_SRC(ui.map.imgTiles[index], src);
}

void LiveMapView::SetArrowTheme(const char* theme)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "gps_arrow_%s", theme);

    const void* src = ResourcePool::GetImage(buf);

    if (src == nullptr)
    {
        ResourcePool::GetImage("gps_arrow_default");
    }

    lv_img_set_src(ui.map.imgArrow, src);
}

void LiveMapView::SetLineActivePoint(lv_coord_t x, lv_coord_t y)
{
    lv_point_t end_point;
    if (!ui.track.lineTrack->get_end_point(&end_point))
    {
        return;
    }

    ui.track.pointActive[0] = end_point;
    ui.track.pointActive[1].x = x;
    ui.track.pointActive[1].y = y;
    lv_line_set_points(ui.track.lineActive, ui.track.pointActive, 2);
}

void LiveMapView::ZoomCtrl_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_add_style(cont, &ui.styleCont, 0);
    lv_obj_set_style_opa(cont, LV_OPA_COVER, 0);
    lv_obj_set_size(cont, 50, 30);
    lv_obj_set_pos(cont, lv_obj_get_style_width(par, 0) - lv_obj_get_style_width(cont, 0) + 5, 40);
    ui.zoom.cont = cont;

    static const lv_style_prop_t prop[] =
    {
        LV_STYLE_X,
        LV_STYLE_OPA,
        LV_STYLE_PROP_INV
    };
    static lv_style_transition_dsc_t tran;
    lv_style_transition_dsc_init(&tran, prop, lv_anim_path_ease_out, 200, 0, nullptr);
    lv_obj_set_style_x(cont, lv_obj_get_style_width(par, 0), LV_STATE_USER_1);
    lv_obj_set_style_opa(cont, LV_OPA_TRANSP, LV_STATE_USER_1);
    lv_obj_set_style_transition(cont, &tran, LV_STATE_USER_1);
    lv_obj_set_style_transition(cont, &tran, LV_STATE_DEFAULT);
    lv_obj_add_state(cont, LV_STATE_USER_1);

    lv_obj_t* label = lv_label_create(cont);
    lv_obj_add_style(label, &ui.styleLabel, 0);
    lv_obj_align(label, LV_ALIGN_CENTER, -2, 0);
    lv_label_set_text(label, "--");
    ui.zoom.labelInfo = label;

    lv_obj_t* slider = lv_slider_create(cont);
    lv_obj_remove_style_all(slider);
    lv_slider_set_value(slider, 15, LV_ANIM_OFF);
    ui.zoom.slider = slider;
}

void LiveMapView::SportInfo_Create(lv_obj_t* par)
{
    /* cont */
    lv_obj_t* obj = lv_obj_create(par);
    lv_obj_remove_style_all(obj);
    lv_obj_add_style(obj, &ui.styleCont, 0);
    lv_obj_set_size(obj, 159, 66);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_LEFT, -10, 10);
    lv_obj_set_style_radius(obj, 10, 0);
    ui.sportInfo.cont = obj;

    /* speed */
    lv_obj_t* label = lv_label_create(obj);
    lv_label_set_text(label, "00");
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("bahnschrift_32"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 20, -10);
    ui.sportInfo.labelSpeed = label;

    label = lv_label_create(obj);
    lv_label_set_text(label, "km/h");
    lv_obj_set_style_text_font(label, ResourcePool::GetFont("bahnschrift_13"), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align_to(label, ui.sportInfo.labelSpeed, LV_ALIGN_OUT_BOTTOM_MID, 0, 3);

    ui.sportInfo.labelTrip = ImgLabel_Create(obj, ResourcePool::GetImage("trip"), 5, 10);
    ui.sportInfo.labelTime = ImgLabel_Create(obj, ResourcePool::GetImage("alarm"), 5, 30);
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

void LiveMapView::Track_Create(lv_obj_t* par)
{
    lv_obj_t* cont = lv_obj_create(par);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    ui.track.cont = cont;

    ui.track.lineTrack = new lv_poly_line(cont);

    ui.track.lineTrack->set_style(&ui.styleLine);

    lv_obj_t* line = lv_line_create(cont);
    lv_obj_remove_style_all(line);
    lv_obj_add_style(line, &ui.styleLine, 0);
#if CONFIG_LIVE_MAP_DEBUG_ENABLE
    lv_obj_set_style_line_color(line, lv_palette_main(LV_PALETTE_BLUE), 0);
#endif
    ui.track.lineActive = line;
}
