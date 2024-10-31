
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include "lv_drivers/sdl/sdl.h"

#include "App.h"
#include "Common/HAL/HAL.h"
#include "lv_fs_if/lv_fs_if.h"

extern "C" {
    LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
}

static void hal_init(const char* evdev_path)
{
    /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
    sdl_init();

    /*Create a display buffer*/
    const uint32_t disp_buf_size = LV_HOR_RES * LV_VER_RES;
    lv_color_t* buf = (lv_color_t*)malloc(disp_buf_size * sizeof(lv_color_t));

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, disp_buf_size);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = sdl_display_flush;
    disp_drv.hor_res    = LV_HOR_RES;
    disp_drv.ver_res    = LV_VER_RES;
    lv_disp_drv_register(&disp_drv);

    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);

    /* Add the mouse as input device
    * Use the 'mouse' driver which reads the PC's mouse*/
   static lv_indev_drv_t indev_drv_1;
   lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
   indev_drv_1.type = LV_INDEV_TYPE_POINTER;

   /*This function will be called periodically (by the library) to get the mouse position and state*/
   indev_drv_1.read_cb = sdl_mouse_read;
   lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

   static lv_indev_drv_t indev_drv_2;
   lv_indev_drv_init(&indev_drv_2); /*Basic initialization*/
   indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
   indev_drv_2.read_cb = sdl_keyboard_read;
   lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
   lv_indev_set_group(kb_indev, g);

   static lv_indev_drv_t indev_drv_3;
   lv_indev_drv_init(&indev_drv_3); /*Basic initialization*/
   indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
   indev_drv_3.read_cb = sdl_mousewheel_read;
   lv_indev_t * enc_indev = lv_indev_drv_register(&indev_drv_3);
   lv_indev_set_group(enc_indev, g);

   /*Set a cursor for the mouse*/
   LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
   lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
   lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
   lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/         /*Connect the image  object to the driver*/
}

int main(int argc, const char* argv[])
{
    /*LittlevGL init*/
    lv_init();

    lv_fs_if_init();

    hal_init(argv[1]);

    HAL::HAL_Init();

    App_Init();

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_task_handler();
        HAL::HAL_Update();
        usleep(1000);
    }

    App_Uninit();

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
extern "C" uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
