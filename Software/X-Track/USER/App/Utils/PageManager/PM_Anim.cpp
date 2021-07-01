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
#include "PageManager.h"
#include "PM_Log.h"

/**
  * @brief  Set the transparency of obj
  * @param  obj: Pointer to obj
  * @param  opa_scale: Transparency value
  * @retval None
  */
static void _lv_obj_set_opa_scale(lv_obj_t* obj, int32_t opa_scale)
{
    lv_obj_set_style_bg_opa(obj, (lv_opa_t)opa_scale, LV_PART_MAIN);
}

/**
  * @brief  Get obj transparency
  * @param  obj: Pointer to obj
  * @retval Transparency value
  */
static int32_t _lv_obj_get_opa_scale(lv_obj_t* obj)
{
    return lv_obj_get_style_bg_opa(obj, LV_PART_MAIN);
}

/**
  * @brief  Get page loading animation properties
  * @param  anim: Animation type
  * @param  attr: Pointer to attribute
  * @retval Whether the acquisition is successful
  */
bool PageManager::GetLoadAnimAttr(uint8_t anim, LoadAnimAttr_t* attr)
{
    lv_coord_t hor = LV_HOR_RES;
    lv_coord_t ver = LV_VER_RES;

    switch (anim)
    {
    case LOAD_ANIM_OVER_LEFT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = hor;
        break;

    case LOAD_ANIM_OVER_RIGHT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = -hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -hor;
        break;

    case LOAD_ANIM_OVER_TOP:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = ver;
        break;

    case LOAD_ANIM_OVER_BOTTOM:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = -ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -ver;
        break;

    case LOAD_ANIM_MOVE_LEFT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = -hor;

        attr->pop.enter.start = -hor;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = hor;
        break;

    case LOAD_ANIM_MOVE_RIGHT:
        attr->dragDir = ROOT_DRAG_DIR_HOR;

        attr->push.enter.start = -hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = hor;

        attr->pop.enter.start = hor;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -hor;
        break;

    case LOAD_ANIM_MOVE_TOP:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = -ver;

        attr->pop.enter.start = -ver;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = ver;
        break;

    case LOAD_ANIM_MOVE_BOTTOM:
        attr->dragDir = ROOT_DRAG_DIR_VER;

        attr->push.enter.start = -ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = ver;

        attr->pop.enter.start = ver;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -ver;
        break;

    case LOAD_ANIM_FADE_ON:
        attr->dragDir = ROOT_DRAG_DIR_NONE;

        attr->push.enter.start = LV_OPA_TRANSP;
        attr->push.enter.end = LV_OPA_COVER;
        attr->push.exit.start = LV_OPA_COVER;
        attr->push.exit.end = LV_OPA_COVER;

        attr->pop.enter.start = LV_OPA_COVER;
        attr->pop.enter.end = LV_OPA_COVER;
        attr->pop.exit.start = LV_OPA_COVER;
        attr->pop.exit.end = LV_OPA_TRANSP;
        break;

    case LOAD_ANIM_NONE:
        memset(attr, 0, sizeof(LoadAnimAttr_t));
        break;

    default:
        PM_LOG_ERROR("Load anim type error: %d", anim);
        return false;
    }

    /* Determine the setter and getter of the animation */
    if (attr->dragDir == ROOT_DRAG_DIR_HOR)
    {
        attr->setter = (lv_anim_setter_t)lv_obj_set_x;
        attr->getter = (lv_anim_getter_t)lv_obj_get_x;
    }
    else if (attr->dragDir == ROOT_DRAG_DIR_VER)
    {
        attr->setter = (lv_anim_setter_t)lv_obj_set_y;
        attr->getter = (lv_anim_getter_t)lv_obj_get_y;
    }
    else
    {
        attr->setter = (lv_anim_setter_t)_lv_obj_set_opa_scale;
        attr->getter = (lv_anim_getter_t)_lv_obj_get_opa_scale;
    }

    return true;
}
