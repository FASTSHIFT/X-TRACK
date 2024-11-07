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
 *
 * Created by W-Mai on 2022/11/23.
 */

#ifndef SIMULATOR_RESOURCEPOOLDEFS_H
#define SIMULATOR_RESOURCEPOOLDEFS_H

#include "Config/Config.h"

#define KType(NAME) KType_##NAME
#define VType(NAME) VType_##NAME
#define OName(NAME) __RMSO_##NAME##_
#define ONameArray(NAME) __RMSO_##NAME##_ARRAY
#define RMSType(NAME) RMSType_##NAME

#define DEF_RES_MNGR_OBJ_EXT(NAME, VTYPE, ...)                             \
    static std::pair<KType(NAME), VTYPE> ONameArray(NAME)[] {              \
        __VA_ARGS__                                                        \
    };                                                                     \
    using RMSType(NAME) = ResourceManagerStatic<KType(NAME), VTYPE,        \
        sizeof(ONameArray(NAME)) / sizeof(std::pair<KType(NAME), VTYPE>)>; \
    static RMSType(NAME) OName(NAME)(ONameArray(NAME))

#define DEF_RES_MNGR_OBJ(NAME, ...) DEF_RES_MNGR_OBJ_EXT(NAME, VType(NAME), __VA_ARGS__)

#define DEF_RES_MNGR_GET(NAME) VType(NAME) get##NAME(KType(NAME) key)

#define IMPORT_FONT_NATIVE(NAME, SIZE)   \
    {                                    \
        "<" #SIZE ">" #NAME,             \
        {                                \
            font_value_type::NATIVE,     \
                &lv_font_##NAME##_##SIZE \
        }                                \
    }

#define IMPORT_FONT_FILE(NAME, FONT_NAME) \
    {                                     \
        NAME,                             \
        {                                 \
            font_value_type::FREETYPE,    \
                (lv_font_t*)FONT_NAME     \
        }                                 \
    }

#define IMPORT_IMAGE_NATIVE(NAME) \
    {                             \
#NAME, &img_src_##NAME    \
    }

#define IMPORT_IMAGE_FILE(NAME)                                      \
    {                                                                \
        NAME, (CONFIG_IMAGE_DIR_PATH "/" NAME CONFIG_IMAGE_EXT_NAME) \
    }

#define SET_DEFAULT(NAME, VALUE) OName(NAME).setDefaultValue((VALUE))

enum font_value_type {
    UNKNOWN,
    NATIVE,
    FREETYPE
};

struct font_value_t {
    font_value_type type;
    const lv_font_t* value;
};

#endif // SIMULATOR_RESOURCEPOOLDEFS_H
