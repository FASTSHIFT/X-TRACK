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

void PageManager::StateUpdate(PageBase* base)
{
    if (base == nullptr)
        return;

    switch (base->priv.State)
    {
    case PageBase::PAGE_STATE_IDLE:
        PM_LOG_INFO("Page(%s) state idle", base->Name);
        break;

    case PageBase::PAGE_STATE_LOAD:
        base->priv.State = StateLoadExecute(base);
        StateUpdate(base);
        break;

    case PageBase::PAGE_STATE_WILL_APPEAR:
        base->priv.State = StateWillAppearExecute(base);
        break;

    case PageBase::PAGE_STATE_DID_APPEAR:
        base->priv.State = StateDidAppearExecute(base);
        PM_LOG_INFO("Page(%s) state active", base->Name);
        break;

    case PageBase::PAGE_STATE_ACTIVITY:
        PM_LOG_INFO("Page(%s) state active break", base->Name);
        base->priv.State = PageBase::PAGE_STATE_WILL_DISAPPEAR;
        StateUpdate(base);
        break;

    case PageBase::PAGE_STATE_WILL_DISAPPEAR:
        base->priv.State = StateWillDisappearExecute(base);
        break;

    case PageBase::PAGE_STATE_DID_DISAPPEAR:
        base->priv.State = StateDidDisappearExecute(base);
        if (base->priv.State == PageBase::PAGE_STATE_UNLOAD)
        {
            StateUpdate(base);
        }
        break;

    case PageBase::PAGE_STATE_UNLOAD:
        base->priv.State = StateUnloadExecute(base);
        break;

    default:
        PM_LOG_ERROR("Page(%s) state[%d] was NOT FOUND!", base->Name, base->priv.State);
        break;
    }
}

PageBase::State_t PageManager::StateLoadExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state load", base->Name);

    if (base->root != nullptr)
    {
        PM_LOG_ERROR("Page(%s) root must be nullptr", base->Name);
    }

    lv_obj_t* root_obj = lv_obj_create(lv_scr_act(), nullptr);
    lv_obj_set_size(root_obj, LV_HOR_RES_MAX, LV_VER_RES_MAX);

    PageBase::root_ext_attr_t* ext_attr_ptr =
        (PageBase::root_ext_attr_t*)lv_obj_allocate_ext_attr(
            root_obj, sizeof(PageBase::root_ext_attr_t)
        );

    ext_attr_ptr->base = base;
    base->root = root_obj;
    base->onViewLoad();

    if (GetIsOverAnim((LoadAnim_t)base->priv.Anim.Type))
    {
        PageBase* topAfter = GetStackTopAfter();
        if (topAfter != nullptr && topAfter->priv.IsCacheEnable)
        {
            lv_anim_exec_xcb_t anim_exec_xcb = AnimState.LoadAnimAttr_Grp[base->priv.Anim.Type].anim_setter;
            if (anim_exec_xcb == (lv_anim_exec_xcb_t)lv_obj_set_x)
            {
                RootEnableDrag(base->root, LV_DRAG_DIR_HOR);
            }
            else if (anim_exec_xcb == (lv_anim_exec_xcb_t)lv_obj_set_y)
            {
                RootEnableDrag(base->root, LV_DRAG_DIR_VER);
            }
        }
    }

    base->onViewDidLoad();
    return PageBase::PAGE_STATE_WILL_APPEAR;
}

PageBase::State_t PageManager::StateWillAppearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state will appear", base->Name);
    base->onViewWillAppear();
    lv_obj_set_hidden(base->root, false);
    SwitchAnimCreate(base);
    return PageBase::PAGE_STATE_DID_APPEAR;
}

PageBase::State_t PageManager::StateDidAppearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state did appear", base->Name);
    base->onViewDidAppear();
    return PageBase::PAGE_STATE_ACTIVITY;
}

PageBase::State_t PageManager::StateWillDisappearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state will disappear", base->Name);
    base->onViewWillDisappear();
    SwitchAnimCreate(base);
    return PageBase::PAGE_STATE_DID_DISAPPEAR;
}

PageBase::State_t PageManager::StateDidDisappearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state did disappear", base->Name);
    if (AnimState.TypeCurrent == LOAD_ANIM_FADE_ON)
    {
        PM_LOG_INFO("AnimState.TypeCurrent == LOAD_ANIM_FADE_ON, Page(%s) hidden", base->Name);
        lv_obj_set_hidden(base->root, true);
    }
    base->onViewDidDisappear();
    if (base->priv.IsCacheEnable)
    {
        PM_LOG_INFO("Page(%s) has cached", base->Name);
        return PageBase::PAGE_STATE_WILL_APPEAR;
    }
    else
    {
        return PageBase::PAGE_STATE_UNLOAD;
    }
}

PageBase::State_t PageManager::StateUnloadExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state unload", base->Name);
    if (base->priv.Stash.ptr != nullptr && base->priv.Stash.size != 0)
    {
        PM_LOG_INFO("Page(%s) free stash(0x%p)[%d]", base->Name, base->priv.Stash.ptr, base->priv.Stash.size);
        lv_mem_free(base->priv.Stash.ptr);
        base->priv.Stash.ptr = nullptr;
        base->priv.Stash.size = 0;
    }
    lv_obj_del(base->root);
    base->root = nullptr;
    base->onViewDidUnload();
    return PageBase::PAGE_STATE_IDLE;
}
