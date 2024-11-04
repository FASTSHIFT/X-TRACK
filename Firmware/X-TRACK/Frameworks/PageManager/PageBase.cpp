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
#include "PageBase.h"
#include "PageLog.h"

PageBase::PageBase()
    : _manager(nullptr)
    , _name(nullptr)
    , _id(0)
    , _root(nullptr)
    , _context { 0 }
{
}

PageBase::~PageBase()
{
}

void PageBase::setCacheEnable(bool en)
{
    PAGE_LOG_INFO("Page(%s) enable %d", _name, en);
    setAutoCacheEnable(false);
    _context.reqEnableCache = en;
}

void PageBase::setAutoCacheEnable(bool en)
{
    PAGE_LOG_INFO("Page(%s) enable %d", _name, en);
    _context.reqDisableAutoCache = !en;
}

void PageBase::setLoadAnimType(PAGE_ANIM type)
{
    _context.anim.attr.type = type;
}

void PageBase::setLoadAnimTime(uint32_t time)
{
    _context.anim.attr.duration = time;
}

void PageBase::setLoadAnimPath(lv_anim_path_cb_t path)
{
    _context.anim.attr.path = path;
}

void PageBase::setBackGestureDirection(lv_dir_t dir)
{
    _context.backGestureDir = dir;
}

bool PageBase::getParam(void* ptr, uint32_t size)
{
    if (!_context.param.ptr) {
        PAGE_LOG_WARN("No param found");
        return false;
    }

    if (_context.param.size != size) {
        PAGE_LOG_WARN(
            "param[%p](%" PRIu32 ") does not match the size(%" PRIu32 ")",
            _context.param.ptr,
            _context.param.size,
            size);
        return false;
    }

    lv_memcpy(ptr, _context.param.ptr, _context.param.size);
    lv_free(_context.param.ptr);
    _context.param.ptr = nullptr;
    return true;
}
