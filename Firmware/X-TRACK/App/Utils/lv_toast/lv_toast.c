/**
 * @file lv_toast.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_toast.h"

/*********************
 *      DEFINES
 *********************/

#define MY_CLASS &lv_toast_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_toast_constructor(const lv_obj_class_t* class_p, lv_obj_t* obj);
static void lv_toast_event(const lv_obj_class_t* class_p, lv_event_t* e);
static void lv_toast_set_height(void* var, int32_t y);
static void lv_toast_set_opa(void* var, int32_t opa);
static void lv_toast_anim_del(lv_anim_t* a);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_toast_class = {
    .base_class = &lv_obj_class,
    .constructor_cb = lv_toast_constructor,
    .width_def = LV_DPI_DEF / 5,
    .height_def = LV_DPI_DEF / 5,
    .event_cb = lv_toast_event,
    .instance_size = sizeof(lv_toast_t),
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t* lv_toast_create(lv_obj_t* parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t* obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    lv_obj_remove_style_all(obj);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_size(obj, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_anim_time(obj, 200, 0);
    lv_obj_set_style_pad_all(obj, 10, 0);

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    return obj;
}

void lv_toast_show_text(lv_obj_t* obj, const char* txt, uint32_t duration)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_toast_t* toast = (lv_toast_t*)obj;

    lv_obj_t* cont = lv_obj_create(obj);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    if (toast->style) {
        lv_obj_add_style(cont, toast->style, 0);
    }

    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, txt);
    lv_obj_center(label);

    lv_obj_update_layout(cont);
    lv_coord_t h = lv_obj_get_height(cont);

    uint32_t anim_time = lv_obj_get_style_anim_time(obj, 0);

    /* height anim */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, cont);
    lv_anim_set_exec_cb(&a, lv_toast_set_height);
    lv_anim_set_values(&a, 0, h);
    lv_anim_set_time(&a, anim_time);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);

    /* opa anim */
    lv_anim_set_delay(&a, anim_time + duration);
    lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_exec_cb(&a, lv_toast_set_opa);
    lv_anim_set_deleted_cb(&a, lv_toast_anim_del);
    lv_anim_start(&a);
}

void lv_toast_set_cont_style(lv_obj_t* obj, lv_style_t* style)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_toast_t* toast = (lv_toast_t*)obj;
    toast->style = style;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_toast_constructor(const lv_obj_class_t* class_p, lv_obj_t* obj)
{
    LV_UNUSED(class_p);
    lv_toast_t* toast = (lv_toast_t*)obj;
    toast->style = NULL;
}

static void lv_toast_event(const lv_obj_class_t* class_p, lv_event_t* e)
{
    LV_UNUSED(class_p);
    lv_obj_event_base(MY_CLASS, e);
}

static void lv_toast_set_height(void* var, int32_t y)
{
    lv_obj_set_height(var, y);
}

static void lv_toast_set_opa(void* var, int32_t opa)
{
    lv_obj_set_style_opa(var, opa, 0);
}

static void lv_toast_anim_del(lv_anim_t* a)
{
    lv_obj_del(a->var);
}
