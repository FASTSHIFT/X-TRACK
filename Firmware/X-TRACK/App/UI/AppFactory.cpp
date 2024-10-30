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
#include "AppFactory.h"
#include <cstring>

AppFactory::AppFactory()
    : _nameArray { 0 }
    , _descriptorArray { 0 }
    , _num(0)
{
}

AppFactory::~AppFactory() { }

void AppFactory::add(const char* name, AppDescriptor* descriptor)
{
    if (_num >= APP_FACTORY_MAX_NUM) {
        return;
    }

    _nameArray[_num] = name;
    _descriptorArray[_num] = descriptor;
    _num++;
}

PageBase* AppFactory::create(const char* name)
{
    for (int i = 0; i < _num; i++) {
        if (strcmp(name, _nameArray[i]) == 0) {
            return _descriptorArray[i]->create();
        }
    }

    return nullptr;
}

void AppFactory::intsallAll(PageManager* manager)
{
    for (int i = 0; i < _num; i++) {
        manager->install(_nameArray[i], nullptr);
    }
}
