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
#ifndef __PM_LOG_H
#define __PM_LOG_H

#define PAGE_MANAGER_USE_LOG 1

#if !defined(ARDUINO) && PAGE_MANAGER_USE_LOG
#include <stdio.h>
#  define _PM_LOG(format, ...)      printf("[PM]" format "\r\n", ##__VA_ARGS__)
#  define PM_LOG_INFO(format, ...)  //_PM_LOG("[Info] " format, ##__VA_ARGS__)
#  define PM_LOG_WARN(format, ...)  _PM_LOG("[Warn] " format, ##__VA_ARGS__)
#  define PM_LOG_ERROR(format, ...) _PM_LOG("[Error] " format, ##__VA_ARGS__)
#else
#  define PM_LOG_INFO(...)
#  define PM_LOG_WARN(...)
#  define PM_LOG_ERROR(...)
#endif

#endif
