#ifndef __DISPLAYPRIVATE_H
#define __DISPLAYPRIVATE_H

void Display_Init();

#ifdef ARDUINO

/*Basic*/
#include "Adafruit_ST7789/Adafruit_ST7789.h"
#include "Basic/SysConfig.h"
#include "lv_port/lv_port_indev.h"

typedef Adafruit_ST7789 SCREEN_CLASS;

void DisplayFault_Init(SCREEN_CLASS* scr);
void lv_port_disp_init(SCREEN_CLASS* scr);
void lv_fs_if_init(void);

#endif

#include "lvgl/lvgl.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#define MAP(x,in_min,in_max,out_min,out_max) \
    (((x)-(in_min))*((out_max)-(out_min))/((in_max)-(in_min))+(out_min))

#endif
