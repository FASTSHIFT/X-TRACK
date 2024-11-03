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

#include "ResourcePool.h"
#include "Config/Config.h"
#include "Frameworks/ResourceManager/ResourceManagerStatic.cpp"
#include "ResourcePoolDefs.h"
#include "Utils/lv_msg/lv_msg.h"
#include <cstdlib>

/* clang-format off */

#define FONT_REGULAR_NAME                   "Alibaba_PuHuiTi_2.0_55_Regular"
#define FONT_MEDIUM_NAME                    "Alibaba_PuHuiTi_2.0_65_Medium"
#define FONT_BOLD_NAME                      "Alibaba_PuHuiTi_2.0_115_Black"

/* clang-format on */

namespace ResourcePool {

static lv_font_manager_t* g_font_manager = NULL;

DEF_RES_MNGR_OBJ_EXT(Font, font_value_t,
    IMPORT_FONT_NATIVE(montserrat, 14),
    IMPORT_FONT_FILE("regular", FONT_REGULAR_NAME),
    IMPORT_FONT_FILE("medium", FONT_MEDIUM_NAME),
    IMPORT_FONT_FILE("bold", FONT_BOLD_NAME));

DEF_RES_MNGR_OBJ(Image,
    IMPORT_IMAGE_FILE("triangle_indicator"),
    IMPORT_IMAGE_FILE("navi_arrow_default"),
    IMPORT_IMAGE_FILE("navi_arrow_dark"),
    IMPORT_IMAGE_FILE("navi_arrow_light"),
    IMPORT_IMAGE_FILE("trip_dist"),
    IMPORT_IMAGE_FILE("trip_time"),
    IMPORT_IMAGE_FILE("location_icon"));

void init()
{
    lv_msg_init();
    g_font_manager = lv_font_manager_create(8);

    lv_font_manager_add_path_static(g_font_manager, FONT_REGULAR_NAME, FONT_PATH_BASE FONT_REGULAR_NAME FONT_EXT);
    lv_font_manager_add_path_static(g_font_manager, FONT_MEDIUM_NAME, FONT_PATH_BASE FONT_MEDIUM_NAME FONT_EXT);
    lv_font_manager_add_path_static(g_font_manager, FONT_BOLD_NAME, FONT_PATH_BASE FONT_BOLD_NAME FONT_EXT);

    font_value_t default_font = {
        font_value_type::UNKNOWN,
        LV_FONT_DEFAULT
    };
    SET_DEFAULT(Font, std::forward<font_value_t>(default_font));
    SET_DEFAULT(Image, LV_SYMBOL_WARNING);
}

void deinit()
{
    lv_font_manager_delete(g_font_manager);
    g_font_manager = NULL;
}

/**
 * @brief Parse real font name and font size from input name.
 * @param name Font name
 * @param size Return font size
 * @return Pointer of real NAME start
 */
static const char* get_font_size_from_name(const char* name, int* size)
{
    if (name[0] != '<' || size == nullptr) {
        return name;
    }
    LV_ASSERT_NULL(size);

    /* +1 to skip '<' */
    char* end_ptr;
    *size = (int)strtol(name + 1, &end_ptr, 10);
    if (end_ptr == name) {
        return name;
    }
    return end_ptr + 1; /* +1 to skip '>' */
}

DEF_RES_MNGR_GET(Font)
{
    /* font key: "name_size" */

    auto font = OName(Font).get(key);

    /* native font */
    if (font.type == font_value_type::NATIVE) {
        return font.value;
    }

    int size = -1;
    auto font_key = get_font_size_from_name(key, &size);
    auto freetype_font = OName(Font).get(font_key);

    if (freetype_font.type == font_value_type::UNKNOWN) {
        LV_LOG_ERROR("error key = %s", key);
        return font.value;
    }

    if (size <= 0) {
        LV_LOG_ERROR("error size = %d", size);
        return font.value;
    }

    const lv_font_t* new_font = lv_font_manager_create_font(
        g_font_manager,
        (const char*)freetype_font.value,
        LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
        size,
        LV_FREETYPE_FONT_STYLE_NORMAL);

    if (!new_font) {
        new_font = LV_FONT_DEFAULT;
        LV_LOG_WARN("new_font is NULL, use LV_FONT_DEFAULT");
    }

    return new_font;
}

void dropFont(const lv_font_t* font)
{
    if (font == LV_FONT_DEFAULT) {
        return;
    }

    lv_font_manager_delete_font(g_font_manager, (lv_font_t*)font);
}

DEF_RES_MNGR_GET(Image)
{
    return OName(Image).get(key);
}
}
