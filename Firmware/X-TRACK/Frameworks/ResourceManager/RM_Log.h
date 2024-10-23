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
#ifndef __RM_LOG_H
#define __RM_LOG_H

#define RESOURCE_MANAGER_USE_LOG 1

#if !defined(ARDUINO) && RESOURCE_MANAGER_USE_LOG
#include <stdio.h>
#  define _RM_LOG(format, ...)      printf("[RM]" format "\r\n", ##__VA_ARGS__)
#  define RM_LOG_INFO(format, ...)  //_RM_LOG("[Info] " format, ##__VA_ARGS__)
#  define RM_LOG_WARN(format, ...)  _RM_LOG("[Warn] " format, ##__VA_ARGS__)
#  define RM_LOG_ERROR(format, ...) _RM_LOG("[Error] " format, ##__VA_ARGS__)
#else
#  define RM_LOG_INFO(...)
#  define RM_LOG_WARN(...)
#  define RM_LOG_ERROR(...)
#endif

#endif
