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
#ifndef __PAGE_LOG_H
#define __PAGE_LOG_H

#define PAGE_LOG_LEVEL_INFO 0
#define PAGE_LOG_LEVEL_WARN 1
#define PAGE_LOG_LEVEL_ERROR 2
#define PAGE_LOG_LEVEL_OFF 3

#ifndef PAGE_LOG_LEVEL
#define PAGE_LOG_LEVEL PAGE_LOG_LEVEL_OFF
#endif

#if (PAGE_LOG_LEVEL < PAGE_LOG_LEVEL_OFF)

void PageLog(int level, const char* func, const char* format, ...);

#define PAGE_LOG_INFO(...) PageLog(PAGE_LOG_LEVEL_INFO, __func__, __VA_ARGS__)
#define PAGE_LOG_WARN(...) PageLog(PAGE_LOG_LEVEL_WARN, __func__, __VA_ARGS__)
#define PAGE_LOG_ERROR(...) PageLog(PAGE_LOG_LEVEL_ERROR, __func__, __VA_ARGS__)
#else
#define PAGE_LOG_INFO(...)
#define PAGE_LOG_WARN(...)
#define PAGE_LOG_ERROR(...)
#endif

#endif
