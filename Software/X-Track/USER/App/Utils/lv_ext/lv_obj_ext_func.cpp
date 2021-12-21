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
#include "lv_obj_ext_func.h"

void lv_obj_set_opa_scale(lv_obj_t* obj, int16_t opa)
{
    lv_obj_set_style_bg_opa(obj, (lv_opa_t)opa, LV_PART_MAIN);
}

int16_t lv_obj_get_opa_scale(lv_obj_t* obj)
{
    return lv_obj_get_style_bg_opa(obj, LV_PART_MAIN);
}

/**
  * @brief  在label后追加字符串
  * @param  label:被追加的对象
  * @param  text:追加的字符串
  * @retval 无
  */
void lv_label_set_text_add(lv_obj_t * label, const char * text)
{
    if(!label)
        return;

    lv_label_ins_text(label, (uint32_t)strlen(lv_label_get_text(label)), text);
}

/**
  * @brief  为对象添加动画
  * @param  obj:对象地址
  * @param  a:动画控制器地址
  * @param  exec_cb:控制对象属性的函数地址
  * @param  start:动画的开始值
  * @param  end:动画的结束值
  * @param  time:动画的执行时间
  * @param  delay:动画开始前的延时时间
  * @param  ready_cb:动画结束事件回调
  * @param  path_cb:动画曲线
  * @retval 无
  */
void lv_obj_add_anim(
    lv_obj_t * obj, lv_anim_t * a,
    lv_anim_exec_xcb_t exec_cb,
    int32_t start, int32_t end,
    uint16_t time,
    uint32_t delay,
    lv_anim_ready_cb_t ready_cb,
    lv_anim_path_cb_t path_cb
)
{
    lv_anim_t anim_temp;

    if (a == NULL)
    {
        a = &anim_temp;

        /* INITIALIZE AN ANIMATION
        *-----------------------*/
        lv_anim_init(a);
    }

    /* MANDATORY SETTINGS
     *------------------*/

    /*Set the "animator" function*/
    lv_anim_set_exec_cb(a, exec_cb);

    /*Set the "animator" function*/
    lv_anim_set_var(a, obj);

    /*Length of the animation [ms]*/
    lv_anim_set_time(a, time);

    /*Set start and end values. E.g. 0, 150*/
    lv_anim_set_values(a, start, end);


    /* OPTIONAL SETTINGS
     *------------------*/

    /*Time to wait before starting the animation [ms]*/
    lv_anim_set_delay(a, delay);

    /*Set the path in an animation*/
    lv_anim_set_path_cb(a, path_cb);

    /*Set a callback to call when animation is ready.*/
    lv_anim_set_ready_cb(a, ready_cb);

    /*Set a callback to call when animation is started (after delay).*/
    lv_anim_set_start_cb(a, ready_cb);

    /* START THE ANIMATION
     *------------------*/
    lv_anim_start(a);                             /*Start the animation*/
}

lv_indev_t* lv_get_indev(lv_indev_type_t type)
{
    lv_indev_t* cur_indev = NULL;
    for (;;)
    {
        cur_indev = lv_indev_get_next(cur_indev);
        if (!cur_indev)
        {
            break;
        }

        if (cur_indev->driver->type == type)
        {
            return cur_indev;
        }
    }
    return NULL;
}
