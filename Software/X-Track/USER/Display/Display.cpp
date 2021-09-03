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
#include "Display.h"
#include "HAL/HAL.h"

#define USE_FPS_TEST 0

#if USE_FPS_TEST
static float Display_GetFPS(SCREEN_CLASS* scr, uint32_t loopNum);
#endif

/**
  * @brief  显示初始化
  * @param  无
  * @retval 无
  */
void Display_Init()
{
    static SCREEN_CLASS screen(
        CONFIG_SCREEN_CS_PIN,
        CONFIG_SCREEN_DC_PIN,
        CONFIG_SCREEN_RST_PIN
    );

    /* 屏幕初始化 */
    screen.begin();
    screen.setRotation(0);
    screen.fillScreen(screen.Black);

#if (USE_FPS_TEST == 1)
    Display_GetFPS(&screen, 50);
    while(1);
#endif

    /* 自动错误诊断初始化 */
    DisplayFault_Init(&screen);

    /* lvgl初始化 */
    lv_init();
    lv_port_disp_init(&screen);
    lv_port_indev_init();
    lv_fs_if_init();

    /* 背光渐亮 */
    HAL::Backlight_SetGradual(1000, 1000);
}

#if (USE_FPS_TEST == 1)
static float Display_GetFPS(SCREEN_CLASS* scr, uint32_t loopNum)
{
    uint32_t startTime = millis();
    for(uint32_t f = 0; f < loopNum; f++)
    {
        scr->fillScreen(SCREEN_CLASS::Red);
        scr->fillScreen(SCREEN_CLASS::Green);
        scr->fillScreen(SCREEN_CLASS::Blue);
    }
    uint32_t costTime = millis() - startTime;
    float fps = loopNum * 3 / (costTime / 1000.0f);

    scr->fillScreen(SCREEN_CLASS::Blue);
    scr->setTextSize(1);
    scr->setTextColor(SCREEN_CLASS::White);
    scr->setCursor(0, scr->height() / 2);

    scr->print("Frame:");
    scr->println(loopNum * 3);

    scr->print("Time:");
    scr->print(costTime);
    scr->println("ms");

    scr->print("FPS:");
    scr->println(fps);

    return fps;
}
#endif
