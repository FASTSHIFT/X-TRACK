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
#ifndef __RESOURCE_MANAGER_H
#define __RESOURCE_MANAGER_H

#include <stddef.h>

template <typename KEY_TYPE, typename VALUE_TYPE>
class ResourceManager {
public:
    typedef struct {
        KEY_TYPE key;
        VALUE_TYPE value;
    } KEY_PAIR;

    typedef bool (*COMPARE_CALLBACK)(KEY_TYPE key1, KEY_TYPE key2);

public:
    ResourceManager(KEY_PAIR* array, size_t size, COMPARE_CALLBACK compareCallback, VALUE_TYPE defaultValue)
        : _array(array)
        , _size(size)
        , _compareCallback(compareCallback)
        , _defaultValue(defaultValue)
    {
    }
    ~ResourceManager() { }

    VALUE_TYPE* get(KEY_TYPE key)
    {
        for (size_t i = 0; i < _size; i++) {
            if (_compareCallback(key, _array[i].key)) {
                return &_array[i].value;
            }
        }
        return &_defaultValue;
    }

    bool set(KEY_TYPE key, VALUE_TYPE value)
    {
        for (size_t i = 0; i < _size; i++) {
            if (_compareCallback(key, _array[i].key)) {
                _array[i].value = value;
                return true;
            }
        }
        return false;
    }

    void setDefault(VALUE_TYPE value)
    {
        _defaultValue = value;
    }

    VALUE_TYPE getDefault()
    {
        return _defaultValue;
    }

private:
    KEY_PAIR* _array;
    size_t _size;
    COMPARE_CALLBACK _compareCallback;
    VALUE_TYPE _defaultValue;
};

#endif
