/**
 * @file lv_toast.h
 *
 */

#ifndef LV_TOAST_H
#define LV_TOAST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl/lvgl.h"
#include "lvgl/src/lvgl_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t obj;
    lv_style_t* style;
} lv_toast_t;

extern const lv_obj_class_t lv_toast_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t* lv_toast_create(lv_obj_t* parent);

void lv_toast_show_text(lv_obj_t* obj, const char* txt, uint32_t duration);

void lv_toast_set_cont_style(lv_obj_t* obj, lv_style_t* style);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TOAST_H*/
