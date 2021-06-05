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
#include "PM_Log.h"

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

    base->priv.IsDisableAutoCache = base->priv.ReqDisableAutoCache;   

    PageStack.push(base);

    PM_LOG_INFO("Page(%s) push >> [Screen] (stash = 0x%p)", name, stash);

    SwitchTo(base, true, stash);

    return base;
}

PageBase* PageManager::Pop()
{
    if (!SwitchAnimStateCheck())
    {
        return nullptr;
    }

    PageBase* top = GetStackTop();

    if (top == nullptr)
    {
        PM_LOG_WARN("Page stack is empty, cat't pop");
        return nullptr;
    }

    if (!top->priv.IsDisableAutoCache)
    {
        PM_LOG_INFO("Page(%s) has auto cache, cache disabled", top->Name);
        top->priv.IsCached = false;
    }

    PM_LOG_INFO("Page(%s) pop << [Screen]  (stash = 0x%p)", top->Name, stash);

    PageStack.pop();

    top = GetStackTop();

    if (top != nullptr)
    {
        SwitchTo(top, false, nullptr);
    }

    return top;
}

void PageManager::SwitchTo(PageBase* newNode, bool isPushAct, const PageBase::Stash_t* stash)
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
        PM_LOG_INFO("stash is detect, %s >> stash(0x%p) >> %s", GetPagePrevName(), stash, newNode->Name);

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

    PageCurrent = newNode;

    if (PageCurrent->priv.IsCached)
    {
        PM_LOG_INFO("Page(%s) has cached, appear driectly", PageCurrent->Name);
        PageCurrent->priv.State = PageBase::PAGE_STATE_WILL_APPEAR;
    }
    else
    {
        PageCurrent->priv.State = PageBase::PAGE_STATE_LOAD;
    }

    if (PagePrev != nullptr)
    {
        PagePrev->priv.Anim.IsEnter = false;
    }

    PageCurrent->priv.Anim.IsEnter = true;

    AnimState.IsPushing = isPushAct;

    if (AnimState.IsPushing)
    {
        SwitchAnimTypeUpdate(PageCurrent);
    }

    StateUpdate(PagePrev);
    StateUpdate(PageCurrent);

    if (AnimState.IsPushing)
    {
        PM_LOG_INFO("Page PUSH is detect, move Page(%s) to foreground", PageCurrent->Name);
        if (PagePrev)lv_obj_move_foreground(PagePrev->root);
        lv_obj_move_foreground(PageCurrent->root);
    }
    else
    {
        PM_LOG_INFO("Page POP is detect, move Page(%s) to foreground", GetPagePrevName());
        lv_obj_move_foreground(PageCurrent->root);
        if (PagePrev)lv_obj_move_foreground(PagePrev->root);
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

bool PageManager::BackHome()
{
    if (!SwitchAnimStateCheck())
    {
        return false;
    }

    SetStackClear(true);

    PagePrev = nullptr;

    PageBase* home = GetStackTop();

    SwitchTo(home, false);

    return true;
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
    bool lastNodeBusy = PagePrev && PagePrev->priv.Anim.IsBusy;

    if (!PageCurrent->priv.Anim.IsBusy && !lastNodeBusy)
    {
        PM_LOG_INFO("----Page switch was all finished----");
        AnimState.IsSwitchReq = false;
        ret = true;
        PagePrev = PageCurrent;
    }
    else
    {
        if (PageCurrent->priv.Anim.IsBusy)
        {
            PM_LOG_WARN("Page PageCurrent(%s) is busy", PageCurrent->Name);
        }
        else
        {
            PM_LOG_WARN("Page PagePrev(%s) is busy", GetPagePrevName());
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
        manager->SwitchAnimTypeUpdate(manager->PageCurrent);
    }
}

void PageManager::SwitchAnimCreate(PageBase* base)
{
    LoadAnimAttr_t animAttr;
    if (!GetCurrentLoadAnimAttr(&animAttr))
    {
        return;
    }

    lv_anim_t a;
    AnimDefaultInit(&a);

    a.user_data = base;
    lv_anim_set_var(&a, base->root);
    lv_anim_set_ready_cb(&a, onSwitchAnimFinish);
    lv_anim_set_exec_cb(&a, animAttr.setter);

    int32_t start = 0;

    if (animAttr.getter)
    {
        start = animAttr.getter(base->root);
    }

    if (AnimState.IsPushing)
    {
        if (base->priv.Anim.IsEnter)
        {
            lv_anim_set_values(
                &a,
                animAttr.push.enter.start,
                animAttr.push.enter.end
            );
        }
        else /* Exit */
        {
            lv_anim_set_values(
                &a,
                start,
                animAttr.push.exit.end
            );
        }
    }
    else /* Pop */
    {
        if (base->priv.Anim.IsEnter)
        {
            lv_anim_set_values(
                &a,
                animAttr.pop.enter.start,
                animAttr.pop.enter.end
            );
        }
        else /* Exit */
        {
            lv_anim_set_values(
                &a,
                start,
                animAttr.pop.exit.end
            );
        }
    }

    lv_anim_start(&a);
    base->priv.Anim.IsBusy = true;
}

void PageManager::SetGlobalLoadAnimType(LoadAnim_t anim, uint16_t time, lv_anim_path_cb_t path)
{
    if (anim > LOAD_ANIM_MAX)
    {
        anim = LOAD_ANIM_NONE;
    }

    AnimState.Global.Type = anim;
    AnimState.Global.Time = time;
    AnimState.Global.Path = path;

    PM_LOG_INFO("Set global load anim type = %d", anim);
}

void PageManager::SwitchAnimTypeUpdate(PageBase* base)
{
    if (base->priv.Anim.Attr.Type == LOAD_ANIM_GLOBAL)
    {
        PM_LOG_INFO(
            "Page(%s) Anim.Type was not set, use AnimState.Global.Type = %d",
            base->Name,
            AnimState.Global.Type
        );
        AnimState.Current = AnimState.Global;
    }
    else
    {
        if (base->priv.Anim.Attr.Type > LOAD_ANIM_MAX)
        {
            PM_LOG_ERROR(
                "Page(%s) ERROR custom Anim.Type = %d, use AnimState.Global.Type = %d",
                base->Name,
                base->priv.Anim.Attr.Type,
                AnimState.Global.Type
            );
            base->priv.Anim.Attr = AnimState.Global;
        }
        else
        {
            PM_LOG_INFO(
                "Page(%s) custom Anim.Type set = %d",
                base->Name,
                base->priv.Anim.Attr.Type
            );
        }
        AnimState.Current = base->priv.Anim.Attr;
    }
}

void PageManager::AnimDefaultInit(lv_anim_t* a)
{
    lv_anim_init(a);

    uint32_t time = (GetCurrentLoadAnimType() == LOAD_ANIM_NONE) ? 0 : AnimState.Current.Time;
    lv_anim_set_time(a, time);
    lv_anim_set_path_cb(a, AnimState.Current.Path);
}
