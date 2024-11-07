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
#include "Config/Config.h"
#include "DataProc.h"
#include "lvgl/lvgl.h"
#include <stdlib.h>

#define MAP_LEVEL_MIN_DEFAULT 3
#define MAP_LEVEL_MAX_DEFAULT 16
#define MAP_LEVEL_MIN 0
#define MAP_LEVEL_MAX 19

using namespace DataProc;

class DP_MapInfo {
public:
    DP_MapInfo(DataNode* node);

private:
    Map_Info_t _info;
    char _path[32];
    char _ext[8];
    bool _isSearched;

private:
    int onEvent(DataNode::EventParam_t* param);
    bool searchDirLevel(const char* dirName, uint8_t* min, uint8_t* max);
};

DP_MapInfo::DP_MapInfo(DataNode* node)
    : _path { CONFIG_MAP_DIR_PATH_DEFAULT }
    , _ext { CONFIG_MAP_EXT_NAME_DEFAULT }
    , _isSearched(false)
{
    _info.path = _path;
    _info.ext = _ext;
    _info.levelMin = MAP_LEVEL_MIN_DEFAULT;
    _info.levelMax = MAP_LEVEL_MAX_DEFAULT;
    _info.coordTrans = false;

    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_MapInfo*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PULL | DataNode::EVENT_NOTIFY);

    Storage_Helper storage(node);
    storage.structStart("map");
    storage.add("path", _path, sizeof(_path), STORAGE_TYPE::STRING);
    storage.add("ext", _ext, sizeof(_ext), STORAGE_TYPE::STRING);
    storage.add("coordTrans", &_info.coordTrans);
    storage.structEnd();
}

int DP_MapInfo::onEvent(DataNode::EventParam_t* param)
{
    if (param->size != sizeof(Map_Info_t)) {
        return DataNode::RES_SIZE_MISMATCH;
    }

    auto info = (Map_Info_t*)param->data_p;

    switch (param->event) {
    case DataNode::EVENT_PULL: {
        if (!_isSearched) {
            searchDirLevel(_path, &_info.levelMin, &_info.levelMax);

            LV_LOG_USER(
                "Map path: %s, ext: %s, coord: %s, level min = %d, max = %d",
                _info.path,
                _info.ext,
                _info.coordTrans ? "GCJ02" : "WGS84",
                _info.levelMin,
                _info.levelMax);

            _isSearched = true;
        }

        *info = _info;
    } break;

    case DataNode::EVENT_NOTIFY: {
        _info.coordTrans = info->coordTrans;

        if (info->path && info->path != _path) {
            strncpy(_path, info->path, sizeof(_path));
            _path[sizeof(_path) - 1] = '\0';
        }

        if (info->ext && info->ext != _ext) {
            strncpy(_ext, info->ext, sizeof(_ext));
            _ext[sizeof(_ext) - 1] = '\0';
        }
    } break;

    default:
        break;
    }

    return DataNode::RES_OK;
}

bool DP_MapInfo::searchDirLevel(const char* dirName, uint8_t* min, uint8_t* max)
{
    lv_fs_dir_t dir;

    if (lv_fs_dir_open(&dir, dirName) != LV_FS_RES_OK) {
        LV_LOG_ERROR("%s open faild", dirName);
        return false;
    }

    LV_LOG_USER("%s open success", dirName);

    uint8_t levelMin = MAP_LEVEL_MAX;
    uint8_t levelMax = MAP_LEVEL_MIN;
    int levelCnt = MAP_LEVEL_MAX + 1;
    bool retval = false;

    char name[128];
    while (levelCnt--) {
        lv_fs_res_t res = lv_fs_dir_read(&dir, name, sizeof(name));

        if (name[0] == '\0' || res != LV_FS_RES_OK) {
            break;
        }
        LV_LOG_INFO("dir: %s", name);

        if (name[0] == '/') {

            /* skip "/" */
            if (name[1] == '\0') {
                continue;
            }

            retval = true;
            int level = atoi(name + 1);

            if (level < MAP_LEVEL_MIN || level > MAP_LEVEL_MAX) {
                LV_LOG_ERROR("Error level = %d", level);
                retval = false;
                break;
            }

            if (level < levelMin) {
                levelMin = level;
            }

            if (level > levelMax) {
                levelMax = level;
            }
        }
    }

    if (retval) {
        *min = levelMin;
        *max = levelMax;
    } else {
        LV_LOG_WARN("search failed!");
    }

    lv_fs_dir_close(&dir);

    return retval;
}

DATA_PROC_DESCRIPTOR_DEF(MapInfo)
