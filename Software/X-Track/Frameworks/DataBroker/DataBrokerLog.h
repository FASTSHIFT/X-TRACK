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
#ifndef __DATA_BROKER_LOG_H
#define __DATA_BROKER_LOG_H

#define DATA_BROKER_LOG_LEVEL_INFO 0
#define DATA_BROKER_LOG_LEVEL_WARN 1
#define DATA_BROKER_LOG_LEVEL_ERROR 2
#define DATA_BROKER_LOG_LEVEL_OFF 3

#ifndef DATA_BROKER_LOG_LEVEL
#define DATA_BROKER_LOG_LEVEL DATA_BROKER_LOG_LEVEL_OFF
#endif

#if (DATA_BROKER_LOG_LEVEL < DATA_BROKER_LOG_LEVEL_OFF)
#include <stdio.h>
#define _DN_LOG(format, ...) printf("[DN]" format "\r\n", ##__VA_ARGS__)
#endif

#if (DATA_BROKER_LOG_LEVEL <= DATA_BROKER_LOG_LEVEL_INFO)
#define DN_LOG_INFO(format, ...) _DN_LOG("[Info] " format, ##__VA_ARGS__)
#define DN_LOG_WARN(format, ...) _DN_LOG("[Warn] " format, ##__VA_ARGS__)
#define DN_LOG_ERROR(format, ...) _DN_LOG("[Error] " format, ##__VA_ARGS__)
#elif (DATA_BROKER_LOG_LEVEL <= DATA_BROKER_LOG_LEVEL_WARN)
#define DN_LOG_INFO(format, ...)
#define DN_LOG_WARN(format, ...) _DN_LOG("[Warn] " format, ##__VA_ARGS__)
#define DN_LOG_ERROR(format, ...) _DN_LOG("[Error] " format, ##__VA_ARGS__)
#elif (DATA_BROKER_LOG_LEVEL <= DATA_BROKER_LOG_LEVEL_ERROR)
#define DN_LOG_INFO(format, ...)
#define DN_LOG_WARN(format, ...)
#define DN_LOG_ERROR(format, ...) _DN_LOG("[Error] " format, ##__VA_ARGS__)
#else
#define DN_LOG_INFO(...)
#define DN_LOG_WARN(...)
#define DN_LOG_ERROR(...)
#endif

#endif
