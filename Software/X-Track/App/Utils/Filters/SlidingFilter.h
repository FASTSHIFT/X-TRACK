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
#ifndef __SLIDING_FILTER_H
#define __SLIDING_FILTER_H

#include "FilterBase.h"

namespace Filter
{

template <typename T> class Sliding : public Base<T>
{
public:
    Sliding(T sldVal)
    {
        this->Reset();
        this->slideValue = sldVal;
    }

    virtual T GetNext(T value)
    {
        if (this->CheckFirst())
        {
            this->lastValue = value;
        }
        else
        {
            if (FILTER_ABS(value - this->lastValue) < this->slideValue)
            {
                this->lastValue = value;
            }
            else
            {
                if (this->lastValue < value)
                {
                    this->lastValue += this->slideValue;
                }
                else if (this->lastValue > value)
                {
                    this->lastValue -= this->slideValue;
                }
            }
        }
        return this->lastValue;
    }

private:
    T slideValue;
};

}

#endif
