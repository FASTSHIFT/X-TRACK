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
#include "StorageService.h"
#include "ArduinoJson.h"
#include "lvgl/lvgl.h"
#include <algorithm>

#define USE_STATIC_JSON_DOC        1
#if USE_STATIC_JSON_DOC
#  define STATIC_JSON_DOC_BUF_SIZE 2048
#endif

#define VALUE_TO_DOC(type)\
do{\
    type value = 0;\
    size_t size = iter->size > sizeof(value) ? sizeof(value) : iter->size;\
    memcpy(&value, iter->value, size);\
    doc[iter->key] = value;\
}while(0)

#define DOC_TO_VALUE(type)\
do{\
    type value = doc[iter->key];\
    size_t size = iter->size < sizeof(value) ? iter->size : sizeof(value);\
    memcpy(iter->value, &value, size);\
}while(0)

class FileWrapper
{
public:
    FileWrapper(const char* path, lv_fs_mode_t mode)
    {
        memset(&file, 0, sizeof(file));
        fs_res = lv_fs_open(&file, path, mode);
    }

    ~FileWrapper()
    {
        lv_fs_close(&file);
    }

    uint8_t read()
    {
        uint8_t data = 0;
        readBytes(&data, 1);
        return data;
    }

    size_t readBytes(void* buffer, size_t length)
    {
        uint32_t br = 0;
        lv_fs_read(&file, buffer, (uint32_t)length, &br);
        return br;
    }

    size_t write(uint8_t c)
    {
        return write(&c, 1);
    }

    size_t write(const uint8_t* s, size_t n)
    {
        uint32_t bw = 0;
        lv_fs_write(&file, s, (uint32_t)n, &bw);
        return bw;
    }

    operator bool()
    {
        return fs_res == LV_FS_RES_OK;
    };

private:
    lv_fs_res_t fs_res;
    lv_fs_file_t file;
};

StorageService::StorageService(const char* filePath, uint32_t bufferSize)
{
    FilePath = filePath;
    BufferSize = bufferSize;
}

StorageService::~StorageService()
{

}

bool StorageService::Add(const char* key, void* value, uint16_t size, DataType_t type)
{
    Node_t* findNode = SearchNode(key);
    if (findNode != nullptr)
    {
        return false;
    }

    Node_t* node = new Node_t;
    node->key = key;
    node->value = value;
    node->size = size;
    node->type = type;

    NodePool.push_back(node);

    return true;
}

bool StorageService::Remove(const char* key)
{
    Node_t* node = SearchNode(key);

    if (node == nullptr)
    {
        return false;
    }

    auto iter = std::find(NodePool.begin(), NodePool.end(), node);

    if (iter == NodePool.end())
    {
        return false;
    }

    NodePool.erase(iter);
    delete node;

    return true;
}

bool StorageService::LoadFile()
{
    FileWrapper file(FilePath, LV_FS_MODE_RD);
    bool retval = true;

    if (!file)
    {
        LV_LOG_ERROR("Failed to open file: %s", FilePath);
        return false;
    }

#if USE_STATIC_JSON_DOC
    StaticJsonDocument<STATIC_JSON_DOC_BUF_SIZE> doc;
#else
    DynamicJsonDocument doc(BufferSize);
#endif

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        LV_LOG_ERROR("Failed to read file: %s", FilePath);
        return false;
    }

    // Copy values from the JsonDocument to the Config
    for (auto iter : NodePool)
    {
        if (!doc.containsKey(iter->key))
        {
            LV_LOG_WARN("NOT contains key: %s, use default value", iter->key);
            retval = false;
            continue;
        }

        switch (iter->type)
        {
        case TYPE_INT:
        {
            DOC_TO_VALUE(int);
            break;
        }
        case TYPE_FLOAT:
        {
            DOC_TO_VALUE(float);
            break;
        }
        case TYPE_DOUBLE:
        {
            DOC_TO_VALUE(double);
            break;
        }
        case TYPE_STRING:
        {
            const char* str = doc[iter->key];
            if (str)
            {
                char* value = (char*)iter->value;
                strncpy(value, str, iter->size);
                value[iter->size - 1] = '\0';
            }
            break;
        }
        default:
            LV_LOG_ERROR("Unknow type: %d", iter->type);
            break;
        }
    }

    return retval;
}

bool StorageService::SaveFile(const char* backupPath)
{
    const char* path = backupPath ? backupPath : FilePath;

    // Open file for writing
    FileWrapper file(path, LV_FS_MODE_WR | LV_FS_MODE_RD);
    if (!file)
    {
        LV_LOG_ERROR("Failed to open file");
        return false;
    }

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use https://arduinojson.org/assistant to compute the capacity.

#if USE_STATIC_JSON_DOC
    StaticJsonDocument<STATIC_JSON_DOC_BUF_SIZE> doc;
#else
    DynamicJsonDocument doc(BufferSize);
#endif

    // Set the values in the document
    for (auto iter : NodePool)
    {
        switch (iter->type)
        {
        case TYPE_INT:
        {
            VALUE_TO_DOC(int);
            break;
        }
        case TYPE_FLOAT:
        {
            VALUE_TO_DOC(float);
            break;
        }
        case TYPE_DOUBLE:
        {
            VALUE_TO_DOC(double);
            break;
        }
        case TYPE_STRING:
        {
            doc[iter->key] = (const char*)iter->value;
            break;
        }
        default:
            LV_LOG_ERROR("Unknow type: %d", iter->type);
            break;
        }
    }

    // Serialize JSON to file
    if (!serializeJsonPretty(doc, file))
    {
        LV_LOG_ERROR("Failed to write to file");
        return false;
    }

    return true;
}

StorageService::Node_t* StorageService::SearchNode(const char* key)
{
    for (auto iter : NodePool)
    {
        if (strcmp(key, iter->key) == 0)
        {
            return iter;
        }
    }
    return nullptr;
}
