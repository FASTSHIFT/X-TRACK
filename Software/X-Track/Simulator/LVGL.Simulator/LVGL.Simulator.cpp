/*
 * PROJECT:   LVGL ported to Windows Desktop
 * FILE:      LVGL.Windows.Desktop.cpp
 * PURPOSE:   Implementation for LVGL ported to Windows Desktop
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include <Windows.h>

#include "resource.h"

#include "App.h"
#include "Common/HAL/HAL.h"

#include <stdio.h>

#if _MSC_VER >= 1200
 // Disable compilation warnings.
#pragma warning(push)
// nonstandard extension used : bit field types other than int
#pragma warning(disable:4214)
// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable:4244)
#endif

#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lv_examples/lv_demo.h"
#include "lv_drivers/win32drv/win32drv.h"
#include "lv_fs_if/lv_fs_if.h"

#if _MSC_VER >= 1200
// Restore compilation warnings.
#pragma warning(pop)
#endif

int main()
{
    lv_init();

    lv_fs_if_init();

    if (!lv_win32_init(
        GetModuleHandleW(NULL),
        SW_SHOW,
        240,
        240,
        LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_LVGL))))
    {
        return -1;
    }

    HAL::HAL_Init();

    App_Init();

    /*
     * Demos, benchmarks, and tests.
     *
     * Uncomment any one (and only one) of the functions below to run that
     * item.
     */

    //lv_example_scroll_2();
    //lv_demo_keypad_encoder();
    //lv_example_flex_3();
    //lv_demo_widgets();
    //lv_demo_benchmark();
    //lv_demo_music();
    //lv_demo_keypad_encoder();
    //lv_demo_printer();
    //lv_demo_stress();
    //lv_ex_get_started_1();
    //lv_ex_get_started_2();
    //lv_ex_get_started_3();

    //lv_ex_style_1();
    //lv_ex_style_2();
    //lv_ex_style_3();
    //lv_ex_style_4();
    //lv_ex_style_5();
    //lv_ex_style_6();
    //lv_ex_style_7();
    //lv_ex_style_8();
    //lv_ex_style_9();
    //lv_ex_style_10();
    //lv_ex_style_11();

    /*
     * There are many examples of individual widgets found under the
     * lv_examples/src/lv_ex_widgets directory.  Here are a few sample test
     * functions.  Look in that directory to find all the rest.
     */
     //lv_ex_arc_1();
     //lv_ex_cpicker_1();
     //lv_ex_gauge_1();
     //lv_ex_img_1();
     //lv_ex_tileview_1();

    while (!lv_win32_quit_signal)
    {
        lv_timer_handler();
        HAL::HAL_Update();
        Sleep(1);
    }

    App_Uninit();

    return 0;
}
