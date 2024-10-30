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
#ifndef __STORAGE_SERVICE_H
#define __STORAGE_SERVICE_H

#include <cstddef>
#include <cstdint>
#include <vector>

struct cJSON;

class StorageService {
public:
    enum class TYPE {
        UNKNOW,
        BOOL,
        INT,
        FLOAT,
        DOUBLE,
        STRING,
        ARRAY,
        STRUCT_START,
        STRUCT_END,
    };

public:
    StorageService(const char* path);
    ~StorageService();

    bool add(const char* key, void* value, size_t size, TYPE type);
    bool save(const char* path = nullptr);
    bool load(const char* path = nullptr);

private:
    typedef struct Node
    {
        Node()
            : key(nullptr)
            , value(nullptr)
            , size(0)
            , type(TYPE::UNKNOW)
        {
        }
        const char* key;
        void* value;
        size_t size;
        TYPE type;
    } Node_t;

private:
    const char* _pathDefault;
    std::vector<Node_t> _nodePool;

private:
    char* loadJson(const char* path);
    bool saveJson(const char* str, const char* path);
    bool loadArray(struct cJSON* json, void* value, size_t size, int len, TYPE type);
    bool saveArray(struct cJSON* json, const char* key, void* value, size_t size, int len, TYPE type);
};

#endif
