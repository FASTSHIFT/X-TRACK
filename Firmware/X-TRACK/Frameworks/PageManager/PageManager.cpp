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
#include "PageManager.h"
#include "PageLog.h"
#include <algorithm>
#include <string.h>

#define PM_EMPTY_PAGE_NAME "EMPTY_PAGE"

PageManager::PageManager(PageFactory* factory)
    : _factory(factory)
    , _pageCnt(0)
    , _pagePrev(nullptr)
    , _pageCurrent(nullptr)
    , _animState { 0 }
    , _rootParent(nullptr)
    , _rootDefaultStyle(nullptr)
    , _layerTop(nullptr)
    , _eventCallback(nullptr)
    , _eventUserData(nullptr)
{
    setGlobalLoadAnim(PAGE_ANIM::OVER_LEFT);
}

PageManager::~PageManager()
{
    clearStack();
}

PageBase* PageManager::findPageInPool(const char* name)
{
    for (auto iter : _pagePool) {
        if (strcmp(name, iter->_name) == 0) {
            return iter;
        }
    }
    return nullptr;
}

PageBase* PageManager::findPageInStack(const char* name)
{
    decltype(_pageStack) stk = _pageStack;
    while (!stk.empty()) {
        PageBase* base = stk.top();

        if (strcmp(name, base->_name) == 0) {
            return base;
        }

        stk.pop();
    }

    return nullptr;
}

PageManager::RES_TYPE PageManager::install(const char* className, const char* appName)
{
    if (!_factory) {
        PAGE_LOG_ERROR("Factory was not registered, can't install page");
        return RES_TYPE::ERR_NOT_FOUND;
    }

    if (!appName) {
        PAGE_LOG_WARN("appName has not set");
        appName = className;
    }

    if (findPageInPool(appName) != nullptr) {
        PAGE_LOG_ERROR("Page(%s) was registered", appName);
        return RES_TYPE::ERR_DUPLICATE;
    }

    PageBase* base = _factory->create(className);
    if (!base) {
        PAGE_LOG_ERROR("Factory has not %s", className);
        return RES_TYPE::ERR_UNKNOWN;
    }

    base->_id = _pageCnt++;

    PAGE_LOG_INFO("Install Page[class = %s, name = %s]", className, appName);
    auto res = registerPage(base, appName);
    if (res == RES_TYPE::OK) {
        base->onInstalled();
    }

    return res;
}

PageManager::RES_TYPE PageManager::uninstall(const char* appName)
{
    PAGE_LOG_INFO("Page(%s) uninstall...", appName);

    PageBase* base = findPageInPool(appName);
    if (!base) {
        PAGE_LOG_ERROR("Page(%s) was not found", appName);
        return RES_TYPE::ERR_NOT_FOUND;
    }

    auto res = unregisterPage(appName);
    if (res != RES_TYPE::OK) {
        PAGE_LOG_ERROR("Page(%s) unregister failed", appName);
        return res;
    }

    if (base->_context.isCached) {
        PAGE_LOG_WARN("Page(%s) has cached, unloading...", appName);
        base->_context.state = PageBase::STATE::UNLOAD;
        stateNext(base);
        stateNext(base);
        stateNext(base);
        stateNext(base);
        stateNext(base);
    } else {
        PAGE_LOG_INFO("Page(%s) has not cache", appName);
    }

    delete base;
    PAGE_LOG_INFO("Uninstall OK");
    return RES_TYPE::OK;
}

PageManager::RES_TYPE PageManager::registerPage(PageBase* base, const char* name)
{
    if (findPageInPool(name) != nullptr) {
        PAGE_LOG_ERROR("Page(%s) was multi registered", name);
        return RES_TYPE::ERR_DUPLICATE;
    }

    base->_manager = this;
    base->_name = name;

    _pagePool.push_back(base);

    return RES_TYPE::OK;
}

PageManager::RES_TYPE PageManager::unregisterPage(const char* name)
{
    PAGE_LOG_INFO("Page(%s) unregister...", name);

    PageBase* base = findPageInStack(name);

    if (base != nullptr) {
        PAGE_LOG_ERROR("Page(%s) was in stack", name);
        return RES_TYPE::ERR_OPERATION;
    }

    base = findPageInPool(name);
    if (!base) {
        PAGE_LOG_ERROR("Page(%s) was not found", name);
        return RES_TYPE::ERR_NOT_FOUND;
    }

    auto iter = std::find(_pagePool.begin(), _pagePool.end(), base);

    if (iter == _pagePool.end()) {
        PAGE_LOG_ERROR("Page(%s) was not found in PagePool", name);
        return RES_TYPE::ERR_NOT_FOUND;
    }

    _pagePool.erase(iter);

    PAGE_LOG_INFO("Unregister OK");
    return RES_TYPE::OK;
}

PageBase* PageManager::getStackTop()
{
    return _pageStack.empty() ? nullptr : _pageStack.top();
}

PageBase* PageManager::getStackTopAfter()
{
    PageBase* top = getStackTop();

    if (!top) {
        return nullptr;
    }

    _pageStack.pop();

    PageBase* topAfter = getStackTop();

    _pageStack.push(top);

    return topAfter;
}

void PageManager::clearStack(bool keepBottom)
{
    while (1) {
        PageBase* top = getStackTop();

        if (!top) {
            PAGE_LOG_INFO("Page stack is empty, breaking...");
            break;
        }

        PageBase* topAfter = getStackTopAfter();

        if (!topAfter) {
            if (keepBottom) {
                _pagePrev = top;
                PAGE_LOG_INFO("Keep page stack bottom(%s), breaking...", top->_name);
                break;
            } else {
                _pagePrev = nullptr;
            }
        }

        fourceUnload(top);

        _pageStack.pop();
    }
    PAGE_LOG_INFO("Stack clear done");
}

const char* PageManager::getPagePrevName()
{
    return _pagePrev ? _pagePrev->_name : PM_EMPTY_PAGE_NAME;
}

void PageManager::setEventCallback(EVENT_CALLBACK callback, void* userData)
{
    _eventCallback = callback;
    _eventUserData = userData;
}

PageManager::RES_TYPE PageManager::sendEvent(EVENT_TYPE eventType, void* param)
{
    if (!_eventCallback) {
        return RES_TYPE::OK;
    }

    EVENT event(eventType, param, _eventUserData);
    return _eventCallback(&event);
}
