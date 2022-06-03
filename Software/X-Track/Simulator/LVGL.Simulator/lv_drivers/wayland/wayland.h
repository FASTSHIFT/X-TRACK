/**
 * @file wayland
 *
 */

#ifndef WAYLAND_H
#define WAYLAND_H

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

#if USE_WAYLAND

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#if LV_USE_USER_DATA == 0
#error "Support for user data is required by wayland driver. Set LV_USE_USER_DATA to 1 in lv_conf.h"
#endif


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef bool (*lv_wayland_display_close_f_t)(lv_disp_t * disp);

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_wayland_init(void);
void lv_wayland_deinit(void);
int lv_wayland_get_fd(void);
lv_disp_t * lv_wayland_create_window(lv_coord_t hor_res, lv_coord_t ver_res, char *title,
                                     lv_wayland_display_close_f_t close_cb);
void lv_wayland_close_window(lv_disp_t * disp);
bool lv_wayland_window_is_open(lv_disp_t * disp);
bool lv_wayland_window_is_flush_pending(lv_disp_t * disp);
void lv_wayland_window_set_fullscreen(lv_disp_t * disp, bool fullscreen);
lv_indev_t * lv_wayland_get_pointer(lv_disp_t * disp);
lv_indev_t * lv_wayland_get_pointeraxis(lv_disp_t * disp);
lv_indev_t * lv_wayland_get_keyboard(lv_disp_t * disp);
lv_indev_t * lv_wayland_get_touchscreen(lv_disp_t * disp);
#ifdef LV_WAYLAND_TIMER_HANDLER
uint32_t lv_wayland_timer_handler(void);
#endif

/**********************
 *      MACROS
 **********************/

#endif /* USE_WAYLAND */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WAYLAND_H */
