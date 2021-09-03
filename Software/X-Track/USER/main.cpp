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
#include "Arduino.h"
#include "Display/Display.h"
#include "HAL/HAL.h"
#include "App/App.h"
#include "Config/Config.h"
#include "Utils/lv_monkey/lv_monkey.h"
#include "Utils/lv_lib_png/lv_png.h"

static void setup()
{
    HAL::HAL_Init();
    Display_Init();
    
#if CONFIG_MAP_PNG_DECODE_ENABLE
    lv_png_init();
#endif

#if CONFIG_MONKEY_TEST_ENABLE
    lv_monkey_create(
        CONFIG_MONKEY_INDEV_TYPE,
        CONFIG_MONKEY_INTERVAL_TIME_MIN,
        CONFIG_MONKEY_INTERVAL_TIME_MAX
    );
#endif

    App_Init();

    HAL::Power_SetEventCallback(App_Uninit);
}

static void loop()
{
    HAL::HAL_Update();
    lv_task_handler();
    __wfi();
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    GPIO_JTAG_Disable();
    Delay_Init();
    setup();
    for(;;)loop();
}
