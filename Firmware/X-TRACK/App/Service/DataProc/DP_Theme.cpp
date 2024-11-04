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
#include "DataProc.h"
#include "UI/Resource/ResourcePool.h"
#include "lvgl/src/lvgl_private.h"
#include <cstdlib>

#define THEME_DARK_COLOR_PRIMARY lv_color_hex(0xEB5E00)
#define THEME_DARK_COLOR_SECONDARY lv_color_hex(0xEA5455)
#define THEME_LIGHT_COLOR_PRIMARY lv_palette_main(LV_PALETTE_BLUE)
#define THEME_LIGHT_COLOR_SECONDARY lv_palette_main(LV_PALETTE_RED)

#define THEME_NAME "theme"
#define THEME_DARK_NAME "dark"
#define THEME_LIGHT_NAME "light"
#define THEME_AUTO_NAME "auto"
#define DISP_ROTATION_NAME "disp-rotation"

#define THEME_FONT_SMALL "<12>regular"
#define THEME_FONT_NORMAL "<16>regular"
#define THEME_FONT_LARGE "<20>regular"

#define THEME_FONTAWESOME_SMALL "<12>awesome"
#define THEME_FONTAWESOME_NORMAL "<16>awesome"
#define THEME_FONTAWESOME_LARGE "<20>awesome"

#define IS_STR_EQ(STR1, STR2) (strcmp(STR1, STR2) == 0)

using namespace DataProc;

class DP_Theme {
public:
    DP_Theme(DataNode* node);

private:
    DataNode* _node;
    const DataNode* _nodeStorage;
    const DataNode* _nodeSunRise;
    Env_Helper _env;
    char _themeName[8];
    int _dispRotation;
    int8_t _isDark;
    ResourcePool::Font _fontSmall;
    ResourcePool::Font _fontNormal;
    ResourcePool::Font _fontLarge;
    ResourcePool::Font _fontAwesomeSmall;
    ResourcePool::Font _fontAwesomeNormal;
    ResourcePool::Font _fontAwesomeLarge;
    lv_font_t _fontSmallHandle;
    lv_font_t _fontNormalHandle;
    lv_font_t _fontLargeHandle;

private:
    int onEvent(DataNode::EventParam_t* param);
    void updateTheme(bool isDark);
};

DP_Theme::DP_Theme(DataNode* node)
    : _node(node)
    , _nodeStorage(nullptr)
    , _nodeSunRise(nullptr)
    , _env(node)
    , _themeName(THEME_DARK_NAME)
    , _isDark(-1)
    , _fontSmall(THEME_FONT_SMALL)
    , _fontNormal(THEME_FONT_NORMAL)
    , _fontLarge(THEME_FONT_LARGE)
    , _fontAwesomeSmall(THEME_FONTAWESOME_SMALL)
    , _fontAwesomeNormal(THEME_FONTAWESOME_NORMAL)
    , _fontAwesomeLarge(THEME_FONTAWESOME_LARGE)
{
    _nodeStorage = node->subscribe("Storage");
    _nodeSunRise = node->subscribe("SunRise");

    Storage_Helper storage(node);
    storage.add(THEME_NAME, _themeName, sizeof(_themeName), STORAGE_TYPE::STRING);
    storage.add(DISP_ROTATION_NAME, &_dispRotation, sizeof(_dispRotation), STORAGE_TYPE::INT);

    node->setEventCallback(
        [](DataNode* n, DataNode::EventParam_t* param) {
            auto ctx = (DP_Theme*)n->getUserData();
            return ctx->onEvent(param);
        },
        DataNode::EVENT_PUBLISH);

    lv_display_add_event_cb(
        lv_display_get_default(),
        [](lv_event_t* e) {
            auto self = (DP_Theme*)lv_event_get_user_data(e);
            auto disp = (lv_disp_t*)lv_event_get_current_target(e);
            self->_dispRotation = lv_disp_get_rotation(disp);
        },
        LV_EVENT_RESOLUTION_CHANGED,
        this);

    /* Initialize font handles */
    _fontSmallHandle = *_fontSmall;
    _fontSmallHandle.fallback = _fontAwesomeSmall;
    _fontNormalHandle = *_fontNormal;
    _fontNormalHandle.fallback = _fontAwesomeNormal;
    _fontLargeHandle = *_fontLarge;
    _fontLargeHandle.fallback = _fontAwesomeLarge;

    updateTheme(true);
}

int DP_Theme::onEvent(DataNode::EventParam_t* param)
{
    if (param->tran == _env) {
        auto info = (const Env_Info_t*)param->data_p;

        if (IS_STR_EQ(info->key, THEME_NAME)) {
            if (info->value != _themeName) {
                strncpy(_themeName, info->value, sizeof(_themeName) - 1);
            }

            bool isDark;
            if (IS_STR_EQ(info->value, THEME_AUTO_NAME)) {
                SunRise_Info_t sunRiseInfo;
                _node->pull(_nodeSunRise, &sunRiseInfo, sizeof(sunRiseInfo));
                isDark = sunRiseInfo.state == SUNRISE_STATE::NIGHT;
            } else {
                isDark = IS_STR_EQ(info->value, THEME_DARK_NAME);
            }
            updateTheme(isDark);
        }
    } else if (param->tran == _nodeStorage) {
        auto info = (const Storage_Info_t*)param->data_p;

        if (info->cmd != STORAGE_CMD::LOAD) {
            return DataNode::RES_UNSUPPORTED_REQUEST;
        }

        _env.set(THEME_NAME, _themeName);
        lv_disp_set_rotation(nullptr, (lv_disp_rotation_t)_dispRotation);
    } else if (param->tran == _nodeSunRise) {
        if (IS_STR_EQ(_themeName, THEME_AUTO_NAME)) {
            auto info = (const SunRise_Info_t*)param->data_p;
            updateTheme(info->state == SUNRISE_STATE::NIGHT);
        }
    }

    return DataNode::RES_OK;
}

void DP_Theme::updateTheme(bool isDark)
{
    /* Update theme only when the theme is changed */
    if (_isDark == isDark) {
        return;
    }

    _isDark = isDark;

    lv_color_t color_primary = isDark ? THEME_DARK_COLOR_PRIMARY : THEME_LIGHT_COLOR_PRIMARY;
    lv_color_t color_secondary = isDark ? THEME_DARK_COLOR_SECONDARY : THEME_LIGHT_COLOR_SECONDARY;

    lv_theme_t* def_theme = lv_theme_default_init(nullptr, color_primary, color_secondary, isDark, LV_FONT_DEFAULT);
    def_theme->font_small = &_fontSmallHandle;
    def_theme->font_normal = &_fontNormalHandle;
    def_theme->font_large = &_fontLargeHandle;
}

DATA_PROC_DESCRIPTOR_DEF(Theme)
