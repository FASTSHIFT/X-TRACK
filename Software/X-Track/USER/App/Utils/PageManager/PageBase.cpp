/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
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
#include "PM_Log.h"

void PageBase::SetCustomCacheEnable(bool en)
{
    PM_LOG_INFO("Page(%s) %s = %d", _Name, __func__, en);
    SetCustomAutoCacheEnable(false);
    priv.ReqEnableCache = en;
}

void PageBase::SetCustomAutoCacheEnable(bool en)
{
    PM_LOG_INFO("Page(%s) %s = %d", _Name, __func__, en);
    priv.ReqDisableAutoCache = !en;
}

void PageBase::SetCustomLoadAnimType(
    uint8_t animType,
    uint16_t time,
    lv_anim_path_cb_t path
)
{
    priv.Anim.Attr.Type = animType;
    priv.Anim.Attr.Time = time;
    priv.Anim.Attr.Path = path;
}

bool PageBase::StashPop(void* ptr, uint32_t size)
{
    if (priv.Stash.ptr == nullptr)
    {
        PM_LOG_WARN("No Stash found");
        return false;
    }

    if (priv.Stash.size != size)
    {
        PM_LOG_WARN(
            "Stash[0x%p](%d) does not match the size(%d)",
            priv.Stash.ptr,
            priv.Stash.size,
            size
        );
        return false;
    }

    memcpy(ptr, priv.Stash.ptr, priv.Stash.size);
    lv_mem_free(priv.Stash.ptr);
    priv.Stash.ptr = nullptr;
    return false;
}

