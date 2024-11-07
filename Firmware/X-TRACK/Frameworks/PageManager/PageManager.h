/*
 * MIT License
 * Copyright (c) 2021 - 2024 _VIFEXTech
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
#ifndef __PAGE_MANAGER_H
#define __PAGE_MANAGER_H

#include "PageBase.h"
#include "PageDef.h"
#include "PageFactory.h"
#include <stack>
#include <vector>

class PageManager {
    friend class PageBase;

public:
    enum class EVENT_TYPE {
        PAGE_STATE_CHANGED, /* param: PageBase::STATE */
        PAGE_PUSH, /* param: const char* */
        PAGE_POP, /* param: None */
        PAGE_REPLACE, /* param: const char* */
        PAGE_BACK_HOME, /* param: None */
    };

    enum class RES_TYPE {
        OK,
        STOP_PROCESS,
        ERR_NOT_FOUND,
        ERR_DUPLICATE,
        ERR_BUSY,
        ERR_PARAM,
        ERR_OPERATION,
        ERR_UNKNOWN,
    };

    struct EVENT {
        EVENT(EVENT_TYPE t, void* p, void* u)
            : type(t)
            , param(p)
            , userData(u)
        {
        }
        EVENT_TYPE type;
        void* param;
        void* userData;
    };

    typedef RES_TYPE (*EVENT_CALLBACK)(EVENT* event);

public:
    PageManager(PageFactory* factory = nullptr);
    ~PageManager();

    /**
     * @brief  Install the page, and register the page to the page pool
     * @param  className: The class name of the page
     * @param  appName: Page application name, no duplicates allowed
     * @retval Return true if successful
     */
    RES_TYPE install(const char* className, const char* appName);

    /**
     * @brief  Uninstall page
     * @param  appName: Page application name, no duplicates allowed
     * @retval Return true if the uninstallation is successful
     */
    RES_TYPE uninstall(const char* appName);

    /**
     * @brief  Register the page to the page pool
     * @param  name: Page application name, duplicate registration is not allowed
     * @retval Return true if the registration is successful
     */
    RES_TYPE registerPage(PageBase* base, const char* name);

    /**
     * @brief  Log out the page from the page pool
     * @param  name: Page application name
     * @retval Return true if the logout is successful
     */
    RES_TYPE unregisterPage(const char* name);

    /**
     * @brief  Enter a new page, replace the old page
     * @param  name: The name of the page to enter
     * @param  param: Parameters passed to the new page
     * @retval Return true if successful
     */
    RES_TYPE replace(const char* name, const PageBase::PARAM* param = nullptr);

    /**
     * @brief  Enter a new page, the old page is pushed onto the stack
     * @param  name: The name of the page to enter
     * @param  param: Parameters passed to the new page
     * @retval Return true if successful
     */
    RES_TYPE push(const char* name, const PageBase::PARAM* param = nullptr);

    /**
     * @brief  Pop the current page
     * @param  None
     * @retval Return true if successful
     */
    RES_TYPE pop();

    /**
     * @brief  Back to the main page (the page at the bottom of the stack)
     * @param  None
     * @retval Return true if successful
     */
    RES_TYPE backHome();

    /**
     * @brief  Get the name of the previous page
     * @param  None
     * @retval The name of the previous page, if it does not exist, return PM_EMPTY_PAGE_NAME
     */
    const char* getPagePrevName();

    /**
     * @brief  Get page stack depth
     * @param  None
     * @retval The depth of the page stack
     */
    size_t getStackDepth()
    {
        return _pageStack.size();
    }

    /**
     * @brief  Set global animation properties
     * @param  anim: Animation type
     * @param  time: Animation duration
     * @param  path: Animation curve
     * @retval None
     */
    void setGlobalLoadAnim(
        PAGE_ANIM anim,
        uint32_t time = PAGE_ANIM_TIME_DEFAULT,
        lv_anim_path_cb_t path = PAGE_ANIM_PATH_DEFAULT);

    /**
     * @brief  Set the root parent of the page
     * @param  par: The parent of the root, if it is nullptr, the parent is lv_scr_act()
     * @retval None
     */
    void setRootParent(lv_obj_t* par)
    {
        _rootParent = par;
    }

    /**
     * @brief  Get the root parent of the page
     * @param  None
     * @retval The parent of the root
     */
    lv_obj_t* getRootParent()
    {
        return (_rootParent ? _rootParent : lv_scr_act());
    }

    /**
     * @brief  Set the default style of the root
     * @param  style: The default style of the root
     * @retval None
     */
    void setRootDefaultStyle(lv_style_t* style)
    {
        _rootDefaultStyle = style;
    }

    /**
     * @brief  Set the top layer of the page
     * @param  obj: The top layer of the page
     * @retval None
     */
    void setLayerTop(lv_obj_t* obj)
    {
        _layerTop = obj;
    }

    /**
     * @brief  Get the top layer of the page
     * @param  None
     * @retval The top layer of the page
     */
    lv_obj_t* getLayerTop()
    {
        return _layerTop ? _layerTop : lv_layer_top();
    }

    /**
     * @brief  Set the event callback
     * @param  callback: Event callback
     * @param  userData: User data
     * @retval None
     */
    void setEventCallback(EVENT_CALLBACK callback, void* userData);

private:
    /* Animation switching param  */
    struct ANIM_PARAM {
        ANIM_PARAM()
            : enter { 0 }
            , exit { 0 }

        {
        }
        /* As the entered party */
        struct
        {
            int32_t start;
            int32_t end;
        } enter;

        /* As the exited party */
        struct
        {
            int32_t start;
            int32_t end;
        } exit;
    };

    /* Page loading animation properties */
    struct LOAD_ANIM_ATTR {
        LOAD_ANIM_ATTR()
            : setter(nullptr)
            , getter(nullptr)
        {
        }
        void (*setter)(void*, int32_t);
        int32_t (*getter)(void*);
        ANIM_PARAM push;
        ANIM_PARAM pop;
    };

private:
    /* Page factory */
    PageFactory* _factory;

    /* Page count */
    uint16_t _pageCnt;

    /* Page pool */
    std::vector<PageBase*> _pagePool;

    /* Page stack */
    std::stack<PageBase*> _pageStack;

    /* Previous page */
    PageBase* _pagePrev;

    /* The current page */
    PageBase* _pageCurrent;

    /* Page animation status */
    struct
    {
        bool isSwitchReq; // Has switch request
        bool isBusy; // Is switching
        bool isEntering; // Is in entering action

        PageBase::ANIM_ATTR current; // Current animation properties
        PageBase::ANIM_ATTR global; // global animation properties
    } _animState;

    /* Root */
    lv_obj_t* _rootParent;
    lv_style_t* _rootDefaultStyle;

    /* Top layer */
    lv_obj_t* _layerTop;

    /* Event */
    EVENT_CALLBACK _eventCallback;
    void* _eventUserData;

private:
    /**
     * @brief  Search pages in the page pool
     * @param  name: Page name
     * @retval A pointer to the base class of the page, or nullptr if not found
     */
    PageBase* findPageInPool(const char* name);

    /**
     * @brief  Search pages in the page stack
     * @param  name: Page name
     * @retval A pointer to the base class of the page, or nullptr if not found
     */
    PageBase* findPageInStack(const char* name);

    /**
     * @brief  Get the top page of the page stack
     * @param  None
     * @retval A pointer to the base class of the page
     */
    PageBase* getStackTop();

    /**
     * @brief  Get the page below the top of the page stack
     * @param  None
     * @retval A pointer to the base class of the page
     */
    PageBase* getStackTopAfter();

    /**
     * @brief  Clear the page stack and end the life cycle of all pages in the page stack
     * @param  keepBottom: Whether to keep the bottom page of the stack
     * @retval None
     */
    void clearStack(bool keepBottom = false);

    /**
     * @brief  Force the end of the life cycle of the page without animation
     * @param  base: Pointer to the page being executed
     * @retval Return true if successful
     */
    RES_TYPE fourceUnload(PageBase* base);

    /**
     * @brief  Get page loading animation properties
     * @param  anim: Animation type
     * @param  attr: Pointer to attribute
     * @retval Whether the acquisition is successful
     */
    RES_TYPE getLoadAnimAttr(PAGE_ANIM anim, LOAD_ANIM_ATTR* attr);

    /**
     * @brief  Set animation default parameters
     * @param  a: Pointer to animation
     * @retval None
     */
    void animDefaultInit(lv_anim_t* a);

    /**
     * @brief  Get the current page loading animation properties
     * @param  attr: Pointer to attribute
     * @retval Whether the acquisition is successful
     */
    RES_TYPE getCurrentLoadAnimAttr(LOAD_ANIM_ATTR* attr)
    {
        return getLoadAnimAttr(getCurrentLoadAnimType(), attr);
    }

    /**
     * @brief  Get the current page loading animation type
     * @param  None
     * @retval The current page loading animation type
     */
    PAGE_ANIM getCurrentLoadAnimType()
    {
        return _animState.current.type;
    }

    /**
     * @brief  Page switching
     * @param  newNode: Pointer to new page
     * @param  isEnterAct: Whether it is a ENTER action
     * @param  param: Parameters passed to the new page
     * @retval Return true if successful
     */
    RES_TYPE switchTo(PageBase* base, bool isEnterAct, const PageBase::PARAM* param = nullptr);

    /**
     * @brief  PPage switching animation execution end callback
     * @param  a: Pointer to animation
     * @retval None
     */
    static void onSwitchAnimFinish(lv_anim_t* a);

    /**
     * @brief  Create page switching animation
     * @param  a: Point to the animated page
     * @retval None
     */
    void switchAnimCreate(PageBase* base);

    /**
     * @brief  Update current animation properties, apply page custom animation
     * @param  base: Pointer to page
     * @retval None
     */
    void switchAnimTypeUpdate(PageBase* base);

    /**
     * @brief  Page switching request check
     * @param  None
     * @retval Return true if all pages are executed
     */
    RES_TYPE switchReqCheck();

    /**
     * @brief  Check if the page switching animation is being executed
     * @param  None
     * @retval Return true if it is executing
     */
    RES_TYPE switchAnimStateCheck();

    /**
     * @brief  Page loading status
     * @param  base: Pointer to the updated page
     * @retval Next state
     */
    PageBase::STATE stateLoad(PageBase* base);

    /**
     * @brief  The page is about to show the status
     * @param  base: Pointer to the updated page
     * @retval Next state
     */
    PageBase::STATE stateWillAppear(PageBase* base);

    /**
     * @brief  The status of the page display
     * @param  base: Pointer to the updated page
     * @retval Next state
     */
    PageBase::STATE stateDidAppear(PageBase* base);

    /**
     * @brief  The page is about to disappear
     * @param  base: Pointer to the updated page
     * @retval Next state
     */
    PageBase::STATE stateWillDisappear(PageBase* base);

    /**
     * @brief  Page disappeared end state
     * @param  base: Pointer to the updated page
     * @retval Next state
     */
    PageBase::STATE stateDidDisappear(PageBase* base);

    /**
     * @brief  Page unload complete
     * @param  base: Pointer to the updated page
     * @retval Next state
     */
    PageBase::STATE stateUnload(PageBase* base);

    /**
     * @brief  Update page state machine
     * @param  base: Pointer to the updated page
     * @retval None
     */
    void stateNext(PageBase* base);

    /**
     * @brief  Get the current state of the page
     * @param  None
     * @retval The current state of the page
     */
    PageBase::STATE getState()
    {
        return _pageCurrent->_context.state;
    }

    /**
     * @brief  Send event
     * @param  eventType: Event type
     * @param  param: Event parameters
     * @retval None
     */
    RES_TYPE sendEvent(EVENT_TYPE eventType, void* param = nullptr);
};

#endif /* __PAGE_BASE_H */
