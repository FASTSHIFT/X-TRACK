#include "lvgl/demos/lv_demos.h"

static void setup()
{
    Serial.begin(115200);
    HAL::Power_Init();
    HAL::Display_Init();
    HAL::Backlight_ForceLit(true);

    lv_init();
    lv_port_disp_init();

//    lv_demo_benchmark_set_max_speed(true);
    lv_demo_benchmark();
}

static void loop()
{
    lv_task_handler();
}
