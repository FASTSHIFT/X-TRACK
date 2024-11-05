/*
 * MIT License
 * Copyright (c) 2023 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file lv_port_sdl2.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

#if LV_USE_SDL

#include "lv_port.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/*********************
 *      DEFINES
 *********************/

#ifndef LV_SCREEN_HOR_RES
#define LV_SCREEN_HOR_RES 240
#endif

#ifndef LV_SCREEN_VER_RES
#define LV_SCREEN_VER_RES 320
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_cursor_init(lv_indev_t* indev, lv_coord_t size);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int lv_port_init(void)
{
    lv_disp_t* disp = lv_sdl_window_create(LV_SCREEN_HOR_RES, LV_SCREEN_VER_RES);
    lv_indev_t* mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_disp(mouse, disp);

#ifdef LV_INDEV_CURSOR_SIZE
    touchpad_cursor_init(mouse, LV_INDEV_CURSOR_SIZE);
    lv_indev_t* mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_disp(mousewheel, disp);
    lv_indev_set_group(mousewheel, lv_group_get_default());
#endif

    lv_indev_t* keyboard = lv_sdl_keyboard_create();
    lv_indev_set_disp(keyboard, disp);
    lv_indev_set_group(keyboard, lv_group_get_default());

    lv_port_profiler_init();

    return 0;
}

uint32_t lv_port_tick_get(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void lv_port_sleep(uint32_t ms)
{
    usleep(ms * 1000);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void touchpad_cursor_init(lv_indev_t* indev, lv_coord_t size)
{
    if (size <= 0) {
        return;
    }

    lv_obj_t* cursor = lv_obj_create(lv_layer_sys());
    lv_obj_remove_style_all(cursor);

    lv_obj_set_size(cursor, size, size);
    lv_obj_set_style_translate_x(cursor, -size / 2, 0);
    lv_obj_set_style_translate_y(cursor, -size / 2, 0);
    lv_obj_set_style_radius(cursor, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(cursor, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(cursor, lv_color_black(), 0);
    lv_obj_set_style_border_width(cursor, 2, 0);
    lv_obj_set_style_border_color(cursor, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_indev_set_cursor(indev, cursor);
}

#endif /*USE_SDL*/
