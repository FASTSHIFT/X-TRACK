/*
 * MIT License
 * Copyright (c) 2022 _VIFEXTech
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
#include "lv_img_png.h"
#include "PNGdec/src/PNGdec.h"

/*********************
 *      DEFINES
 *********************/

#define MY_CLASS &lv_img_png_class

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    lv_color_t* line_buf;
    lv_color_t* dest_buf;
    const lv_area_t* dest_area;
    const lv_area_t* src_area;
    const lv_area_t* disp_area;
    PNG* png_dec;
} lv_img_png_draw_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_img_png_constructor(const lv_obj_class_t* class_p, lv_obj_t* obj);
static void lv_img_png_destructor(const lv_obj_class_t* class_p, lv_obj_t* obj);
static void lv_img_png_event(const lv_obj_class_t* class_p, lv_event_t* e);
static lv_res_t lv_png_draw(const char* src, lv_img_png_draw_dsc_t* dsc);
static void png_draw_line(PNGDRAW* p_draw);
static void* fs_open(const char* filename, int32_t* size);
static void fs_close(void* handle);
static int32_t fs_read(PNGFILE* handle, uint8_t* buffer, int32_t length);
static int32_t fs_seek(PNGFILE* handle, int32_t position);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_img_png_class =
{
    .base_class = &lv_obj_class,
    .constructor_cb = lv_img_png_constructor,
    .destructor_cb = lv_img_png_destructor,
    .event_cb = lv_img_png_event,
    .instance_size = sizeof(lv_img_png_t),
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t* lv_img_png_create(lv_obj_t* parent)
{
    lv_obj_t* obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    return obj;
}

void lv_img_png_set_src(lv_obj_t* obj, const char* src)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_img_png_t* img = (lv_img_png_t*)obj;

    if (src)
    {
        size_t len = strlen(src) + 1;
        img->src = (char*)lv_mem_realloc(img->src, len);
        strcpy(img->src, src);
    }
    else
    {
        if (img->src)
        {
            lv_mem_free(img->src);
            img->src = NULL;
        }
    }

    lv_obj_invalidate(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_img_png_constructor(const lv_obj_class_t* class_p, lv_obj_t* obj)
{
    LV_TRACE_OBJ_CREATE("begin");

    lv_img_png_t* img = (lv_img_png_t*)obj;

    img->src = NULL;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_img_png_destructor(const lv_obj_class_t* class_p, lv_obj_t* obj)
{
    LV_TRACE_OBJ_CREATE("begin");

    lv_img_png_t* img = (lv_img_png_t*)obj;

    if (img->src)
    {
        lv_mem_free(img->src);
        img->src = NULL;
    }

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_img_png_event(const lv_obj_class_t* class_p, lv_event_t* e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /* Call the ancestor's event handler */
    lv_event_code_t code = lv_event_get_code(e);

    if(code != LV_EVENT_DRAW_MAIN_BEGIN)
    {
        res = lv_obj_event_base(MY_CLASS, e);
        if(res != LV_RES_OK) return;
    }

    if (code == LV_EVENT_DRAW_MAIN_BEGIN)
    {
        lv_obj_t* obj = lv_event_get_current_target(e);
        lv_img_png_t* img = (lv_img_png_t*)obj;

        if (img->src == NULL)
        {
            return;
        }

        const lv_draw_ctx_t* draw_ctx = (const lv_draw_ctx_t*)lv_event_get_param(e);

        lv_disp_t* disp = _lv_refr_get_disp_refreshing();
        lv_disp_draw_buf_t* draw_buf = lv_disp_get_draw_buf(disp);
        lv_color_t* disp_buf = (lv_color_t*)draw_buf->buf_act;

        lv_area_t disp_area;
        lv_area_set(&disp_area, 0, 0, LV_HOR_RES - 1, LV_VER_RES - 1);
        lv_coord_t disp_width = lv_area_get_width(&disp_area);

        lv_area_t src_area;
        src_area = *draw_ctx->clip_area;
        lv_area_move(&src_area, -obj->coords.x1, -obj->coords.y1);

        lv_img_png_draw_dsc_t dsc;
        dsc.dest_buf = disp_buf + draw_ctx->clip_area->y1 * disp_width + draw_ctx->clip_area->x1;
        dsc.dest_area = draw_ctx->clip_area;
        dsc.src_area = &src_area;
        dsc.disp_area = &disp_area;

        lv_png_draw(img->src, &dsc);
    }
}

static void png_draw_line(PNGDRAW* p_draw)
{
    lv_img_png_draw_dsc_t* dsc = (lv_img_png_draw_dsc_t*)p_draw->pUser;

    int iEndianness = PNG_RGB565_LITTLE_ENDIAN;
    lv_color16_t* line_buf;

#if LV_COLOR_DEPTH == 16
#if LV_COLOR_16_SWAP == 1
    iEndianness = PNG_RGB565_BIG_ENDIAN;
#endif
    line_buf = dsc->line_buf;
#else
    line_buf = (lv_color16_t*)lv_mem_buf_get(p_draw->iWidth * sizeof(lv_color16_t));
#endif

    dsc->png_dec->getLineAsRGB565(p_draw, (uint16_t*)line_buf, iEndianness, 0xffffffff);

    lv_coord_t draw_y = p_draw->y;

    if (draw_y >= dsc->src_area->y1 && draw_y <= dsc->src_area->y2)
    {
        lv_coord_t width = lv_area_get_width(dsc->src_area);
        lv_coord_t x_offset = dsc->src_area->x1;
        lv_coord_t y_offset = draw_y - dsc->src_area->y1;

        lv_color_t* dest = dsc->dest_buf + y_offset * lv_area_get_width(dsc->disp_area);
        const lv_color16_t* src = line_buf + x_offset;
#if LV_COLOR_DEPTH == 16
        lv_memcpy(dest, src, width * sizeof(lv_color_t));
#else
        for (int i = 0; i < width; i++)
        {
            *dest = lv_color_make(
                src->ch.red   << 3,
                src->ch.green << 2,
                src->ch.blue  << 3
            );
            dest++;
            src++;
        }
#endif
    }

#if LV_COLOR_DEPTH != 16
    lv_mem_buf_release(line_buf);
#endif
}

static lv_res_t lv_png_draw(const char* src, lv_img_png_draw_dsc_t* dsc)
{
    static PNG png_dec;

    lv_res_t res = LV_RES_INV;

    int ret = png_dec.open(src, fs_open, fs_close, fs_read, fs_seek, png_draw_line);

    if (ret == PNG_SUCCESS)
    {
        LV_LOG_INFO(
            "image specs: (%d x %d), %d bpp, pixel type: %d",
            png_dec.getWidth(),
            png_dec.getHeight(),
            png_dec.getBpp(),
            png_dec.getPixelType()
        );

        dsc->png_dec = &png_dec;
        dsc->line_buf = (lv_color_t*)lv_mem_buf_get(png_dec.getWidth() * sizeof(lv_color_t));

        if (png_dec.decode(dsc, PNG_FAST_PALETTE) == PNG_SUCCESS)
        {
            res = LV_RES_OK;
        }

        lv_mem_buf_release(dsc->line_buf);

        png_dec.close();
    }
    return res;
}

static void* fs_open(const char* filename, int32_t* size)
{
    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, filename, LV_FS_MODE_RD);

    if (res != LV_FS_RES_OK)
    {
        *size = 0;
        return NULL;
    }

    lv_fs_file_t* p_file = (lv_fs_file_t*)lv_mem_buf_get(sizeof(file));
    *p_file = file;
    uint32_t pos = 0;

    lv_fs_seek(p_file, 0, LV_FS_SEEK_END);
    lv_fs_tell(p_file, &pos);
    lv_fs_seek(p_file, 0, LV_FS_SEEK_SET);
    *size = pos;

    return p_file;
}

static void fs_close(void* handle)
{
    lv_fs_file_t* p_file = (lv_fs_file_t*)handle;
    lv_fs_close(p_file);
    lv_mem_buf_release(p_file);
}

static int32_t fs_read(PNGFILE* handle, uint8_t* buffer, int32_t length)
{
    lv_fs_file_t* p_file = (lv_fs_file_t*)handle->fHandle;
    uint32_t br;
    lv_fs_read(p_file, buffer, length, &br);
    return br;
}

static int32_t fs_seek(PNGFILE* handle, int32_t position)
{
    lv_fs_file_t* p_file = (lv_fs_file_t*)handle->fHandle;
    lv_fs_res_t res = lv_fs_seek(p_file, position, LV_FS_SEEK_SET);
    return res == LV_FS_RES_OK ? 0 : -1;
}
