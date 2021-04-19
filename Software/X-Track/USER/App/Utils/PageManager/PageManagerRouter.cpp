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

PageBase* PageManager::Push(const char* name, const PageBase::Stash_t* stash)
{
    if (!SwitchAnimStateCheck())
    {
        return nullptr;
    }

    if (FindPageInStack(name) != nullptr)
    {
        PM_LOG_ERROR("Page(%s) was multi push", name);
        return nullptr;
    }

    PageBase* base = FindPageInPool(name);

    if (base == nullptr)
    {
        PM_LOG_ERROR("Page(%s) was not install", name);
        return nullptr;
    }

    PageBase* top = GetStackTop();
    if (top != nullptr)
    {
        top->priv.IsDisableAutoCache = top->priv.DisableAutoCacheReq;

        if (top->priv.IsDisableAutoCache)
        {
            PM_LOG_INFO("Page(%s) disable auto cache, CacheEnableReq = %d", top->Name, top->priv.CacheEnableReq);
            top->priv.IsCacheEnable = top->priv.CacheEnableReq;
        }
        else
        {
            PM_LOG_INFO("Page(%s) has auto cache, cache enabled", top->Name);
            top->priv.IsCacheEnable = true;
        }     
    }

    PageBasePtr_t* tail = (PageBasePtr_t*)_lv_ll_ins_tail(&PageStack_LL);
    *tail = base;

    PM_LOG_INFO("Page(%s) push >> [Screen] (stash = 0x%p)", name, stash);

    AnimState.IsPushing = true;

    SwitchTo(base, stash);

    return base;
}

PageBase* PageManager::Pop(const PageBase::Stash_t* stash)
{
    if (!SwitchAnimStateCheck())
    {
        return nullptr;
    }

    PageBasePtr_t* top = GetStackTopPtr();

    if (top == nullptr)
    {
        PM_LOG_WARN("Page stack is empty, cat't pop");
        return nullptr;
    }

    if (!(*top)->priv.IsDisableAutoCache)
    {
        PM_LOG_INFO("Page(%s) has auto cache, cache disabled", (*top)->Name);
        (*top)->priv.IsCacheEnable = false;
    }

    PageBasePtr_t* topAfter = (PageBasePtr_t*)_lv_ll_get_prev(&PageStack_LL, top);
    _lv_ll_remove(&PageStack_LL, top);

    if (topAfter != nullptr)
    {
        PM_LOG_INFO("Page(%s) pop << [Screen]  (stash = 0x%p)", (*top)->Name, stash);
        PageBase* base = *topAfter;
        AnimState.IsPushing = false;
        SwitchTo(base, stash);
    }

    return *top;
}

void PageManager::SwitchTo(PageBase* newNode, const PageBase::Stash_t* stash)
{
    if (newNode == nullptr)
    {
        PM_LOG_ERROR("newNode is nullptr");
        return;
    }

    if (AnimState.IsSwitchReq)
    {
        PM_LOG_WARN("Page switch busy, reqire(%s) is ignore", newNode->Name);
        return;
    }

    AnimState.IsSwitchReq = true;

    if (stash != nullptr)
    {
        PM_LOG_INFO("stash is detect, %s >> stash(0x%p) >> %s", GetPrevNodeName(), stash, newNode->Name);

        void* buffer = nullptr;

        if (newNode->priv.Stash.ptr == nullptr)
        {
            buffer = lv_mem_alloc(stash->size);
            if (buffer == nullptr)
            {
                PM_LOG_ERROR("stash malloc failed");
            }
            else
            {
                PM_LOG_INFO("stash(0x%p) malloc[%d]", buffer, stash->size);
            }           
        }
        else if(newNode->priv.Stash.size == stash->size)
        {
            buffer = newNode->priv.Stash.ptr;
            PM_LOG_INFO("stash(0x%p) is exist", buffer);
        }

        if (buffer != nullptr)
        {
            memcpy(buffer, stash->ptr, stash->size);
            PM_LOG_INFO("stash memcpy[%d] 0x%p >> 0x%p", stash->size, stash->ptr, buffer);
            newNode->priv.Stash.ptr = buffer;
            newNode->priv.Stash.size = stash->size;
        }
    }

    CurrentNode = newNode;
    if (CurrentNode->priv.IsCacheEnable)
    {
        PM_LOG_INFO("Page(%s) has cached, appear driectly", CurrentNode->Name);
        CurrentNode->priv.State = PageBase::PAGE_STATE_WILL_APPEAR;
    }
    else
    {
        CurrentNode->priv.State = PageBase::PAGE_STATE_LOAD;
    }

    if (PrevNode != nullptr)
    {
        PrevNode->priv.Anim.IsEnter = false;
    }
    CurrentNode->priv.Anim.IsEnter = true;

    if (AnimState.IsPushing)
    {
        SwitchAnimTypeUpdate(CurrentNode);
    }

    StateUpdate(PrevNode);
    StateUpdate(CurrentNode);

    if (AnimState.IsPushing)
    {
        PM_LOG_INFO("Page PUSH is detect, move Page(%s) to foreground", CurrentNode->Name);
        if (PrevNode)lv_obj_move_foreground(PrevNode->root);
        lv_obj_move_foreground(CurrentNode->root);
    }
    else
    {
        PM_LOG_INFO("Page POP is detect, move Page(%s) to foreground", GetPrevNodeName());
        lv_obj_move_foreground(CurrentNode->root);
        if (PrevNode)lv_obj_move_foreground(PrevNode->root);
    }
}

bool PageManager::FourceUnload(PageBase* base)
{
    if (base == nullptr)
    {
        PM_LOG_ERROR("Page is nullptr, Unload failed");
        return false;
    }

    PM_LOG_INFO("Page(%s) Fource unloading...", base->Name);

    if (base->priv.State == PageBase::PAGE_STATE_ACTIVITY)
    {
        PM_LOG_INFO("Page state is ACTIVITY, Disappearing...");
        base->onViewWillDisappear();
        base->onViewDidDisappear();
    }

    base->priv.State = StateUnloadExecute(base);

    return true;
}

void PageManager::GoHome()
{
    SetStackClear(true);
}

bool PageManager::SwitchAnimStateCheck()
{
    if (AnimState.IsSwitchReq || AnimState.IsBusy)
    {
        PM_LOG_WARN(
            "Page switch busy[AnimState.IsSwitchReq = %d,"
            "AnimState.IsBusy = %d],"
            "reqire is ignore",
            AnimState.IsSwitchReq, 
            AnimState.IsBusy
        );
        return false;
    }

    return true;
}


bool PageManager::SwitchReqCheck()
{
    bool ret = false;
    bool lastNodeBusy = PrevNode && PrevNode->priv.Anim.IsBusy;

    if (!CurrentNode->priv.Anim.IsBusy && !lastNodeBusy)
    {
        PM_LOG_INFO("----Page switch was all finished----");
        AnimState.IsSwitchReq = false;
        ret = true;
        PrevNode = CurrentNode;
    }
    else
    {
        if (CurrentNode->priv.Anim.IsBusy)
        {
            PM_LOG_WARN("Page CurrentNode(%s) is busy", CurrentNode->Name);
        }
        else
        {
            PM_LOG_WARN("Page PrevNode(%s) is busy", GetPrevNodeName());
        }
    }

    return ret;
}

void PageManager::onSwitchAnimFinish(lv_anim_t* a)
{
    PageBase* base = (PageBase*)a->user_data;
    PageManager* manager = base->Manager;

    PM_LOG_INFO("Page(%s) Anim finish", base->Name);

    manager->StateUpdate(base);
    base->priv.Anim.IsBusy = false;
    bool isFinished = manager->SwitchReqCheck();

    if (!manager->AnimState.IsPushing && isFinished)
    {
        manager->SwitchAnimTypeUpdate(manager->CurrentNode);
    }
}

void PageManager::SwitchAnimCreate(PageBase* base)
{
    const LoadAnimAttr_t* animAttr = GetCurrentLoadAnimAttr();

    lv_anim_t a;
    AnimDefaultInit(&a);

    a.user_data = base;
    lv_anim_set_var(&a, base->root);
    lv_anim_set_ready_cb(&a, onSwitchAnimFinish);
    lv_anim_set_exec_cb(&a, animAttr->anim_setter);

    lv_anim_value_t start_val = animAttr->anim_getter ? animAttr->anim_getter(base->root) : 0;

    if (AnimState.IsPushing)
    {
        if (base->priv.Anim.IsEnter)
        {
            lv_anim_set_values(
                &a,
                animAttr->PushEnterStart,
                animAttr->PushEnterEnd
            );
        }
        else /* Exit */
        {
            lv_anim_set_values(
                &a,
                start_val,
                animAttr->PushExitEnd
            );
        }
    }
    else /* Pop */
    {
        if (base->priv.Anim.IsEnter)
        {
            lv_anim_set_values(
                &a,
                animAttr->PopEnterStart,
                animAttr->PopEnterEnd
            );
        }
        else /* Exit */
        {
            lv_anim_set_values(
                &a,
                start_val,
                animAttr->PopExitEnd
            );
        }
    }

    lv_anim_start(&a);
    base->priv.Anim.IsBusy = true;
}

void PageManager::SetGlobalLoadAnimType(LoadAnim_t anim, uint16_t time, lv_anim_path_cb_t path)
{
    if (anim >= LOAD_ANIM_MAX)
    {
        anim = LOAD_ANIM_NONE;
    }

    AnimState.TypeGlobal = anim;
    AnimState.Time = time;
    AnimState.Path = path;

    PM_LOG_INFO("Set global load anim type = %d", anim);
}

void PageManager::SwitchAnimTypeUpdate(PageBase* base)
{
    if (base->priv.Anim.Type == LOAD_ANIM_GLOBAL)
    {
        PM_LOG_INFO(
            "Page(%s) Anim.Type was not set, use AnimState.TypeGlobal = %d",
            base->Name,
            AnimState.TypeGlobal
        );
        AnimState.TypeCurrent = AnimState.TypeGlobal;
    }
    else
    {
        if (base->priv.Anim.Type >= LOAD_ANIM_MAX)
        {
            PM_LOG_ERROR(
                "Page(%s) ERROR custom Anim.Type = %d, use AnimState.TypeGlobal = %d",
                base->Name,
                base->priv.Anim.Type,
                AnimState.TypeGlobal
            );
            base->priv.Anim.Type = AnimState.TypeGlobal;
        }
        else
        {
            PM_LOG_INFO(
                "Page(%s) custom Anim.Type set = %d",
                base->Name,
                base->priv.Anim.Type
            );
        }
        AnimState.TypeCurrent = (LoadAnim_t)base->priv.Anim.Type;
    }
}

void PageManager::AnimDefaultInit(lv_anim_t* a)
{
    lv_anim_init(a);

    uint32_t time = (AnimState.TypeCurrent == LOAD_ANIM_NONE) ? 0 : AnimState.Time;
    lv_anim_set_time(a, time);

    lv_anim_path_t path;
    lv_anim_path_init(&path);
    lv_anim_path_set_cb(&path, AnimState.Path);
    lv_anim_set_path(a, &path);
}
