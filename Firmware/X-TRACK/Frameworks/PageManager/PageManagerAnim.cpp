/*
 * MIT License
 * Copyright (c) 2021 - 2024 _VIFEXTech
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
#include "PageLog.h"
#include "PageManager.h"

PageManager::RES_TYPE PageManager::getLoadAnimAttr(PAGE_ANIM anim, LOAD_ANIM_ATTR* attr)
{
    lv_obj_t* rootParent = getRootParent();
    lv_obj_update_layout(rootParent);
    lv_coord_t hor = lv_obj_get_width(rootParent);
    lv_coord_t ver = lv_obj_get_height(rootParent);

    switch (anim) {
    case PAGE_ANIM::OVER_LEFT:
        attr->push.enter.start = hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = hor;
        break;

    case PAGE_ANIM::OVER_RIGHT:
        attr->push.enter.start = -hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -hor;
        break;

    case PAGE_ANIM::OVER_TOP:
        attr->push.enter.start = ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = ver;
        break;

    case PAGE_ANIM::OVER_BOTTOM:
        attr->push.enter.start = -ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = 0;

        attr->pop.enter.start = 0;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -ver;
        break;

    case PAGE_ANIM::MOVE_LEFT:
        attr->push.enter.start = hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = -hor;

        attr->pop.enter.start = -hor;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = hor;
        break;

    case PAGE_ANIM::MOVE_RIGHT:
        attr->push.enter.start = -hor;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = hor;

        attr->pop.enter.start = hor;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -hor;
        break;

    case PAGE_ANIM::MOVE_TOP:
        attr->push.enter.start = ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = -ver;

        attr->pop.enter.start = -ver;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = ver;
        break;

    case PAGE_ANIM::MOVE_BOTTOM:
        attr->push.enter.start = -ver;
        attr->push.enter.end = 0;
        attr->push.exit.start = 0;
        attr->push.exit.end = ver;

        attr->pop.enter.start = ver;
        attr->pop.enter.end = 0;
        attr->pop.exit.start = 0;
        attr->pop.exit.end = -ver;
        break;

    case PAGE_ANIM::FADE_ON:
        attr->push.enter.start = LV_OPA_TRANSP;
        attr->push.enter.end = LV_OPA_COVER;
        attr->push.exit.start = LV_OPA_COVER;
        attr->push.exit.end = LV_OPA_COVER;

        attr->pop.enter.start = LV_OPA_COVER;
        attr->pop.enter.end = LV_OPA_COVER;
        attr->pop.exit.start = LV_OPA_COVER;
        attr->pop.exit.end = LV_OPA_TRANSP;
        break;

    case PAGE_ANIM::NONE:
        /* Use default NONE */
        break;

    default:
        PAGE_LOG_ERROR("Load anim type error: %d", anim);
        return RES_TYPE::ERR_PARAM;
    }

    /* Determine the setter and getter of the animation */
    switch (anim) {
    case PAGE_ANIM::OVER_LEFT:
    case PAGE_ANIM::OVER_RIGHT:
    case PAGE_ANIM::MOVE_LEFT:
    case PAGE_ANIM::MOVE_RIGHT:
        attr->setter = [](void* obj, int32_t v) {
            lv_obj_set_x((lv_obj_t*)obj, v);
        };
        attr->getter = [](void* obj) {
            return (int32_t)lv_obj_get_x((lv_obj_t*)obj);
        };
        break;

    case PAGE_ANIM::OVER_TOP:
    case PAGE_ANIM::OVER_BOTTOM:
    case PAGE_ANIM::MOVE_TOP:
    case PAGE_ANIM::MOVE_BOTTOM:
        attr->setter = [](void* obj, int32_t v) {
            lv_obj_set_y((lv_obj_t*)obj, v);
        };
        attr->getter = [](void* obj) {
            return (int32_t)lv_obj_get_y((lv_obj_t*)obj);
        };
        break;

    case PAGE_ANIM::FADE_ON:
        attr->setter = [](void* obj, int32_t v) {
            lv_obj_set_style_opa((lv_obj_t*)obj, (lv_opa_t)v, LV_PART_MAIN);
        };
        attr->getter = [](void* obj) {
            return (int32_t)lv_obj_get_style_opa((lv_obj_t*)obj, LV_PART_MAIN);
        };
        break;

    default:
        break;
    }

    return RES_TYPE::OK;
}
