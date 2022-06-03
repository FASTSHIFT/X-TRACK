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
#ifndef __VERSION_H
#define __VERSION_H

/* Firmware Version */
#define VERSION_FIRMWARE_NAME   "X-TRACK"
#define VERSION_SOFTWARE        "v2.7"
#define VERSION_HARDWARE        "v1.0"
#define VERSION_AUTHOR_NAME     "_VIFEXTech"
#define VERSION_PROJECT_LINK    "https://github.com/FASTSHIFT/X-TRACK"

/* Number to string macro */
#define _VERSION_NUM_TO_STR_(n)  #n
#define VERSION_NUM_TO_STR(n)   _VERSION_NUM_TO_STR_(n)

/* LVGL Version */
#include "lvgl/lvgl.h"
#define VERSION_LVGL            "v"\
                                VERSION_NUM_TO_STR(LVGL_VERSION_MAJOR)\
                                "."\
                                VERSION_NUM_TO_STR(LVGL_VERSION_MINOR)\
                                "."\
                                VERSION_NUM_TO_STR(LVGL_VERSION_PATCH)\
                                " "\
                                LVGL_VERSION_INFO


/* File System Version */
#ifdef ARDUINO
#  include "SdFat.h"
#  define VERSION_FILESYSTEM    "SDFAT v" VERSION_NUM_TO_STR(SD_FAT_VERSION)
#else
#  define VERSION_FILESYSTEM    "STDC"
#endif


/* Compiler Version */
#if defined(_MSC_FULL_VER)
#  define VERSION_COMPILER      "MSVC\nv" VERSION_NUM_TO_STR(_MSC_FULL_VER)
#elif defined(__ARMCC_VERSION)
#  define VERSION_COMPILER      "ARMCC\nv" VERSION_NUM_TO_STR(__ARMCC_VERSION)
#elif defined(__GNUC__)
#  define VERSION_COMPILER      "GCC\n"\
                                "v"\
                                VERSION_NUM_TO_STR(__GNUC__)\
                                "."\
                                VERSION_NUM_TO_STR(__GNUC_MINOR__)\
                                "."\
                                VERSION_NUM_TO_STR(__GNUC_PATCHLEVEL__)
#else
#  define VERSION_COMPILER      "UNKNOW"
#endif

/* Build Time */
#define VERSION_BUILD_TIME    __DATE__ "\n" __TIME__


#endif
