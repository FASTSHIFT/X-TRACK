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

static const PageManager::LoadAnimAttr_t LoadAnimAttrArr[PageManager::LOAD_ANIM_MAX] =
{
    /* LOAD_ANIM_GLOBAL */
    {
    },

    /* LOAD_ANIM_NONE */
    {
    },

    /* LOAD_ANIM_OVER_LEFT */
    {
        (PageManager::lv_anim_setter_t)lv_obj_set_x,
        (PageManager::lv_anim_getter_t)lv_obj_get_x,

        /*Enter*/
        LV_HOR_RES_MAX, 0,  //Push
        0, 0,               //Pop

        /*Exit*/
        0, 0,               //Push
        0, LV_HOR_RES,      //Pop
    },

    /* LOAD_ANIM_OVER_RIGHT */
    {
        (PageManager::lv_anim_setter_t)lv_obj_set_x,
        (PageManager::lv_anim_getter_t)lv_obj_get_x,

        /*Enter*/
        -LV_HOR_RES_MAX, 0, //Push
        0, 0,               //Pop

        /*Exit*/
        0, 0,               //Push
        0, -LV_HOR_RES_MAX, //Pop
    },

    /* LOAD_ANIM_OVER_TOP */
    {
        (PageManager::lv_anim_setter_t)lv_obj_set_y,
        (PageManager::lv_anim_getter_t)lv_obj_get_y,

        /*Enter*/
        LV_VER_RES_MAX, 0,  //Push
        0, 0,               //Pop

        /*Exit*/
        0, 0,               //Push
        0, LV_VER_RES_MAX,  //Pop
    },

    /* LOAD_ANIM_OVER_BOTTOM */
    {
        (PageManager::lv_anim_setter_t)lv_obj_set_y,
        (PageManager::lv_anim_getter_t)lv_obj_get_y,

        /*Enter*/
        -LV_VER_RES_MAX, 0,  //Push
        0, 0,                //Pop

        /*Exit*/
        0, 0,                //Push
        0, - LV_VER_RES_MAX, //Pop
    },

    /* LOAD_ANIM_MOVE_LEFT */
    {
        (PageManager::lv_anim_setter_t)lv_obj_set_x,
        (PageManager::lv_anim_getter_t)lv_obj_get_x,

        /*Enter*/
        LV_HOR_RES_MAX, 0,      //Push
        -LV_HOR_RES_MAX, 0,     //Pop

        /*Exit*/
        0, -LV_HOR_RES_MAX,     //Push
        0, LV_HOR_RES_MAX,      //Pop
    },

    /* LOAD_ANIM_MOVE_RIGHT */
    {
        (PageManager::lv_anim_setter_t)lv_obj_set_x,
        (PageManager::lv_anim_getter_t)lv_obj_get_x,

        /*Enter*/
        -LV_HOR_RES_MAX, 0,     //Push
        LV_HOR_RES_MAX, 0,      //Pop

        /*Exit*/
        0, LV_HOR_RES_MAX,      //Push
        0, -LV_HOR_RES_MAX,     //Pop
    },

    /* LOAD_ANIM_MOVE_TOP */
    {
        (PageManager::lv_anim_setter_t)lv_obj_set_y,
        (PageManager::lv_anim_getter_t)lv_obj_get_y,

        /*Enter*/
        LV_VER_RES_MAX, 0,     //Push
        -LV_VER_RES_MAX, 0,    //Pop

        /*Exit*/
        0, -LV_VER_RES_MAX,    //Push
        0, LV_HOR_RES_MAX,     //Pop
    },

    /* LOAD_ANIM_MOVE_BOTTOM */
    {
        (PageManager::lv_anim_setter_t)lv_obj_set_y,
        (PageManager::lv_anim_getter_t)lv_obj_get_y,

        /*Enter*/
        -LV_VER_RES_MAX, 0,    //Push
        LV_VER_RES_MAX, 0,     //Pop

        /*Exit*/
        0, LV_VER_RES_MAX,     //Push
        0, -LV_HOR_RES_MAX,    //Pop
    },

    /* LOAD_ANIM_FADE_ON */
    {
        (PageManager::lv_anim_setter_t)PageManager::lv_obj_set_opa_scale,
        (PageManager::lv_anim_getter_t)PageManager::lv_obj_get_opa_scale,

        /*Enter*/
        LV_OPA_TRANSP, LV_OPA_COVER, //Push
        LV_OPA_COVER, LV_OPA_COVER, //Pop

        /*Exit*/
        LV_OPA_COVER, LV_OPA_COVER, //Push
        LV_OPA_COVER, LV_OPA_TRANSP, //Pop
    },
};

PageManager::PageManager(PageFactory* factory)
{
    Factory = factory;
    _lv_ll_init(&PagePool_LL, sizeof(PageBasePtr_t));
    _lv_ll_init(&PageStack_LL, sizeof(PageBasePtr_t));

    CurrentNode = nullptr;
    PrevNode = nullptr;
    lv_obj_signal_cb = nullptr;
    memset(&AnimState, 0, sizeof(AnimState));
    AnimState.LoadAttr_Grp = LoadAnimAttrArr;

    SetGlobalLoadAnimType();
}

PageManager::~PageManager()
{
    _lv_ll_clear(&PagePool_LL);
    _lv_ll_clear(&PageStack_LL);
}

PageBase* PageManager::FindPage(lv_ll_t* ll, const char* name)
{
    PageBasePtr_t* node = FindPagePtr(ll, name);
    return node ? *node : nullptr;
}

PageManager::PageBasePtr_t* PageManager::FindPagePtr(lv_ll_t* ll, const char* name)
{
    void* i = nullptr;
    _LV_LL_READ((*ll), i)
    {
        PageBasePtr_t* node = (PageBasePtr_t*)i;

        if (strcmp(name, (*node)->Name) == 0)
        {
            return node;
        }
    }
    return nullptr;
}

PageBase* PageManager::FindPageInPool(const char* name)
{
    return FindPage(&PagePool_LL, name);
}

PageBase* PageManager::FindPageInStack(const char* name)
{
    return FindPage(&PageStack_LL, name);
}

PageBase* PageManager::Install(const char* className, const char* appName)
{
    if (Factory == nullptr)
    {
        PM_LOG_ERROR("Factory is not regsite, can't install page");
        return nullptr;
    }

    PageBase* base = Factory->CreatePage(className);
    if (base == nullptr)
    {
        PM_LOG_ERROR("Factory has not %s", className);
        return nullptr;
    }

    base->root = nullptr;
    base->ID = 0;
    base->Manager = nullptr;
    base->UserData = nullptr;
    memset(&base->priv, 0, sizeof(base->priv));

    if (appName == nullptr)
    {
        PM_LOG_WARN("appName has not set");
        appName = className;
    }

    PM_LOG_INFO("Install Page[class = %s, name = %s]", className, appName);
    Register(base, appName);

    return base;
}

bool PageManager::Uninstall(const char* appName)
{
    PM_LOG_INFO("Page(%s) uninstall...", appName);

    PageBase* base = FindPageInPool(appName);
    if (base == nullptr)
    {
        PM_LOG_ERROR("Page(%s) was not found", appName);
        return false;
    }

    if (!Unregister(appName))
    {
        PM_LOG_ERROR("Page(%s) unregister failed", appName);
        return false;
    }

    if (base->priv.IsCacheEnable)
    {
        PM_LOG_WARN("Page(%s) has cache, unloading...", appName);
        base->priv.State = PageBase::PAGE_STATE_UNLOAD;
        StateUpdate(base);
    }
    else
    {
        PM_LOG_INFO("Page(%s) has not cache", appName);
    }

    delete base;
    PM_LOG_INFO("Uninstall OK");
    return true;
}

bool PageManager::Register(PageBase* base, const char* name)
{
    if (FindPageInPool(name) != nullptr)
    {
        PM_LOG_ERROR("Page(%s) was multi registered", name);
        return false;
    }
        
    base->Manager = this;
    base->Name = name;

    PageBasePtr_t* node = (PageBasePtr_t*)_lv_ll_ins_tail(&PagePool_LL);
    *node = base;

    return true;
}

bool PageManager::Unregister(const char* name)
{
    PM_LOG_INFO("Page(%s) unregister...", name);

    PageBasePtr_t* node = FindPagePtr(&PageStack_LL, name);
    if (node != nullptr)
    {
        PM_LOG_ERROR("Page(%s) was in stack", name);
        return false;
    }

    node = FindPagePtr(&PagePool_LL, name);
    if (node == nullptr)
    {
        PM_LOG_ERROR("Page(%s) was not found", name);
        return false;
    }

    _lv_ll_remove(&PagePool_LL, node);
    lv_mem_free(node);
    PM_LOG_INFO("Unregister OK");
    return true;
}

PageManager::PageBasePtr_t* PageManager::GetStackTopPtr()
{
    return (PageBasePtr_t*)_lv_ll_get_tail(&PageStack_LL);
}

PageBase* PageManager::GetStackTop()
{
    PageBasePtr_t* top = GetStackTopPtr();

    if (top == nullptr)
    {
        return nullptr;
    }

    return *top;
}

void PageManager::SetStackClear()
{
}

PageBase* PageManager::GetStackTopAfter()
{
    PageBasePtr_t* top = GetStackTopPtr();

    if (top == nullptr)
        return nullptr;

    PageBasePtr_t* topAfter = (PageBasePtr_t*)_lv_ll_get_prev(&PageStack_LL, top);

    if (topAfter == nullptr)
        return nullptr;

    return *topAfter;
}

const char* PageManager::GetPrevNodeName()
{
    return PrevNode ? PrevNode->Name : "EMPTY_PAGE";
}
