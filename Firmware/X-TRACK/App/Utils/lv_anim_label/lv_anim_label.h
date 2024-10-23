/**
 * @file lv_anim_label.h
 *
 */

#ifndef LV_ANIM_LABEL_H
#define LV_ANIM_LABEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

typedef struct {
    lv_obj_t obj;
    lv_obj_t * label_1;
    lv_obj_t * label_2;
    lv_obj_t * label_act;
    lv_dir_t enter_dir;
    lv_dir_t exit_dir;
    lv_anim_t a_enter;
    lv_anim_t a_exit;
    uint32_t duration;
    lv_anim_path_cb_t path_cb;
}lv_anim_label_t;

extern const lv_obj_class_t lv_anim_label_class;

lv_obj_t * lv_anim_label_create(lv_obj_t * parent);

void lv_anim_label_set_dir(lv_obj_t * obj, lv_dir_t dir);

void lv_anim_label_set_enter_dir(lv_obj_t * obj, lv_dir_t dir);

void lv_anim_label_set_exit_dir(lv_obj_t * obj, lv_dir_t dir);

void lv_anim_label_set_time(lv_obj_t * obj, uint32_t duration);

void lv_anim_label_set_path(lv_obj_t * obj, lv_anim_path_cb_t path_cb);

void lv_anim_label_add_style(lv_obj_t * obj, lv_style_t * style);

void lv_anim_label_set_custom_enter_anim(lv_obj_t * obj, const lv_anim_t * a);

void lv_anim_label_set_custom_exit_anim(lv_obj_t * obj, const lv_anim_t * a);

void lv_anim_label_push_text(lv_obj_t * obj, const char* txt);

lv_dir_t lv_anim_label_get_enter_dir(lv_obj_t * obj);

lv_dir_t lv_anim_label_get_exit_dir(lv_obj_t * obj);

uint32_t lv_anim_label_get_time(lv_obj_t * obj);

lv_anim_path_cb_t lv_anim_label_get_path(lv_obj_t * obj);

const char * lv_anim_label_get_text(lv_obj_t * obj);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ANIM_LABEL_H*/
