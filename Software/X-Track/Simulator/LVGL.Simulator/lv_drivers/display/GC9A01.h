/**
 * @file GC9A01.h
 *
 **/

#ifndef GC9A01_H
#define GC9A01_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifndef LV_DRV_NO_CONF
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_drv_conf.h"
#else
#include "../../lv_drv_conf.h"
#endif
#endif

#if USE_GC9A01

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

// #include "../../lv_drv_conf.h"
// #if USE_GC9A01 != 0

// #include <stdint.h>
// #include "lvgl/lv_misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Init script function */
struct GC9A01_function {
	uint16_t cmd;
	uint16_t data;
};

/* Init script commands */
enum GC9A01_cmd {
	GC9A01_START,
	GC9A01_END,
	GC9A01_CMD,
	GC9A01_DATA,
	GC9A01_DELAY
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

int GC9A01_init(void);
void GC9A01_flush(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void GC9A01_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color);
void GC9A01_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t * color_p);
void GC9A01_setRotation(uint8_t m);
void GC9A01_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void GC9A01_fillScreen(uint16_t color);
uint16_t GC9A01_Color565(uint8_t r, uint8_t g, uint8_t b);
void GC9A01_invertDisplay(bool i);
void GC9A01_drawPixel(int16_t x, int16_t y, uint16_t color);
void GC9A01_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void GC9A01_drawFastVLine(int16_t x, int16_t y, int16_t w, uint16_t color);

/**********************
 *      MACROS
 **********************/

#endif /* USE_GC9A01 */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GC9A01_H */