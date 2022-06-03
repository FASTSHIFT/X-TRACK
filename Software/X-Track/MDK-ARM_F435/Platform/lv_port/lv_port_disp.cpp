#include "lv_port/lv_port.h"
#include "lvgl/lvgl.h"
#include "HAL/HAL.h"

#define SCREEN_BUFFER_SIZE (CONFIG_SCREEN_HOR_RES * CONFIG_SCREEN_VER_RES)

static lv_disp_drv_t* disp_drv_p = NULL;

static void disp_flush_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
    disp_drv_p = disp;

    const lv_coord_t w = (area->x2 - area->x1 + 1);
    const lv_coord_t h = (area->y2 - area->y1 + 1);
    const uint32_t len = w * h;

    HAL::Display_SetAddrWindow(area->x1, area->y1, area->x2, area->y2);

    HAL::Display_SendPixels((uint16_t*)color_p, len);
}

static void disp_send_finish_callback()
{
    lv_disp_flush_ready(disp_drv_p);
}

static void disp_wait_cb(lv_disp_drv_t* disp_drv)
{
    __wfi();
}

void lv_port_disp_init()
{
    HAL::Display_SetSendFinishCallback(disp_send_finish_callback);

    static lv_color_t lv_disp_buf1[SCREEN_BUFFER_SIZE];
    static lv_color_t lv_disp_buf2[SCREEN_BUFFER_SIZE];

    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, lv_disp_buf1, lv_disp_buf2, SCREEN_BUFFER_SIZE);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = CONFIG_SCREEN_HOR_RES;
    disp_drv.ver_res = CONFIG_SCREEN_VER_RES;
    disp_drv.flush_cb = disp_flush_cb;
    disp_drv.wait_cb = disp_wait_cb;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);
}
