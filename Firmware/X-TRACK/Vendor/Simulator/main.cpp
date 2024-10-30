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
#include "lv_port/lv_port.h"
#include "lvgl/lvgl.h"
#include <unistd.h>

/**
 * @brief  Main Function
 * @param  argc: Argument count
 * @param  argv: Argument vector
 * @retval error code
 */
int main(int argc, const char* argv[])
{
    lv_init();
    HAL::Init();

    if (lv_port_init() < 0) {
        LV_LOG_USER("hal init failed");
        return -1;
    }

    AppContext_t* appCtx = App_CreateContext(argc, argv);
    LV_LOG_USER("App context created: %p", appCtx);

    while (1) {
        /* Run app loop */
        uint32_t app_idle_time = App_RunLoopExecute(appCtx);

        /* Run lvgl loop */
        uint32_t lvgl_idle_time = lv_timer_handler();

        /* Sleep the idle time */
        lv_port_sleep(LV_MIN(app_idle_time, lvgl_idle_time));
    }

    App_DestroyContext(appCtx);
    lv_deinit();
}
