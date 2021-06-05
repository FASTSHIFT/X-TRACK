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
#ifndef __PAGE_BASE_H
#define __PAGE_BASE_H

#include "lvgl/lvgl.h"

#define PAGE_STASH_MAKE(data) {&(data), sizeof(data)}
#define PAGE_STASH_POP(data)  this->GetStash(&(data), sizeof(data))

class PageManager;

class PageBase {
public:
    typedef enum {
        PAGE_STATE_IDLE,
        PAGE_STATE_LOAD,
        PAGE_STATE_WILL_APPEAR,
        PAGE_STATE_DID_APPEAR,
        PAGE_STATE_ACTIVITY,
        PAGE_STATE_WILL_DISAPPEAR,
        PAGE_STATE_DID_DISAPPEAR,
        PAGE_STATE_UNLOAD,
        PAGE_STATE_MAX
    }State_t;

    typedef struct {
        void* ptr;
        uint32_t size;
    }Stash_t;

    typedef struct {
        uint8_t Type;
        uint16_t Time;
        lv_anim_path_cb_t Path;
    }AnimAttr_t;

public:
    lv_obj_t* root;
    PageManager* Manager;
    const char* Name;
    uint16_t ID;
    void* UserData;

    struct {
        bool ReqEnableCache;
        bool ReqDisableAutoCache;

        bool IsDisableAutoCache;
        bool IsCached;

        Stash_t Stash;
        State_t State;

        struct {
            bool IsEnter;
            bool IsBusy;
            AnimAttr_t Attr;
        }Anim;
    }priv;

public:
    virtual ~PageBase() {}

    virtual void onCustomAttrConfig() {}
    virtual void onViewLoad() {}
    virtual void onViewDidLoad() {}
    virtual void onViewWillAppear() {}
    virtual void onViewDidAppear() {}
    virtual void onViewWillDisappear() {}
    virtual void onViewDidDisappear() {}
    virtual void onViewDidUnload() {}
    
    void SetCustomCacheEnable(bool en)
    {
        SetCustomAutoCacheEnable(false);
        priv.ReqEnableCache = en;
    }

    void SetCustomAutoCacheEnable(bool en)
    {
        priv.ReqDisableAutoCache = !en;
    }

    void SetCustomLoadAnimType(
        uint8_t animType,
        uint16_t time = 500,
        lv_anim_path_cb_t path = lv_anim_path_ease_out
    )
    {
        priv.Anim.Attr.Type = animType;
        priv.Anim.Attr.Time = time;
        priv.Anim.Attr.Path = path;
    }

    bool GetStash(void* ptr, uint32_t size)
    {
        bool retval = false;
        if (priv.Stash.ptr != nullptr && priv.Stash.size == size)
        {
            memcpy(ptr, priv.Stash.ptr, priv.Stash.size);
            //lv_mem_free(priv.Stash.ptr);
            //priv.Stash.ptr = nullptr;
            retval = true;
        }
        return retval;
    }
};

#endif // ! __PAGE_BASE_H
