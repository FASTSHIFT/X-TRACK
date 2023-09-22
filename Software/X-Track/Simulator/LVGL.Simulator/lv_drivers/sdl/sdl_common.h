/**
 * @file sdl_common.h
 *
 */

#ifndef SDL_COMMON_H
#define SDL_COMMON_H

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

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#if USE_MONITOR || USE_SDL

#ifndef SDL_INCLUDE_PATH
#define SDL_INCLUDE_PATH MONITOR_SDL_INCLUDE_PATH
#endif
#include SDL_INCLUDE_PATH

#ifndef SDL_ZOOM
#define SDL_ZOOM MONITOR_ZOOM
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern volatile bool sdl_quit_qry;

/**
 * Initialize SDL to be used as display, mouse and mouse wheel drivers.
 */
void sdl_init(void);

/**
 * Flush a buffer to the marked area
 * @param drv pointer to driver where this function belongs
 * @param area an area where to copy `color_p`
 * @param color_p an array of pixel to copy to the `area` part of the screen
 */
void sdl_display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

/**
 * Get the current position and state of the mouse
 * @param indev_drv pointer to the related input device driver
 * @param data store the mouse data here
 */
void sdl_mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

/**
 * Get encoder (i.e. mouse wheel) ticks difference and pressed state
 * @param indev_drv pointer to the related input device driver
 * @param data store the read data here
 */
void sdl_mousewheel_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

/**
 * Get input from the keyboard.
 * @param indev_drv pointer to the related input device driver
 * @param data store the red data here
 */
void sdl_keyboard_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

int quit_filter(void * userdata, SDL_Event * event);

void mouse_handler(SDL_Event * event);
void mousewheel_handler(SDL_Event * event);
uint32_t keycode_to_ctrl_key(SDL_Keycode sdl_key);
void keyboard_handler(SDL_Event * event);

/**********************
 *      MACROS
 **********************/

#endif /* USE_MONITOR || USE_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SDL_COMMON_H */
