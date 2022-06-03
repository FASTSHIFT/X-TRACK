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
#include <stdio.h>
#include <assert.h>
#include "resource.h"
#include "App.h"
#include "Common/HAL/HAL.h"

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
#include "lv_drivers/win32drv/win32drv.h"
#include "lv_fs_if/lv_fs_if.h"

#if _MSC_VER >= 1200
// Restore compilation warnings.
#pragma warning(pop)
#endif

#define SCREEN_HOR_RES  240
#define SCREEN_VER_RES  240

#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main()
{
    lv_init();

    lv_fs_if_init();

    if (!lv_win32_init(
        GetModuleHandleW(NULL),
        SW_SHOW,
        SCREEN_HOR_RES,
        SCREEN_VER_RES,
        LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_LVGL))))
    {
        return -1;
    }

    lv_win32_add_all_input_devices_to_group(NULL);

    HAL::HAL_Init();  

    App_Init();

    while (!lv_win32_quit_signal)
    {
        lv_timer_handler();
        HAL::HAL_Update();
        Sleep(1);
    }

    App_Uninit();

    return 0;
}
