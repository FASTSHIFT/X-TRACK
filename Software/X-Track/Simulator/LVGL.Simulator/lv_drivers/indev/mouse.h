/**
 * @file mouse.h
 *
 */

#ifndef MOUSE_H
#define MOUSE_H

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

#if USE_MOUSE

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
 * Initialize the mouse
 */
static inline void mouse_init(void)
{
    /*Nothing to do*/
}

/**
 * Get the current position and state of the mouse
 * @param indev_drv pointer to the related input device driver
 * @param data store the mouse data here
 */
void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    sdl_mouse_read(indev_drv, data);
}

/**********************
 *      MACROS
 **********************/

#endif /* USE_MOUSE */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MOUSE_H */
