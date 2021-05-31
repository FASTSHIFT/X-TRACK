#ifndef __VERSION_H
#define __VERSION_H

#define _VERSION_NUM_TO_STR_(n)  #n
#define VERSION_NUM_TO_STR(n)   _VERSION_NUM_TO_STR_(n)

#define VERSION_FIRMWARE_NAME   "X-Track"
#define VERSION_SOFTWARE        "v0.6"
#define VERSION_HARDWARE        "v1.0"

#include "lvgl/lvgl.h"
#define VERSION_LVGL            "v"\
                                VERSION_NUM_TO_STR(LVGL_VERSION_MAJOR)\
                                "."\
                                VERSION_NUM_TO_STR(LVGL_VERSION_MINOR)\
                                "."\
                                VERSION_NUM_TO_STR(LVGL_VERSION_PATCH)\
                                " "\
                                LVGL_VERSION_INFO

#ifdef ARDUINO
#  include "SdFat.h"
#  define VERSION_FILESYSTEM    "SDFAT v" VERSION_NUM_TO_STR(SD_FAT_VERSION)
#else
#  define VERSION_FILESYSTEM    "STDC"
#endif

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

#define VERSION_COMPILE_TIME    __DATE__ "\n" __TIME__


#endif
