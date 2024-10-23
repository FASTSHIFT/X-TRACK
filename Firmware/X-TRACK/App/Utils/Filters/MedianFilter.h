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
#ifndef __MEDIAN_FILTER_H
#define __MEDIAN_FILTER_H

#include "FilterBase.h"
#include <algorithm>

namespace Filter
{

template <typename T, size_t bufferSize> class Median : public Base<T>
{
public:
    Median()
    {
        this->Reset();
        this->dataIndex = 0;
    }

    bool FillBuffer(T value)
    {
        if (this->dataIndex < bufferSize)
        {
            this->buffer[this->dataIndex] = value;
            this->dataIndex++;
            return false;
        }
        return true;
    }

    virtual T GetNext(T value)
    {
        if (this->isFirst)
        {
            this->isFirst = !FillBuffer(value);
            this->lastValue = value;
        }
        else
        {
            if (FillBuffer(value))
            {
                std::sort(this->buffer, this->buffer + bufferSize);
                this->lastValue = this->buffer[bufferSize / 2];
                this->dataIndex = 0;
            }
        }

        return this->lastValue;
    }

protected:
    T buffer[bufferSize];
    size_t dataIndex;
};

}

#endif
