/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
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
#include "Config/Config.h"
#include "Common/DataProc/DataProc.h"
#include "Resource/ResourcePool.h"
#include "Pages/AppFactory.h"
#include "Pages/StatusBar/StatusBar.h"
#include "Utils/PageManager/PageManager.h"

#if CONFIG_MAP_PNG_DECODE_ENABLE
#  include "Utils/lv_lib_png/lv_png.h"
#endif

#if CONFIG_MONKEY_TEST_ENABLE
#  include "Utils/lv_monkey/lv_monkey.h"
#endif

#define ACCOUNT_SEND_CMD(ACT, CMD)\
do{\
    DataProc::ACT##_Info_t info;\
    memset(&info, 0, sizeof(info));\
    info.cmd = DataProc::CMD;\
    DataProc::Center()->AccountMain.Notify(#ACT, &info, sizeof(info));\
}while(0)

void App_Init()
{
    static AppFactory factory;
    static PageManager manager(&factory);

#if CONFIG_MAP_PNG_DECODE_ENABLE
    lv_png_init();
#endif

#if CONFIG_MONKEY_TEST_ENABLE
    lv_monkey_config_t config;
    lv_monkey_config_init(&config);
    config.type = CONFIG_MONKEY_INDEV_TYPE;
    config.time.min = CONFIG_MONKEY_TIME_MIN;
    config.time.max = CONFIG_MONKEY_TIME_MAX;
    config.input_range.min = CONFIG_MONKEY_INPUT_RANGE_MIN;
    config.input_range.max = CONFIG_MONKEY_INPUT_RANGE_MAX;
    lv_monkey_create(&config);
    LV_LOG_WARN("lv_monkey test started!");
#endif

    DataProc_Init();

    ACCOUNT_SEND_CMD(Storage, STORAGE_CMD_LOAD);
    ACCOUNT_SEND_CMD(SysConfig, SYSCONFIG_CMD_LOAD);

    lv_obj_t* scr = lv_scr_act();
    lv_obj_remove_style_all(scr);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_disp_set_bg_color(lv_disp_get_default(), lv_color_black());

    ResourcePool::Init();

    StatusBar::Init(lv_layer_top());

    manager.Install("Template",    "Pages/_Template");
    manager.Install("LiveMap",     "Pages/LiveMap");
    manager.Install("Dialplate",   "Pages/Dialplate");
    manager.Install("SystemInfos", "Pages/SystemInfos");
    manager.Install("Startup",     "Pages/Startup");

    manager.SetGlobalLoadAnimType(PageManager::LOAD_ANIM_OVER_TOP, 500);

    manager.Push("Pages/Startup");
}

void App_Uninit()
{
    ACCOUNT_SEND_CMD(SysConfig, SYSCONFIG_CMD_SAVE);
    ACCOUNT_SEND_CMD(Storage,   STORAGE_CMD_SAVE);
    ACCOUNT_SEND_CMD(Recorder,  RECORDER_CMD_STOP);
}
