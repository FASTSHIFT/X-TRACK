/*
 * MIT License
 * Copyright (c) 2021 - 2024 _VIFEXTech
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
#ifndef __LV_EXT_FUNC_H
#define __LV_EXT_FUNC_H

#include "lvgl/lvgl.h"

#define LV_ANIM_EXEC(attr) (lv_anim_exec_xcb_t) lv_obj_set_##attr

#define LV_SYMBOL_UNICODE(unicode) lv_txt_unicode_to_utf8_str(unicode)

void lv_obj_set_opa_scale(lv_obj_t* obj, lv_opa_t opa);

lv_opa_t lv_obj_get_opa_scale(lv_obj_t* obj);

void lv_label_set_text_add(lv_obj_t* label, const char* text);

lv_indev_t* lv_indev_search(lv_indev_type_t type);

const char* lv_txt_unicode_to_utf8_str(uint32_t unicode);

#endif /* __LV_EXT_FUNC_H */