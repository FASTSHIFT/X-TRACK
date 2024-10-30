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
#ifndef __APP_FACTORY_H
#define __APP_FACTORY_H

#include "Frameworks/PageManager/PageManager.h"

#ifndef APP_FACTORY_MAX_NUM
#define APP_FACTORY_MAX_NUM 32
#endif

#define APP_DESCRIPTOR_DEF(name)                        \
    class name##Descriptor : public AppDescriptor {     \
    public:                                             \
        name##Descriptor()                              \
            : AppDescriptor(#name)                      \
        {                                               \
        }                                               \
        virtual PageBase* create() { return new name; } \
    };                                                  \
    static name##Descriptor name##DescriptorInstance;

class AppDescriptor;

class AppFactory : public PageFactory {
public:
    AppFactory();
    virtual ~AppFactory();
    virtual PageBase* create(const char* name);
    void add(const char* name, AppDescriptor* descriptor);

    static AppFactory* getInstance()
    {
        static AppFactory instance;
        return &instance;
    }

    void intsallAll(PageManager* manager);

private:
    const char* _nameArray[APP_FACTORY_MAX_NUM];
    AppDescriptor* _descriptorArray[APP_FACTORY_MAX_NUM];
    int _num;
};

class AppDescriptor {
public:
    AppDescriptor(const char* name)
    {
        AppFactory::getInstance()->add(name, this);
    }
    virtual PageBase* create() = 0;
};

#endif /* __APP_FACTORY_H */
