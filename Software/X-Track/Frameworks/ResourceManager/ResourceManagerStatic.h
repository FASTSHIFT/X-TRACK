/*
 * Created by W-Mai on 2022/10/15.
 *
 * MIT License
 * Copyright (c) 2022 XCLZ STUDIO
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

#ifndef __RESOURCE_MANAGER_STATIC_H
#define __RESOURCE_MANAGER_STATIC_H

#include <utility>
#include <vector>
#include <cstddef>

template <typename KeyType, typename ValType, size_t NUM>
class ResourceManagerStatic {
public:
    using KeyValueType = std::pair<KeyType, ValType>;

private:
    KeyValueType* __pool_;

    ValType __defaultValue_;

public:
    explicit ResourceManagerStatic(KeyValueType* init)
    {
        __pool_ = init;
    }
    /**
     * @brief  Set default resources
     * @param  defaultValue: Value to the default resource
     */
    void setDefaultValue(ValType&& defaultValue)
    {
        __defaultValue_ = {
            std::forward<ValType>(defaultValue)
        };
    }

    /**
     * @brief  Get default resources
     * @retval Value to the default resource
     */
    const ValType& getDefaultValue()
    {
        return __defaultValue_;
    }

    /**
     * @brief  Get resource associated with key
     * @param  key: Resource key
     * @return Value resource associated with key
     */
    const ValType& get(KeyType key);
};

#endif // __RESOURCE_MANAGER_STATIC_H
