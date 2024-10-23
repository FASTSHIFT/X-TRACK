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

#if defined(ARDUINO) || defined(NDEBUG)

#include "lvgl/lvgl.h"

typedef void* (*alloc_func_t)(size_t);

static void* first_alloc(size_t size);

static alloc_func_t alloc_func = first_alloc;

static void* first_alloc(size_t size)
{
    if(!lv_is_initialized())
    {
        lv_init();
    }

    alloc_func = lv_mem_alloc;
    return lv_mem_alloc(size);;
}

void *operator new(size_t size)
{
    return alloc_func(size);
}

void *operator new[](size_t size)
{
    return alloc_func(size);
}

void operator delete(void* ptr)
{
    lv_mem_free(ptr);
}

void operator delete[](void* ptr)
{
    lv_mem_free(ptr);
}

#endif
