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
#include "StorageService.h"
#include "lvgl/lvgl.h"
#include <algorithm>
#include <cJSON.h>
#include <cstring>
#include <stack>

#define JSON_GET_ITEM(DEST, JSON_TYPE)                                  \
    do {                                                                \
        cJSON* it = cJSON_GetObjectItem(curObj, iter.key);              \
        if (!it) {                                                      \
            LV_LOG_WARN("Can't get " #JSON_TYPE " item: %s", iter.key); \
            retval = false;                                             \
        }                                                               \
        DEST = it->value##JSON_TYPE;                                    \
    } while (0)

#define JSON_ADD_ITEM(TYPE, OBJ_TYPE)                                       \
    do {                                                                    \
        TYPE value = *(TYPE*)iter.value;                                    \
        cJSON* it = cJSON_Add##OBJ_TYPE##ToObject(curObj, iter.key, value); \
        LV_ASSERT_MSG(it != nullptr, "Add" #OBJ_TYPE "failed");             \
    } while (0)

#define JSON_GET_ARRAY(DEST_TYPE, JSON_TYPE)                \
    do {                                                    \
        for (int i = 0; i < arrSize; i++) {                 \
            cJSON* it = cJSON_GetArrayItem(json, i);        \
            if (!it) {                                      \
                LV_LOG_WARN("Can't get array item: %d", i); \
                return false;                               \
            }                                               \
            *(TYPE*)value = (TYPE)it->value##JSON_TYPE;     \
            value = (TYPE*)value + 1;                       \
        }                                                   \
    } while (0)

StorageService::StorageService(const char* path)
{
    LV_ASSERT_NULL(path);
    _pathDefault = path;

    cJSON_Hooks hooks;
    memset(&hooks, 0, sizeof(hooks));
    hooks.malloc_fn = lv_malloc,
    hooks.free_fn = lv_free,
    cJSON_InitHooks(&hooks);
}

StorageService::~StorageService()
{
}

bool StorageService::add(const char* key, void* value, size_t size, TYPE type)
{
    LV_LOG_INFO("key: %s, value: %p, size: %zu, type: %d",
        key, value, size, (int)type);

    switch (type) {
    case TYPE::BOOL:
        LV_ASSERT(size == sizeof(bool));
        break;
    case TYPE::INT:
        LV_ASSERT(size == sizeof(int));
        break;
    case TYPE::FLOAT:
        LV_ASSERT(size == sizeof(float));
        break;
    case TYPE::DOUBLE:
        LV_ASSERT(size == sizeof(double));
        break;
    default:
        break;
    }

    Node_t node;
    node.key = key;
    node.value = value;
    node.size = size;
    node.type = type;
    _nodePool.push_back(node);

    return true;
}

bool StorageService::loadArray(struct cJSON* json, void* value, size_t size, int len, TYPE type)
{
    int arrSize = cJSON_GetArraySize(json);

    if (arrSize != len) {
        LV_LOG_WARN("array size(%d) not match: %d", arrSize, len);
    }

    len = std::min(arrSize, (int)len);

    if (len <= 0) {
        return false;
    }

    switch (type) {
    case TYPE::BOOL:
        JSON_GET_ARRAY(bool, int);
        break;

    case TYPE::INT:
        JSON_GET_ARRAY(int, int);
        break;

    case TYPE::FLOAT:
        JSON_GET_ARRAY(float, double);
        break;

    case TYPE::DOUBLE:
        JSON_GET_ARRAY(double, double);
        break;

    case TYPE::STRING: {
        for (int i = 0; i < arrSize; i++) {
            cJSON* it = cJSON_GetArrayItem(json, i);
            if (!it) {
                LV_LOG_WARN("Can't get array item: %d", i);
                return false;
            }
            lv_snprintf((char*)value, size, "%s", cJSON_GetStringValue(it));
            value = (char*)value + size;
        }
    } break;

    default:
        LV_LOG_ERROR("Unknow type: %d", (int)type);
        return false;
    }

    return true;
}

bool StorageService::saveArray(struct cJSON* json, const char* key, void* value, size_t size, int len, TYPE type)
{
    LV_ASSERT_NULL(json);
    LV_ASSERT_NULL(key);
    LV_ASSERT_NULL(value);

    bool retval = false;
    cJSON* array = nullptr;

    switch (type) {
    case TYPE::BOOL:
        LV_LOG_WARN("bool array not support");
        break;

    case TYPE::INT: {
        array = cJSON_CreateIntArray((const int*)value, len);
        retval = cJSON_AddItemToObject(json, key, array);
    } break;

    case TYPE::FLOAT: {
        array = cJSON_CreateFloatArray((const float*)value, len);
        retval = cJSON_AddItemToObject(json, key, array);
    } break;

    case TYPE::DOUBLE: {
        array = cJSON_CreateDoubleArray((const double*)value, len);
        retval = cJSON_AddItemToObject(json, key, array);
    } break;

    case TYPE::STRING: {
        array = cJSON_CreateStringArray((const char* const*)value, len);
        retval = cJSON_AddItemToObject(json, key, array);
    } break;

    default:
        LV_LOG_ERROR("Unknow type: %d", (int)type);
        break;
    }

    return retval && array != nullptr;
}

bool StorageService::load(const char* path)
{
    bool retval = true;
    char* buf = nullptr;
    cJSON* cjson = nullptr;
    cJSON* curObj = nullptr;
    cJSON* arrayObj = nullptr;
    int arrayLen = 0;
    std::stack<cJSON*> objStack;

    path = path ? path : _pathDefault;

    buf = loadJson(path);
    if (!buf) {
        return false;
    }

    cjson = cJSON_Parse(buf);
    if (!cjson) {
        LV_LOG_ERROR("cJSON_Parse failed");
        retval = false;
        goto failed;
    }

    curObj = cjson;

    for (auto& iter : _nodePool) {
        /* print node info */
        LV_LOG_INFO("key: %s, value: %p, size: %zu, type: %d",
            iter.key, iter.value, iter.size, (int)iter.type);

        if (arrayObj) {
            retval = loadArray(
                arrayObj,
                iter.value,
                iter.size,
                arrayLen,
                iter.type);
            arrayObj = nullptr;
            arrayLen = 0;
            continue;
        }

        cJSON* item = nullptr;
        if (iter.key) {
            item = cJSON_GetObjectItem(curObj, iter.key);
            if (!item) {
                LV_LOG_WARN("could NOT get key: %s", iter.key);
                retval = false;
                continue;
            }
        }

        /* skip empty data */
        if (!iter.key && iter.type != TYPE::STRUCT_END) {
            continue;
        }

        switch (iter.type) {
        case TYPE::STRUCT_START:
            LV_LOG_INFO("struct start: %s {", iter.key);
            objStack.push(curObj);
            curObj = cJSON_GetObjectItem(curObj, iter.key);
            break;

        case TYPE::STRUCT_END:
            if (objStack.empty()) {
                LV_LOG_ERROR("struct end not match");
                retval = false;
                continue;
            }
            curObj = objStack.top();
            objStack.pop();
            LV_LOG_INFO("} struct end");
            break;

        case TYPE::ARRAY:
            arrayObj = item;
            arrayLen = iter.size;
            LV_LOG_INFO("array: %s[%d]", iter.key, arrayLen);
            break;

        case TYPE::BOOL:
            JSON_GET_ITEM(*(bool*)iter.value, int);
            break;

        case TYPE::INT:
            JSON_GET_ITEM(*(int*)iter.value, int);
            break;

        case TYPE::FLOAT:
            JSON_GET_ITEM(*(float*)iter.value, double);
            break;

        case TYPE::DOUBLE:
            JSON_GET_ITEM(*(double*)iter.value, double);
            break;

        case TYPE::STRING: {
            cJSON* it = cJSON_GetObjectItem(curObj, iter.key);
            if (!it) {
                LV_LOG_WARN("can't get string item: %s", iter.key);
                retval = false;
            }
            lv_snprintf((char*)iter.value, iter.size, "%s", cJSON_GetStringValue(it));
        } break;

        default:
            LV_LOG_ERROR("Unknow type: %d", (int)iter.type);
            retval = false;
            break;
        }
    }

failed:
    if (buf) {
        lv_free(buf);
    }

    if (cjson) {
        cJSON_Delete(cjson);
    }

    return retval;
}

bool StorageService::save(const char* path)
{
    bool retval = false;
    char* json_str = nullptr;
    cJSON* cjson = nullptr;
    cJSON* curObj = nullptr;
    int arrayLen = 0;
    const char* arrayKey = nullptr;
    std::stack<cJSON*> objStack;

    cjson = cJSON_CreateObject();
    LV_ASSERT_MALLOC(cjson);
    curObj = cjson;

    path = path ? path : _pathDefault;

    if (!cjson) {
        LV_LOG_ERROR("can't create cjson object");
        return false;
    }

    for (auto& iter : _nodePool) {
        /* print node info */
        LV_LOG_INFO("key: %s, value: %p, size: %zu, type: %d",
            iter.key, iter.value, iter.size, (int)iter.type);

        if (arrayLen > 0) {
            LV_ASSERT(saveArray(
                curObj,
                arrayKey,
                iter.value,
                iter.size,
                arrayLen,
                iter.type));
            arrayLen = 0;
            continue;
        }

        switch (iter.type) {
        case TYPE::STRUCT_START:
            LV_LOG_INFO("struct start: %s {", iter.key);
            objStack.push(curObj);
            curObj = cJSON_AddObjectToObject(curObj, iter.key);
            break;

        case TYPE::STRUCT_END:
            curObj = objStack.top();
            objStack.pop();
            LV_LOG_INFO("} struct end");
            break;

        case TYPE::ARRAY:
            arrayKey = iter.key;
            arrayLen = iter.size;
            LV_LOG_INFO("array: %s[%d]", iter.key, arrayLen);
            break;

        case TYPE::BOOL:
            JSON_ADD_ITEM(bool, Bool);
            break;

        case TYPE::INT:
            JSON_ADD_ITEM(int, Number);
            break;

        case TYPE::FLOAT:
            JSON_ADD_ITEM(float, Number);
            break;

        case TYPE::DOUBLE:
            JSON_ADD_ITEM(double, Number);
            break;

        case TYPE::STRING: {
            cJSON* it = cJSON_AddStringToObject(curObj, iter.key, (const char*)iter.value);
            LV_ASSERT_MSG(it != nullptr, "add String failed");
        } break;

        default:
            LV_ASSERT_MSG(false, "Unknow type");
            break;
        }
    }

    json_str = cJSON_Print(cjson);
    if (!json_str) {
        LV_LOG_ERROR("json_str is NULL");
        goto failed;
    }

    retval = saveJson(json_str, path);

failed:
    if (cjson) {
        cJSON_Delete(cjson);
    }

    if (json_str) {
        cJSON_free(json_str);
    }

    return retval;
}

char* StorageService::loadJson(const char* path)
{
    lv_fs_file_t file;
    char* buf = NULL;

    /* open json file */
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_RD);
    if (res != LV_FS_RES_OK) {
        LV_LOG_ERROR("open file: %s faild: %d", path, res);
        return NULL;
    }

    /* get file size */
    lv_fs_seek(&file, 0, LV_FS_SEEK_END);
    uint32_t size;
    res = lv_fs_tell(&file, &size);
    if (res != LV_FS_RES_OK) {
        LV_LOG_ERROR("can't get file size");
        goto failed;
    }
    lv_fs_seek(&file, 0, LV_FS_SEEK_SET);

    /* alloc string buffer */
    buf = (char*)lv_malloc(size + 1);
    LV_ASSERT_MALLOC(buf);
    if (!buf) {
        LV_LOG_ERROR("malloc failed for json buf");
        goto failed;
    }

    /* read json sting */
    uint32_t br;
    res = lv_fs_read(&file, buf, size, &br);
    lv_fs_close(&file);
    if (res != LV_FS_RES_OK || br != size) {
        LV_LOG_ERROR("read file failed");
        goto failed;
    }
    buf[size] = '\0';

    return buf;

failed:
    lv_fs_close(&file);

    if (buf) {
        lv_free(buf);
    }

    return NULL;
}

bool StorageService::saveJson(const char* str, const char* path)
{
    LV_ASSERT_NULL(str);

    uint32_t len = strlen(str);

    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_WR);
    if (res != LV_FS_RES_OK) {
        LV_LOG_ERROR("open file: %s faild: %d", path, res);
        return false;
    }

    uint32_t bw;
    res = lv_fs_write(&file, str, len, &bw);
    lv_fs_close(&file);

    if (!(res == LV_FS_RES_OK && bw == len)) {
        LV_LOG_ERROR("write file failed: %d", res);
        return false;
    }

    return true;
}
