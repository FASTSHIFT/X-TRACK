/**
 * @file keyboard.h
 *
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

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

#if USE_KEYBOARD

#warning "Deprecated, use the SDL driver instead. See lv_drivers/sdl/sdl.c"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#if USE_SDL_GPU
#include "../sdl/sdl_gpu.h"
#else
#include "../sdl/sdl.h"
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

/**
 * Initialize the keyboard
 */
static inline void keyboard_init(void)
{
    /*Nothing to do*/
}

/**
 * Get the last pressed or released character from the PC's keyboard
 * @param indev_drv pointer to the related input device driver
 * @param data store the read data here
 */
static inline void keyboard_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    sdl_keyboard_read(indev_drv, data);
}


/**********************
 *      MACROS
 **********************/

#endif /*USE_KEYBOARD*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*KEYBOARD_H*/
