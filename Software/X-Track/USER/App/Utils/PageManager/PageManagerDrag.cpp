/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the follo18wing conditions:
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
#include "PageManagerLog.h"

#ifndef constrain
#  define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif // !constrain

static lv_signal_cb_t lv_obj_origin_signal_cb = nullptr;

lv_res_t PageManager::onRootSignal(lv_obj_t* obj, lv_signal_t signal, void* param)
{
    PageBase::root_ext_attr_t ext_attr;
    memcpy(&ext_attr, lv_obj_get_ext_attr(obj), sizeof(ext_attr));

    PageBase* base = ext_attr.base;
    PageManager* manager = base->Manager;

    lv_res_t res = lv_obj_origin_signal_cb(obj, signal, param);

    if (res != LV_RES_OK)
        return res;

    const LoadAnimAttr_t* animAttr = manager->GetCurrentLoadAnimAttr();

    if (signal == LV_SIGNAL_COORD_CHG)
    {   
        lv_coord_t val = animAttr->anim_getter(obj);

        lv_coord_t val_max = LV_MATH_MAX(animAttr->PopExitStart, animAttr->PopExitEnd);
        lv_coord_t val_min = LV_MATH_MIN(animAttr->PopExitStart, animAttr->PopExitEnd);

        animAttr->anim_setter(obj, constrain(val, val_min, val_max));
    }
    else if (signal == LV_SIGNAL_DRAG_THROW_BEGIN)
    {
        if (manager->AnimState.IsSwitchReq)
            return res;

        lv_coord_t offset_sum = animAttr->PushEnterEnd - animAttr->PushEnterStart;

        lv_coord_t x_predict = 0;
        lv_coord_t y_predict = 0;
        RootGetDragPredict(&x_predict, &y_predict);

        lv_coord_t start = animAttr->anim_getter(obj);
        lv_coord_t end = start;

        if (animAttr->anim_getter == (lv_anim_getter_t)lv_obj_get_x)
        {
            end += x_predict;
            PM_LOG_INFO("Root drag x_predict = %d", end);
        }
        else if (animAttr->anim_getter == (lv_anim_getter_t)lv_obj_get_y)
        {
            end += y_predict;
            PM_LOG_INFO("Root drag y_predict = %d", end);
        }

        if (LV_MATH_ABS(end) > LV_MATH_ABS(offset_sum) / 2)
        {
            lv_async_call(RootAsyncCall, base);
        }
        else if(end != animAttr->PushEnterEnd)
        {
            manager->AnimState.IsBusy = true;

            lv_anim_t a;
            manager->AnimDefaultInit(&a);
            a.user_data = manager;
            lv_anim_set_var(&a, obj);
            lv_anim_set_values(&a, animAttr->anim_getter(obj), animAttr->PushEnterEnd);
            lv_anim_set_exec_cb(&a, animAttr->anim_setter);
            lv_anim_set_ready_cb(&a, onRootAnimFinish);    
            lv_anim_start(&a);
            PM_LOG_INFO("Root anim start");
        }
    }
    else if (signal == LV_SIGNAL_PRESSED)
    {
        if (manager->AnimState.IsSwitchReq)
            return res;

        if (!manager->AnimState.IsBusy)
            return res;

        PM_LOG_INFO("Root anim interrupted");
        lv_anim_del(obj, animAttr->anim_setter);
        manager->AnimState.IsBusy = false;
    }

    return res;
}

void PageManager::onRootAnimFinish(lv_anim_t* a)
{
    PageManager* manager = (PageManager*)a->user_data;
    PM_LOG_INFO("Root anim finish");
    manager->AnimState.IsBusy = false;
}

void PageManager::RootEnableDrag(lv_obj_t* root, lv_drag_dir_t drag_dir)
{
    if (lv_obj_origin_signal_cb == nullptr)
    {
        lv_obj_origin_signal_cb = lv_obj_get_signal_cb(root);
    }
    lv_obj_set_signal_cb(root, onRootSignal);
    lv_obj_set_drag(root, true);
    lv_obj_set_drag_throw(root, true);
    lv_obj_set_drag_dir(root, drag_dir);
    PM_LOG_INFO("Root drag enabled");
}

void PageManager::RootAsyncCall(void* user_data)
{ 
    PageBase* base = (PageBase*)user_data;
    lv_event_send(base->root, LV_EVENT_LEAVE, nullptr);
    PM_LOG_INFO("Root send event = LV_EVENT_LEAVE, need to handle");
}

void PageManager::RootGetDragPredict(lv_coord_t* x, lv_coord_t* y)
{
    lv_indev_t* indev = lv_indev_get_act();
    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);

    lv_coord_t y_predict = 0;
    lv_coord_t x_predict = 0;

    while (vect.y != 0)
    {
        y_predict += vect.y;
        vect.y = vect.y * (100 - LV_INDEV_DEF_DRAG_THROW) / 100;
    }

    while (vect.x != 0)
    {
        x_predict += vect.x;
        vect.x = vect.x * (100 - LV_INDEV_DEF_DRAG_THROW) / 100;
    }

    *x = x_predict;
    *y = y_predict;

    lv_indev_finish_drag(indev);
}
