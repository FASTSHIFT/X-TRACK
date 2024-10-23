/*
 * Created by W-Mai on 2022/10/17.
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

#include "ResourceManagerStatic.h"
#include <algorithm>
#include <cstring>

template <typename KeyType, typename ValType, size_t NUM>
const ValType& ResourceManagerStatic<KeyType, ValType, NUM>::get(KeyType key)
{
    using KeyValType = std::pair<KeyType, ValType>;
    auto res = std::find_if(__pool_, __pool_ + NUM, [&key](KeyValType node) {
        return strcmp(node.first, key) == 0;
    });

    if (res == __pool_ + NUM) {
        return __defaultValue_;
    } else {
        return res->second;
    }
}
