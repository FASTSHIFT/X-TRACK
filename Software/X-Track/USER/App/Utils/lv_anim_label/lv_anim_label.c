/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "lv_anim_label.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_anim_label_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_anim_label_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_anim_label_set_x(void* obj, int32_t x);
static void lv_anim_label_set_y(void* obj, int32_t y);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_anim_label_class =
{
    .constructor_cb = lv_anim_label_constructor,
    .instance_size = sizeof(lv_anim_label_t),
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_anim_label_create(lv_obj_t * parent)
{
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    return obj;
}

void lv_anim_label_set_dir(lv_obj_t * obj, lv_dir_t dir)
{
    lv_anim_label_set_enter_dir(obj, dir);
    lv_anim_label_set_exit_dir(obj, dir);
}

void lv_anim_label_set_enter_dir(lv_obj_t * obj, lv_dir_t dir)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    alabel->enter_dir = dir;
}

void lv_anim_label_set_exit_dir(lv_obj_t * obj, lv_dir_t dir)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    alabel->exit_dir = dir;
}

void lv_anim_label_set_time(lv_obj_t * obj, uint32_t duration)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    alabel->duration = duration;
}

void lv_anim_label_set_path(lv_obj_t * obj, lv_anim_path_cb_t path_cb)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    alabel->path_cb = path_cb;
}

void lv_anim_label_add_style(lv_obj_t * obj, lv_style_t * style)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    lv_obj_add_style(alabel->label_1, style, LV_PART_MAIN);
    lv_obj_add_style(alabel->label_2, style, LV_PART_MAIN);
}

void lv_anim_label_set_custom_enter_anim(lv_obj_t * obj, const lv_anim_t * a)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    if (a != NULL)
    {
        alabel->a_enter = *a;
    }
}

void lv_anim_label_set_custom_exit_anim(lv_obj_t * obj, const lv_anim_t * a)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    if (a != NULL)
    {
        alabel->a_exit = *a;
    }
}

static void lv_anim_label_start_anim(
    lv_obj_t * label, 
    lv_dir_t dir, 
    int32_t start, 
    int32_t end,
    uint32_t duration,
    lv_anim_path_cb_t path_cb
)
{
    lv_anim_exec_xcb_t exec_xcb;

    if (dir & LV_DIR_HOR)
    {
        exec_xcb = lv_anim_label_set_x;
    }
    else if (dir & LV_DIR_VER)
    {
        exec_xcb = lv_anim_label_set_y;
    }
    else
    {
        return;
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, label);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_time(&a, duration);
    lv_anim_set_path_cb(&a, path_cb);
    lv_anim_set_exec_cb(&a, exec_xcb);
    lv_anim_start(&a);
}

void lv_anim_label_push_text(lv_obj_t * obj, const char * txt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    lv_obj_t * label_exit = alabel->label_act;
    lv_obj_t * label_enter = alabel->label_act == alabel->label_1 ? alabel->label_2 : alabel->label_1;
    lv_dir_t enter_dir = alabel->enter_dir;
    lv_dir_t exit_dir = alabel->exit_dir;

    lv_label_set_text(label_enter, txt);
    lv_obj_update_layout(label_enter);

    lv_coord_t obj_width = lv_obj_get_width(obj);
    lv_coord_t obj_height = lv_obj_get_height(obj);

    /* enter */
    lv_coord_t label_enter_width = lv_obj_get_width(label_enter);
    lv_coord_t label_enter_height = lv_obj_get_height(label_enter);
    lv_coord_t label_enter_end_x = (obj_width - label_enter_width) / 2;
    lv_coord_t label_enter_end_y = (obj_height - label_enter_height) / 2;

    if (enter_dir & LV_DIR_HOR)
    {
        lv_obj_set_y(label_enter, label_enter_end_y);
        lv_coord_t start_x = label_enter_end_x;
        start_x += ((enter_dir == LV_DIR_LEFT) ? -obj_width : obj_width);
        lv_anim_label_start_anim(
            label_enter,
            enter_dir,
            start_x,
            label_enter_end_x,
            alabel->duration,
            alabel->path_cb
        );
    }
    else if (enter_dir & LV_DIR_VER)
    {
        lv_obj_set_x(label_enter, label_enter_end_x);
        lv_coord_t start_y = label_enter_end_y;
        start_y += ((enter_dir == LV_DIR_TOP) ? -obj_height : obj_height);
        lv_anim_label_start_anim(
            label_enter,
            enter_dir,
            start_y,
            label_enter_end_y,
            alabel->duration,
            alabel->path_cb
        );
    }
    else
    {
        lv_obj_set_pos(label_enter, label_enter_end_x, label_enter_end_y);
    }

    if (alabel->a_enter.exec_cb != NULL)
    {
        lv_anim_set_var(&alabel->a_enter, label_enter);
        lv_anim_start(&alabel->a_enter);
    }

    /* exit */
    lv_coord_t label_exit_width = lv_obj_get_width(label_exit);
    lv_coord_t label_exit_height = lv_obj_get_height(label_exit);
    lv_coord_t label_exit_start_x = (obj_width - label_exit_width) / 2;
    lv_coord_t label_exit_start_y = (obj_height - label_exit_height) / 2;

    if (exit_dir & LV_DIR_HOR)
    {
        lv_obj_set_y(label_exit, label_exit_start_y);
        lv_coord_t end_x = label_exit_start_x;
        end_x += ((exit_dir == LV_DIR_LEFT) ? obj_width : -obj_width);
        lv_anim_label_start_anim(
            label_exit,
            exit_dir,
            label_exit_start_x,
            end_x,
            alabel->duration,
            alabel->path_cb
        );
    }
    else if (exit_dir & LV_DIR_VER)
    {
        lv_obj_set_x(label_exit, label_exit_start_x);
        lv_coord_t end_y = label_exit_start_y;
        end_y += ((exit_dir == LV_DIR_TOP) ? obj_height : -obj_height);
        lv_anim_label_start_anim(
            label_exit,
            exit_dir,
            label_exit_start_y,
            end_y,
            alabel->duration,
            alabel->path_cb
        );
    }
    else
    {
        lv_obj_set_pos(label_exit, label_exit_start_x, label_exit_start_y);
    }

    if (alabel->a_exit.exec_cb != NULL)
    {
        lv_anim_set_var(&alabel->a_exit, label_exit);
        lv_anim_start(&alabel->a_exit);
    }

    alabel->label_act = label_enter;
}

lv_dir_t lv_anim_label_get_enter_dir(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    return alabel->enter_dir;
}

lv_dir_t lv_anim_label_get_exit_dir(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    return alabel->exit_dir;
}

uint32_t lv_anim_label_get_time(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    return alabel->duration;
}

lv_anim_path_cb_t lv_anim_label_get_path(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    return alabel->path_cb;
}

const char * lv_anim_label_get_text(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    return lv_label_get_text(alabel->label_act);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_anim_label_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_TRACE_OBJ_CREATE("begin");

    lv_anim_label_t * alabel = (lv_anim_label_t *)obj;

    alabel->label_1 = lv_label_create(obj);
    lv_label_set_text(alabel->label_1, "");

    alabel->label_2 = lv_label_create(obj);
    lv_label_set_text(alabel->label_2, "");

    alabel->label_act = alabel->label_1;
    //alabel->dir = LV_DIR_BOTTOM;
    alabel->duration = 500;
    alabel->path_cb = lv_anim_path_ease_out;

    lv_anim_init(&alabel->a_enter);
    lv_anim_init(&alabel->a_exit);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_anim_label_set_x(void * obj, int32_t x)
{
    lv_obj_set_x(obj, x);
}

static void lv_anim_label_set_y(void * obj, int32_t y)
{
    lv_obj_set_y(obj, y);
}
