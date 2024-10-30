/*
 * MIT License
 * Copyright (c) 2021 - 2023 _VIFEXTech
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
#include "../HAL/HAL_Log.h"
#include "DataProc.h"
#include "Version.h"

/* clang-format off */

/* Number to string macro */
#define _VERSION_NUM_TO_STR_(n)  #n
#define VERSION_NUM_TO_STR(n)   _VERSION_NUM_TO_STR_(n)

/* Compiler Version */
#if defined(_MSC_FULL_VER)
#  define VERSION_COMPILER      "MSVC v" VERSION_NUM_TO_STR(_MSC_FULL_VER)
#elif defined(__ARMCC_VERSION)
#  define VERSION_COMPILER      "ARMCC v" VERSION_NUM_TO_STR(__ARMCC_VERSION)
#elif defined(__GNUC__)
#  define VERSION_COMPILER      "GCC v"\
                                VERSION_NUM_TO_STR(__GNUC__)\
                                "."\
                                VERSION_NUM_TO_STR(__GNUC_MINOR__)\
                                "."\
                                VERSION_NUM_TO_STR(__GNUC_PATCHLEVEL__)
#else
#  define VERSION_COMPILER      "UNKNOW"
#endif

/* LVGL Version */
#include "lvgl/lvgl.h"
#define VERSION_GRAPHICS        "LVGL v"\
                                VERSION_NUM_TO_STR(LVGL_VERSION_MAJOR)\
                                "."\
                                VERSION_NUM_TO_STR(LVGL_VERSION_MINOR)\
                                "."\
                                VERSION_NUM_TO_STR(LVGL_VERSION_PATCH)\
                                " "\
                                LVGL_VERSION_INFO

/* clang-format on */

using namespace DataProc;

class DP_Version {
public:
    DP_Version(DataNode* node);

private:
    int onEvent(DataNode::EventParam_t* param);
    void getInfo(Version_Info_t* info);
    void dumpInfo(const Version_Info_t* info);
};

DP_Version::DP_Version(DataNode* node)
{
    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_Version*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PULL);

    Version_Info_t version;
    getInfo(&version);
    dumpInfo(&version);
}

int DP_Version::onEvent(DataNode::EventParam_t* param)
{
    if (param->size != sizeof(Version_Info_t)) {
        return DataNode::RES_SIZE_MISMATCH;
    }

    auto info = (Version_Info_t*)param->data_p;
    getInfo(info);

    return DataNode::RES_OK;
}

void DP_Version::getInfo(Version_Info_t* info)
{
    info->name = VERSION_FIRMWARE_NAME;
    info->software = VERSION_SOFTWARE;
    info->hardware = VERSION_HARDWARE;
    info->author = VERSION_AUTHOR_NAME;
    info->website = VERSION_WEBSITE;
    info->graphics = VERSION_GRAPHICS;
    info->compiler = VERSION_COMPILER;
    info->buildDate = __DATE__;
    info->buildTime = __TIME__;
}

void DP_Version::dumpInfo(const Version_Info_t* info)
{
    HAL_LOG_INFO("Firmware: %s", info->name);
    HAL_LOG_INFO("Software: %s", info->software);
    HAL_LOG_INFO("Hardware: %s", info->hardware);
    HAL_LOG_INFO("Author: %s", info->author);
    HAL_LOG_INFO("Website: %s", info->website);
    HAL_LOG_INFO("Graphics: %s", info->graphics);
    HAL_LOG_INFO("Compiler: %s", info->compiler);
    HAL_LOG_INFO("Build Time: %s %s", info->buildDate, info->buildTime);
}

DATA_PROC_DESCRIPTOR_DEF(Version)
