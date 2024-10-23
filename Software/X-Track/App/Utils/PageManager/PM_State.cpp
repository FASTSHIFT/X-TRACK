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
  * @brief  Update page state machine
  * @param  base: Pointer to the updated page
  * @retval None
  */
void PageManager::StateUpdate(PageBase* base)
{
    if (base == nullptr)
        return;

    switch (base->priv.State)
    {
    case PageBase::PAGE_STATE_IDLE:
        PM_LOG_INFO("Page(%s) state idle", base->_Name);
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
        PM_LOG_INFO("Page(%s) state active", base->_Name);
        break;

    case PageBase::PAGE_STATE_ACTIVITY:
        PM_LOG_INFO("Page(%s) state active break", base->_Name);
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
        PM_LOG_ERROR("Page(%s) state[%d] was NOT FOUND!", base->_Name, base->priv.State);
        break;
    }
}

/**
  * @brief  Page loading status
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateLoadExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state load", base->_Name);

    if (base->_root != nullptr)
    {
        PM_LOG_ERROR("Page(%s) root must be nullptr", base->_Name);
    }

    lv_obj_t* root_obj = lv_obj_create(lv_scr_act());
    
    lv_obj_clear_flag(root_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_user_data(root_obj, base);

    if (_RootDefaultStyle)
    {
        lv_obj_add_style(root_obj, _RootDefaultStyle, LV_PART_MAIN);
    }

    base->_root = root_obj;
    base->onViewLoad();

    if (GetIsOverAnim(GetCurrentLoadAnimType()))
    {
        PageBase* bottomPage = GetStackTopAfter();

        if (bottomPage != nullptr && bottomPage->priv.IsCached)
        {
            LoadAnimAttr_t animAttr;
            if (GetCurrentLoadAnimAttr(&animAttr))
            {
                if (animAttr.dragDir != ROOT_DRAG_DIR_NONE)
                {
                    RootEnableDrag(base->_root);
                }
            }
        }
    }

    base->onViewDidLoad();

    if (base->priv.IsDisableAutoCache)
    {
        PM_LOG_INFO("Page(%s) disable auto cache, ReqEnableCache = %d", base->_Name, base->priv.ReqEnableCache);
        base->priv.IsCached = base->priv.ReqEnableCache;
    }
    else
    {
        PM_LOG_INFO("Page(%s) AUTO cached", base->_Name);
        base->priv.IsCached = true;
    }

    return PageBase::PAGE_STATE_WILL_APPEAR;
}

/**
  * @brief  The page is about to show the status
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateWillAppearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state will appear", base->_Name);
    base->onViewWillAppear();
    lv_obj_clear_flag(base->_root, LV_OBJ_FLAG_HIDDEN);
    SwitchAnimCreate(base);
    return PageBase::PAGE_STATE_DID_APPEAR;
}

/**
  * @brief  The status of the page display
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateDidAppearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state did appear", base->_Name);
    base->onViewDidAppear();
    return PageBase::PAGE_STATE_ACTIVITY;
}

/**
  * @brief  The page is about to disappear
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateWillDisappearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state will disappear", base->_Name);
    base->onViewWillDisappear();
    SwitchAnimCreate(base);
    return PageBase::PAGE_STATE_DID_DISAPPEAR;
}

/**
  * @brief  Page disappeared end state
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateDidDisappearExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state did disappear", base->_Name);
    lv_obj_add_flag(base->_root, LV_OBJ_FLAG_HIDDEN);
    base->onViewDidDisappear();
    if (base->priv.IsCached)
    {
        PM_LOG_INFO("Page(%s) has cached", base->_Name);
        return PageBase::PAGE_STATE_WILL_APPEAR;
    }
    else
    {
        return PageBase::PAGE_STATE_UNLOAD;
    }
}

/**
  * @brief  Page unload complete
  * @param  base: Pointer to the updated page
  * @retval Next state
  */
PageBase::State_t PageManager::StateUnloadExecute(PageBase* base)
{
    PM_LOG_INFO("Page(%s) state unload", base->_Name);
    if (base->_root == nullptr)
    {
        PM_LOG_WARN("Page is loaded!");
        goto Exit;
    }

    base->onViewUnload();
    if (base->priv.Stash.ptr != nullptr && base->priv.Stash.size != 0)
    {
        PM_LOG_INFO("Page(%s) free stash(0x%p)[%d]", base->_Name, base->priv.Stash.ptr, base->priv.Stash.size);
        lv_mem_free(base->priv.Stash.ptr);
        base->priv.Stash.ptr = nullptr;
        base->priv.Stash.size = 0;
    }

    /* Delete after the end of the root animation life cycle */
    lv_obj_del_async(base->_root);
    base->_root = nullptr;
    base->priv.IsCached = false;
    base->onViewDidUnload();

Exit:
    return PageBase::PAGE_STATE_IDLE;
}
