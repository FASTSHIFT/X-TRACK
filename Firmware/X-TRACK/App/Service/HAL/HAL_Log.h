/*
 * MIT License
 * Copyright (c) 2023 _VIFEXTech
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
#ifndef __HAL_LOG_H
#define __HAL_LOG_H

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_LOG_LEVEL_OFF -1
#define HAL_LOG_LEVEL_INFO 0
#define HAL_LOG_LEVEL_WARN 1
#define HAL_LOG_LEVEL_ERROR 2
#define _HAL_LOG_LEVEL_LAST 3

#ifndef HAL_LOG_LEVEL
#define HAL_LOG_LEVEL HAL_LOG_LEVEL_INFO
#endif

#if HAL_LOG_LEVEL > HAL_LOG_LEVEL_OFF

#if HAL_LOG_LEVEL <= HAL_LOG_LEVEL_INFO
#define HAL_LOG_INFO(...) HAL_Log(HAL_LOG_LEVEL_INFO, __func__, __VA_ARGS__)
#else
#define HAL_LOG_INFO(...)
#endif

#if HAL_LOG_LEVEL <= HAL_LOG_LEVEL_WARN
#define HAL_LOG_WARN(...) HAL_Log(HAL_LOG_LEVEL_WARN, __func__, __VA_ARGS__)
#else
#define HAL_LOG_WARN(...)
#endif

#if HAL_LOG_LEVEL <= HAL_LOG_LEVEL_ERROR
#define HAL_LOG_ERROR(...) HAL_Log(HAL_LOG_LEVEL_ERROR, __func__, __VA_ARGS__)
#else
#define HAL_LOG_ERROR(...)
#endif

#else
#define HAL_LOG_INFO(...)
#define HAL_LOG_WARN(...)
#define HAL_LOG_ERROR(...)
#endif

void HAL_Log_Init(void);
void HAL_Log_PrintString(const char* str);
void HAL_Log(uint8_t level, const char* func, const char* fmt, ...);
void HAL_Log_Printf(const char* fmt, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
