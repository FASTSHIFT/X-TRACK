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

lv_anim_label_t* lv_anim_label_create(lv_obj_t* parent)
{
    lv_anim_label_t * alabel = lv_mem_alloc(sizeof(lv_anim_label_t));
    LV_ASSERT_MALLOC(alabel);
    lv_memset_00(alabel, sizeof(lv_anim_label_t));

    alabel->obj = lv_obj_create(parent);
    lv_obj_remove_style_all(alabel->obj);

    alabel->label_1 = lv_label_create(alabel->obj);
    lv_obj_remove_style_all(alabel->label_1);
    lv_label_set_text(alabel->label_1, "");

    alabel->label_2 = lv_label_create(alabel->obj);
    lv_obj_remove_style_all(alabel->label_2);
    lv_label_set_text(alabel->label_2, "");

    alabel->label_act = alabel->label_1;
    alabel->dir = LV_DIR_BOTTOM;
    alabel->duration = 500;
    alabel->path_cb = lv_anim_path_ease_out;

    return alabel;
}

void lv_anim_label_set_size(lv_anim_label_t* alabel, lv_coord_t width, lv_coord_t height)
{
    LV_ASSERT_NULL(alabel);
    lv_obj_set_size(alabel->obj, width, height);
}

void lv_anim_label_set_style(lv_anim_label_t* alabel, lv_style_t* style)
{
    LV_ASSERT_NULL(alabel);
    lv_obj_add_style(alabel->label_1, style, 0);
    lv_obj_add_style(alabel->label_2, style, 0);
}

void lv_anim_label_set_dir(lv_anim_label_t* alabel, lv_dir_t dir)
{
    LV_ASSERT_NULL(alabel);
    alabel->dir = dir;
}

void lv_anim_label_set_time(lv_anim_label_t* alabel, uint32_t duration)
{
    LV_ASSERT_NULL(alabel);
    alabel->duration = duration;
}

void lv_anim_label_set_path(lv_anim_label_t* alabel, lv_anim_path_cb_t path_cb)
{
    LV_ASSERT_NULL(alabel);
    alabel->path_cb = path_cb;
}

void lv_anim_label_set_custom_anin(lv_anim_label_t* alabel, const lv_anim_t* a_enter, const lv_anim_t* a_exit)
{
    LV_ASSERT_NULL(alabel);
    if (a_enter)
    {
        alabel->a_enter = *a_enter;
    }

    if (a_exit)
    {
        alabel->a_exit = *a_exit;
    }
}

static void lv_anim_label_set_x(void* obj, int32_t x)
{
    lv_obj_set_x(obj, x);
}

static void lv_anim_label_set_y(void* obj, int32_t y)
{
    lv_obj_set_y(obj, y);
}

void lv_anim_label_push_text(lv_anim_label_t* alabel, const char* txt)
{
    LV_ASSERT_NULL(alabel);

    lv_obj_t* label_act = alabel->label_act;
    lv_obj_t* label_inv = label_act == alabel->label_1 ? alabel->label_2 : alabel->label_1;
    lv_dir_t dir = alabel->dir;

    lv_label_set_text(label_inv, txt);
    lv_obj_update_layout(label_inv);

    lv_coord_t obj_width = lv_obj_get_width(alabel->obj);
    lv_coord_t obj_height = lv_obj_get_height(alabel->obj);

    lv_coord_t label_inv_width = lv_obj_get_width(label_inv);
    lv_coord_t label_inv_height = lv_obj_get_height(label_inv);

    lv_coord_t label_inv_x = (obj_width - label_inv_width) / 2;
    lv_coord_t label_inv_y = (obj_height - label_inv_height) / 2;
    lv_coord_t label_inv_start;
    lv_coord_t label_inv_end;
    lv_coord_t label_act_end;
    lv_anim_exec_xcb_t exec_xcb;
    typedef lv_coord_t(*coord_get_cb_t)(const lv_obj_t*);
    coord_get_cb_t coord_get_cb;

    if (dir & LV_DIR_HOR)
    {
        exec_xcb = lv_anim_label_set_x;
        coord_get_cb = lv_obj_get_x;
        label_inv_end = label_inv_x;

        if (dir == LV_DIR_LEFT)
        {
            label_inv_x += obj_width;
            label_act_end = lv_obj_get_x(label_act) - obj_width;
        }
        else /* dir == LV_DIR_RIGHT */
        {
            label_inv_x -= obj_width;
            label_act_end = lv_obj_get_x(label_act) + obj_width;
        }
        label_inv_start = label_inv_x;
    }
    else if (dir & LV_DIR_VER)
    {
        exec_xcb = lv_anim_label_set_y;
        coord_get_cb = lv_obj_get_y;
        label_inv_end = label_inv_y;

        if (dir == LV_DIR_TOP)
        {
            label_inv_y += obj_height;
            label_act_end = lv_obj_get_y(label_act) - obj_height;
        }
        else /* dir == LV_DIR_BOTTOM */
        {
            label_inv_y -= obj_height;
            label_act_end = lv_obj_get_y(label_act) + obj_height;
        }
        label_inv_start = label_inv_y;
    }
    else
    {
        LV_LOG_WARN("dir = %d error", dir);
        return;
    }

    lv_obj_set_pos(label_inv, label_inv_x, label_inv_y);

    /* common */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_time(&a, alabel->duration);
    lv_anim_set_path_cb(&a, alabel->path_cb);
    lv_anim_set_exec_cb(&a, exec_xcb);

    /* label_inv */
    lv_anim_set_var(&a, label_inv);
    lv_anim_set_values(&a, label_inv_start, label_inv_end);
    lv_anim_start(&a);

    if (alabel->a_enter.exec_cb)
    {
        lv_anim_set_var(&alabel->a_enter, label_inv);
        lv_anim_start(&alabel->a_enter);
    }

    /* label_act */
    lv_anim_set_var(&a, label_act);
    lv_anim_set_values(&a, coord_get_cb(label_act), label_act_end);
    lv_anim_start(&a);

    if (alabel->a_exit.exec_cb)
    {
        lv_anim_set_var(&alabel->a_exit, label_act);
        lv_anim_start(&alabel->a_exit);
    }

    alabel->label_act = label_inv;
}

void lv_anim_label_del(lv_anim_label_t* alabel)
{
    LV_ASSERT_NULL(alabel);
    lv_obj_del(alabel->obj);
    lv_mem_free(alabel);
}
