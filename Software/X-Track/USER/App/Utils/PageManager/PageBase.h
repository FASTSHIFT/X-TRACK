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
#define PAGE_STASH_POP(data)\
do{\
    if (priv.Stash.ptr != nullptr && priv.Stash.size == sizeof(data))\
    {\
        memcpy(&(data), priv.Stash.ptr, priv.Stash.size);\
    }\
}while(0)

class PageManager;

class PageBase {
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
        priv.CacheEnableReq = en;
    }

    void SetCustomAutoCacheEnable(bool en)
    {
        priv.DisableAutoCacheReq = !en;
    }

    void SetCustomLoadAnimType(uint8_t animType)
    {
        priv.Anim.Type = animType;
    }

    lv_obj_t* root;
    typedef struct {
        PageBase* base;
    }root_ext_attr_t;

    PageManager* Manager;
    const char* Name;
    uint16_t ID;
    void* UserData;

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

    struct {
        bool CacheEnableReq;
        bool DisableAutoCacheReq;
        bool IsDisableAutoCache; 
        bool IsCacheEnable;
        struct {
            bool IsEnter;
            bool IsBusy;
            uint8_t Type;
        }Anim;  
        Stash_t Stash;
        State_t State;
    }priv;
};

#endif // ! __PAGE_BASE_H
