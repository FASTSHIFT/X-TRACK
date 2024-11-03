/*
 * MIT License
 * Copyright (c) 2022 XCLZ STUDIO@W-Mai
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

#ifndef RESOURCE_POOL_H
#define RESOURCE_POOL_H

#include "lv_symbol_ext_def.h"
#include "lvgl/lvgl.h"

/* Macro Definitions */
#define RES_MNG(NAME, K_TYPE, V_TYPE) \
    using KType_##NAME = K_TYPE;      \
    using VType_##NAME = V_TYPE;      \
    VType_##NAME get##NAME(KType_##NAME key)

namespace ResourcePool {

void init();
void deinit();

RES_MNG(Font, const char*, const lv_font_t*);
RES_MNG(Image, const char*, const void*);

void dropFont(const lv_font_t* font);

/* Font Wrapper */
class Font {
public:
    Font(const char* key)
    {
        _font = getFont(key);
    }
    Font(int size, const char* name)
    {
        char key[64];
        lv_snprintf(key, sizeof(key), "<%d>%s", size, name);
        _font = getFont(key);
    }
    ~Font()
    {
        dropFont(_font);
    }

    operator const lv_font_t*() const
    {
        return _font;
    }

private:
    const lv_font_t* _font;
};

}

/* Macro UnDefinitions */
#undef RES_MNG

#endif // RESOURCE_POOL_H
