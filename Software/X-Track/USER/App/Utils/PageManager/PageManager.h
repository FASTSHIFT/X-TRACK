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

class PageManager {
    typedef PageBase* PageBasePtr_t;

public:
    PageManager(PageFactory* factory = nullptr);
    ~PageManager();

    PageBase* Install(const char* className, const char* appName);
    bool Uninstall(const char* appName);
    bool Register(PageBase* base, const char* name);
    bool Unregister(const char* name);
  
    PageBase* Push(const char* name, const PageBase::Stash_t* stash = nullptr);
    PageBase* Pop(const PageBase::Stash_t* stash = nullptr);
    PageBase* Back(const PageBase::Stash_t* stash = nullptr)
    {
        return Pop(stash);
    }
    PageBase* Replace(const char* name, const PageBase::Stash_t* stash = nullptr);
    void SwitchTo(PageBase* base, const PageBase::Stash_t* stash = nullptr);
    PageBase::State_t GetState()
    {
        return CurrentNode->priv.State;
    }

    PageBase* FindPage(lv_ll_t* ll, const char* name);
    PageBasePtr_t* FindPagePtr(lv_ll_t* ll, const char* name);
    PageBase* FindPageInPool(const char* name);
    PageBase* FindPageInStack(const char* name);
    PageBasePtr_t* GetStackTopPtr();
    PageBase* GetStackTop();
    PageBase* GetStackTopAfter();
    void SetStackClear();
    int GetStackLength();
    void GetStackPages(const char* buff, size_t len);
    const char* GetPrevNodeName();

    typedef enum {
        LOAD_ANIM_GLOBAL = 0,
        LOAD_ANIM_NONE,
        LOAD_ANIM_OVER_LEFT,
        LOAD_ANIM_OVER_RIGHT,
        LOAD_ANIM_OVER_TOP,
        LOAD_ANIM_OVER_BOTTOM,
        LOAD_ANIM_MOVE_LEFT,
        LOAD_ANIM_MOVE_RIGHT,
        LOAD_ANIM_MOVE_TOP,
        LOAD_ANIM_MOVE_BOTTOM,
        LOAD_ANIM_FADE_ON,
        LOAD_ANIM_MAX
    } LoadAnim_t;

    typedef lv_anim_exec_xcb_t lv_anim_setter_t;
    typedef lv_anim_value_t(*lv_anim_getter_t)(void*);
    typedef struct {
        lv_anim_setter_t anim_setter;
        lv_anim_getter_t anim_getter;

        /*在Push动作时，作为被进入方*/
        lv_anim_value_t PushEnterStart;
        lv_anim_value_t PushEnterEnd;

        /*在Pop动作时，作为被进入方*/
        lv_anim_value_t PopEnterStart;
        lv_anim_value_t PopEnterEnd;

        /*在Push动作时，作为被退出方*/
        lv_anim_value_t PushExitStart;
        lv_anim_value_t PushExitEnd;

        /*在Pop动作时，作为被退出方*/
        lv_anim_value_t PopExitStart;
        lv_anim_value_t PopExitEnd;
    }LoadAnimAttr_t;

    bool GetIsOverAnim(LoadAnim_t anim)
    {
        return (anim >= LOAD_ANIM_OVER_LEFT && anim <= LOAD_ANIM_OVER_BOTTOM);
    }

    bool GetIsMoveAnim(LoadAnim_t anim)
    {
        return (anim >= LOAD_ANIM_MOVE_LEFT && anim <= LOAD_ANIM_MOVE_BOTTOM);
    }

    void SetGlobalLoadAnimType(LoadAnim_t anim = LOAD_ANIM_OVER_LEFT, uint16_t time = 500, lv_anim_path_cb_t path = lv_anim_path_ease_out);
    void AnimDefaultInit(lv_anim_t* a);

    const LoadAnimAttr_t* GetCurrentLoadAnimAttr()
    {
        return &(AnimState.LoadAnimAttr_Grp[AnimState.TypeCurrent]);
    }

    LoadAnim_t GetCurrentLoadAnimType()
    {
        return AnimState.TypeCurrent;
    }

    static void lv_obj_set_opa_scale(lv_obj_t* obj, lv_anim_value_t opa_scale);
    static lv_anim_value_t lv_obj_get_opa_scale(lv_obj_t* obj);
    
private:
    PageFactory* Factory;

    lv_ll_t PagePool_LL;
    lv_ll_t PageStack_LL;
    PageBase* PrevNode;
    PageBase* CurrentNode;

    struct {
        bool IsSwitchReq;
        bool IsBusy;
        LoadAnim_t TypeCurrent;
        LoadAnim_t TypeGlobal;
        uint16_t Time;
        lv_anim_path_cb_t Path;
        bool IsPushing;
        const LoadAnimAttr_t* LoadAnimAttr_Grp;
    }AnimState;

    static lv_res_t onRootSignal(lv_obj_t* obj, lv_signal_t signal, void* param);
    static void onRootAnimFinish(lv_anim_t* a);
    static void RootAsyncCall(void* user_data);
    void RootEnableDrag(lv_obj_t* root, lv_drag_dir_t drag_dir);
    static void RootGetDragPredict(lv_coord_t* x, lv_coord_t* y);
    lv_signal_cb_t lv_obj_signal_cb;

    static void onSwitchAnimFinish(lv_anim_t* a);
    void SwitchAnimCreate(PageBase* base);
    void SwitchAnimTypeUpdate(PageBase* base);
    bool SwitchReqCheck();

    PageBase::State_t StateLoadExecute(PageBase* base);
    PageBase::State_t StateWillAppearExecute(PageBase* base);
    PageBase::State_t StateDidAppearExecute(PageBase* base);
    PageBase::State_t StateWillDisappearExecute(PageBase* base);
    PageBase::State_t StateDidDisappearExecute(PageBase* base);
    PageBase::State_t StateUnloadExecute(PageBase* base);
    void StateUpdate(PageBase* base);
};

#endif
