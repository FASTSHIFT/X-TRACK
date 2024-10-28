/*
 * MIT License
 * Copyright (c) 2024 _VIFEXTech
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
#include "App/App.h"
#include "HAL/HAL.h"
#include "lvgl/lvgl.h"
#include <unistd.h>

#ifndef LV_SCREEN_HOR_RES
#define LV_SCREEN_HOR_RES 240
#endif

#ifndef LV_SCREEN_VER_RES
#define LV_SCREEN_VER_RES 240
#endif

/**
 * Initialize the Hardware Abstraction Layer (HAL) forLVGL
 */
static lv_disp_t* hal_init(int32_t w, int32_t h)
{
    lv_disp_t* disp = lv_sdl_window_create(w, h);
    lv_indev_t* mouse = lv_sdl_mouse_create();
    // lv_display_set_rotation(disp, LV_DISP_ROTATION_90);
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_disp(mouse, disp);

    lv_indev_t* mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_disp(mousewheel, disp);
    lv_indev_set_group(mousewheel, lv_group_get_default());

    lv_indev_t* keyboard = lv_sdl_keyboard_create();
    lv_indev_set_disp(keyboard, disp);
    lv_indev_set_group(keyboard, lv_group_get_default());

    return disp;
}

/**
 * @brief  Main Function
 * @param  argc: Argument count
 * @param  argv: Argument vector
 * @retval error code
 */
int main(int argc, const char* argv[])
{
    // HAL::HAL_Init();
    lv_init();
    hal_init(LV_SCREEN_HOR_RES, LV_SCREEN_VER_RES);
    // App_Init();
    while (1) {
        // HAL::HAL_Update();
        uint32_t time_until_next = lv_timer_handler();
        usleep(time_until_next * 1000);
    }

    lv_deinit();
}
