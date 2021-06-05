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
#ifndef __PAGE_MANAGER_H
#define __PAGE_MANAGER_H

#include "PageBase.h"
#include "PageFactory.h"
#include <vector>
#include <stack>

class PageManager {
public:
    typedef enum {
        LOAD_ANIM_GLOBAL = 0,
        LOAD_ANIM_OVER_LEFT,
        LOAD_ANIM_OVER_RIGHT,
        LOAD_ANIM_OVER_TOP,
        LOAD_ANIM_OVER_BOTTOM,
        LOAD_ANIM_MOVE_LEFT,
        LOAD_ANIM_MOVE_RIGHT,
        LOAD_ANIM_MOVE_TOP,
        LOAD_ANIM_MOVE_BOTTOM,
        LOAD_ANIM_FADE_ON,
        LOAD_ANIM_NONE,
        LOAD_ANIM_MAX = LOAD_ANIM_NONE
    } LoadAnim_t;

    typedef enum {
        ROOT_DRAG_DIR_NONE,
        ROOT_DRAG_DIR_HOR,
        ROOT_DRAG_DIR_VER,
    }RootDragDir_t;

    typedef lv_anim_exec_xcb_t lv_anim_setter_t;
    typedef lv_coord_t(*lv_anim_getter_t)(void*);

    typedef struct {
        /* 作为被进入方 */
        struct {
            lv_coord_t start;
            lv_coord_t end;
        }enter;

        /* 作为被退出方 */
        struct {
            lv_coord_t start;
            lv_coord_t end;
        }exit;
    }AnimValue_t;

    typedef struct {
        lv_anim_setter_t setter;
        lv_anim_getter_t getter;
        RootDragDir_t dragDir;
        AnimValue_t push;
        AnimValue_t pop;
    }LoadAnimAttr_t;

public:
    PageManager(PageFactory* factory = nullptr);
    ~PageManager();

    /* Loader */
    PageBase* Install(const char* className, const char* appName);
    bool Uninstall(const char* appName);
    bool Register(PageBase* base, const char* name);
    bool Unregister(const char* name);
  
    /* Router */
    PageBase* Push(const char* name, const PageBase::Stash_t* stash = nullptr);
    PageBase* Pop();
    bool BackHome();
    const char* GetPagePrevName();

    /* Global Anim */
    void SetGlobalLoadAnimType(
        LoadAnim_t anim = LOAD_ANIM_OVER_LEFT, 
        uint16_t time = 500, 
        lv_anim_path_cb_t path = lv_anim_path_ease_out
    );

private:
    /* Page Pool */
    PageBase* FindPageInPool(const char* name);

    /* Page Stack */
    PageBase* FindPageInStack(const char* name);
    PageBase* GetStackTop();
    PageBase* GetStackTopAfter();
    void SetStackClear(bool keepBottom = true);
    bool FourceUnload(PageBase* base);

    /* Anim */
    bool GetLoadAnimAttr(uint8_t anim, LoadAnimAttr_t* attr);
    bool GetIsOverAnim(uint8_t anim)
    {
        return (anim >= LOAD_ANIM_OVER_LEFT && anim <= LOAD_ANIM_OVER_BOTTOM);
    }
    bool GetIsMoveAnim(uint8_t anim)
    {
        return (anim >= LOAD_ANIM_MOVE_LEFT && anim <= LOAD_ANIM_MOVE_BOTTOM);
    }
    void AnimDefaultInit(lv_anim_t* a);
    bool GetCurrentLoadAnimAttr(LoadAnimAttr_t* attr)
    {
        return GetLoadAnimAttr(GetCurrentLoadAnimType(), attr);
    }
    LoadAnim_t GetCurrentLoadAnimType()
    {
        return (LoadAnim_t)AnimState.Current.Type;
    }

    /* Root */
    static void onRootDragEvent(lv_event_t* event);
    static void onRootAnimFinish(lv_anim_t* a);
    static void onRootAsyncLeave(void* base);
    void RootEnableDrag(lv_obj_t* root);
    static void RootGetDragPredict(lv_coord_t* x, lv_coord_t* y);

    /* Switch */
    void SwitchTo(PageBase* base, bool isPushAct, const PageBase::Stash_t* stash = nullptr);
    static void onSwitchAnimFinish(lv_anim_t* a);
    void SwitchAnimCreate(PageBase* base);
    void SwitchAnimTypeUpdate(PageBase* base);
    bool SwitchReqCheck();
    bool SwitchAnimStateCheck();

    /* State */
    PageBase::State_t StateLoadExecute(PageBase* base);
    PageBase::State_t StateWillAppearExecute(PageBase* base);
    PageBase::State_t StateDidAppearExecute(PageBase* base);
    PageBase::State_t StateWillDisappearExecute(PageBase* base);
    PageBase::State_t StateDidDisappearExecute(PageBase* base);
    PageBase::State_t StateUnloadExecute(PageBase* base);
    void StateUpdate(PageBase* base);
    PageBase::State_t GetState()
    {
        return PageCurrent->priv.State;
    }
    
private:
    PageFactory* Factory;

    std::vector<PageBase*> PagePool;
    std::stack<PageBase*> PageStack;

    PageBase* PagePrev;
    PageBase* PageCurrent;

    struct {
        bool IsSwitchReq;
        bool IsBusy;
        bool IsPushing;

        PageBase::AnimAttr_t Current;
        PageBase::AnimAttr_t Global;
    }AnimState;
};

#endif
