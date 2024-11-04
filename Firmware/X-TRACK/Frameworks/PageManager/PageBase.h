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
#ifndef __PAGE_BASE_H
#define __PAGE_BASE_H

#include "PageDef.h"
#include "lvgl/lvgl.h"

/* Generate page param */
#define PAGE_PARAM_MAKE(PARAM)  \
    {                           \
        &(PARAM), sizeof(PARAM) \
    }

/* Get page param */
#define PAGE_GET_PARAM(PARAM) this->getParam(&(PARAM), sizeof(PARAM))

class PageManager;

class PageBase {
    friend class PageManager;

public:
    /* Page statement */
    enum class STATE {
        IDLE,
        LOAD,
        WILL_APPEAR,
        DID_APPEAR,
        ACTIVITY,
        WILL_DISAPPEAR,
        DID_DISAPPEAR,
        UNLOAD,
    };

public:
    PageBase();
    virtual ~PageBase();

    /**
     * @brief  Page installation callback
     */
    virtual void onInstalled() { }

    /**
     * @brief  Page load start callback
     */
    virtual void onViewLoad() { }

    /**
     * @brief  Page load end callback
     */
    virtual void onViewDidLoad() { }

    /**
     * @brief  Page appear animation start callback
     */
    virtual void onViewWillAppear() { }

    /**
     * @brief  Page appear animation end callback
     */
    virtual void onViewDidAppear() { }

    /**
     * @brief  Page disappear animation start callback
     */
    virtual void onViewWillDisappear() { }

    /**
     * @brief  Page disappear animation end callback
     */
    virtual void onViewDidDisappear() { }

    /**
     * @brief  Page unload start callback
     */
    virtual void onViewUnload() { }

    /**
     * @brief  Page unload end callback
     */
    virtual void onViewDidUnload() { }

    /**
     * @brief  Set page cache enable
     * @param  en: Cache enable
     * @retval None
     */
    void setCacheEnable(bool en);

    /**
     * @brief  Set page automatic cache management enable
     * @param  en: Automatic cache management enable
     * @retval None
     */
    void setAutoCacheEnable(bool en);

    /**
     * @brief  Set page animation type
     * @param  type: Animation type
     * @retval None
     */
    void setLoadAnimType(PAGE_ANIM type);

    /**
     * @brief  Set page animation time
     * @param  time: Animation time
     * @retval None
     */
    void setLoadAnimTime(uint32_t time);

    /**
     * @brief  Set page animation path
     * @param  path: Animation path
     * @retval None
     */
    void setLoadAnimPath(lv_anim_path_cb_t path);

    /**
     * @brief  Set page back gesture direction
     * @param  dir: Back gesture direction, See lv_dir_t
     * @retval None
     */
    void setBackGestureDirection(lv_dir_t dir);

    /**
     * @brief  Get page parameter
     * @param  ptr: Parameter pointer
     * @param  size: Parameter size
     * @retval return true if success
     */
    bool getParam(void* ptr, uint32_t size);

    /**
     * @brief  Get page manager
     * @param  None
     * @retval Page manager pointer
     */
    PageManager* getManager() { return _manager; }

    /**
     * @brief  Get page name
     * @param  None
     * @retval Page name
     */
    const char* getName() { return _name; }

    /**
     * @brief  Get page ID
     * @param  None
     * @retval Page ID
     */
    uint16_t getID() { return _id; }

    /**
     * @brief  Get page root node
     * @param  None
     * @retval Page root node
     */
    lv_obj_t* getRoot() { return _root; }

protected:
    /* Page parameter */
    struct PARAM {
        void* ptr;
        uint32_t size;
    };

private:
    /* Page switching animation properties */
    struct ANIM_ATTR {
        ANIM_ATTR()
            : type(PAGE_ANIM::GLOBAL)
            , duration(PAGE_ANIM_TIME_DEFAULT)
            , path(PAGE_ANIM_PATH_DEFAULT)
        {
        }
        PAGE_ANIM type;
        uint32_t duration;
        lv_anim_path_cb_t path;
    };

    struct CONTEXT {
        bool reqEnableCache; /* Cache enable request */
        bool reqDisableAutoCache; /* Automatic cache management enable request */

        bool isDisableAutoCache; /* Whether it is automatic cache management */
        bool isCached; /* Cache enable */

        PARAM param; /* Page parameter  */
        STATE state; /* Page statement */

        struct
        {
            bool isEnter; /* Animation enter or exit */
            bool isBusy; /* Animation is busy */
            ANIM_ATTR attr; /* Animation properties */
        } anim;

        lv_dir_t backGestureDir; /* Back gesture direction */
    };

private:
    PageManager* _manager; /* Page manager pointer */
    const char* _name; /* Page name */
    uint16_t _id; /* Page ID */
    lv_obj_t* _root; /* UI root node */
    CONTEXT _context; /* Page context */
};

#endif /* __PAGE_BASE_H */
