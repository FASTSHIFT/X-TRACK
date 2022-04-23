#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#include "App.h"
#include "Common/HAL/HAL.h"
#include "lv_fs_if/lv_fs_if.h"

extern "C" {
    LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
}

static void hal_init(const char* evdev_path)
{
    /*Linux frame buffer device init*/
    fbdev_init();

    uint32_t width, height;
    fbdev_get_sizes(&width, &height);

    uint32_t disp_buf_size = width * height;
    lv_color_t* buf = (lv_color_t*)malloc(disp_buf_size * sizeof(lv_color_t));

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, disp_buf_size);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = width;
    disp_drv.ver_res    = height;
    lv_disp_drv_register(&disp_drv);

    /*Input device*/
    evdev_init();

    printf("evdev path: %s\n", evdev_path);
    evdev_set_file((char*)evdev_path);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;
    lv_indev_t* indev = lv_indev_drv_register(&indev_drv);

    lv_group_t* group = lv_group_create();
    lv_indev_set_group(indev, group);
    lv_group_set_default(group);

    lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
    lv_indev_set_cursor(indev, cursor_obj);             /*Connect the image  object to the driver*/
}

int main(int argc, const char* argv[])
{
    if(argc != 2)
    {
        printf("input event device path\n");
        return -1;
    }
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
