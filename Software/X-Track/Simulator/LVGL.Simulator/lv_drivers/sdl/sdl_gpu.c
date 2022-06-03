/**
 * @file sdl_gpu.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "sdl_gpu.h"
#if USE_SDL_GPU

#if LV_USE_GPU_SDL == 0
# error "LV_USE_DRAW_SDL must be enabled"
#endif

#if USE_KEYBOARD
# warning "KEYBOARD is deprecated, use SDL instead. See lv_drivers/sdl/sdl.c"
#endif

#if USE_MOUSE
# warning "MOUSE is deprecated, use SDL instead. See lv_drivers/sdl/sdl.c"
#endif

#if USE_MOUSEWHEEL
# warning "MOUSEWHEEL is deprecated, use SDL instead that. See lv_drivers/sdl/sdl.c"
#endif

#if USE_MONITOR
# error "Cannot enable both MONITOR and SDL at the same time. "
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <lvgl/src/draw/sdl/lv_draw_sdl.h>
#include SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/
#ifndef KEYBOARD_BUFFER_SIZE
#define KEYBOARD_BUFFER_SIZE SDL_TEXTINPUTEVENT_TEXT_SIZE
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_draw_sdl_drv_param_t drv_param;
    SDL_Window * window;
    SDL_Texture * texture;
}monitor_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void window_create(monitor_t * m);
static void window_update(lv_disp_drv_t *disp_drv, void * buf);
static void monitor_sdl_clean_up(void);
static void sdl_event_handler(lv_timer_t * t);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

static volatile bool sdl_inited = false;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void sdl_init(void)
{
    /*Initialize the SDL*/
    SDL_Init(SDL_INIT_VIDEO);

    SDL_SetEventFilter(quit_filter, NULL);

    sdl_inited = true;

    SDL_StartTextInput();

    lv_timer_create(sdl_event_handler, 1, NULL);
}

void sdl_disp_drv_init(lv_disp_drv_t * disp_drv, lv_coord_t hor_res, lv_coord_t ver_res)
{
    monitor_t *m = lv_mem_alloc(sizeof(monitor_t));
    window_create(m);
    lv_disp_drv_init(disp_drv);
    disp_drv->direct_mode = 1;
    disp_drv->flush_cb = monitor_flush;
    disp_drv->hor_res = hor_res;
    disp_drv->ver_res = ver_res;
    lv_disp_draw_buf_t *disp_buf = lv_mem_alloc(sizeof(lv_disp_draw_buf_t));
    lv_disp_draw_buf_init(disp_buf, m->texture, NULL, hor_res * ver_res);
    disp_drv->draw_buf = disp_buf;
    disp_drv->antialiasing = 1;
    disp_drv->user_data = &m->drv_param;
}

/**
 * Flush a buffer to the marked area
 * @param disp_drv pointer to driver where this function belongs
 * @param area an area where to copy `color_p`
 * @param color_p an array of pixels to copy to the `area` part of the screen
 */
void sdl_display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_coord_t hres = disp_drv->hor_res;
    lv_coord_t vres = disp_drv->ver_res;

//    printf("x1:%d,y1:%d,x2:%d,y2:%d\n", area->x1, area->y1, area->x2, area->y2);

    /*Return if the area is out the screen*/
    if(area->x2 < 0 || area->y2 < 0 || area->x1 > hres - 1 || area->y1 > vres - 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    /* TYPICALLY YOU DO NOT NEED THIS
     * If it was the last part to refresh update the texture of the window.*/
    if(lv_disp_flush_is_last(disp_drv)) {
        window_update(disp_drv, color_p);
    }

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_disp_flush_ready(disp_drv);

}

void sdl_display_resize(lv_disp_t *disp, int width, int height)
{
    lv_disp_drv_t *driver = disp->driver;
    SDL_Renderer *renderer = ((lv_draw_sdl_drv_param_t *) driver->user_data)->renderer;
    if (driver->draw_buf->buf1) {
        SDL_DestroyTexture(driver->draw_buf->buf1);
    }
    SDL_Texture *texture = lv_draw_sdl_create_screen_texture(renderer, width, height);
    lv_disp_draw_buf_init(driver->draw_buf, texture, NULL, width * height);
    driver->hor_res = (lv_coord_t) width;
    driver->ver_res = (lv_coord_t) height;
    SDL_RendererInfo renderer_info;
    SDL_GetRendererInfo(renderer, &renderer_info);
    SDL_assert(renderer_info.flags & SDL_RENDERER_TARGETTEXTURE);
    SDL_SetRenderTarget(renderer, texture);
    lv_disp_drv_update(disp, driver);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * SDL main thread. All SDL related task have to be handled here!
 * It initializes SDL, handles drawing and the mouse.
 */

static void sdl_event_handler(lv_timer_t * t)
{
    (void)t;

    /*Refresh handling*/
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        mouse_handler(&event);
        mousewheel_handler(&event);
        keyboard_handler(&event);

        switch (event.type) {
            case SDL_WINDOWEVENT: {
                SDL_Window * window = SDL_GetWindowFromID(event.window.windowID);
                switch (event.window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                    case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                    case SDL_WINDOWEVENT_EXPOSED:
                        for (lv_disp_t *cur = lv_disp_get_next(NULL); cur; cur = lv_disp_get_next(cur)) {
                            window_update(cur->driver, cur->driver->draw_buf->buf_act);
                        }
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        for (lv_disp_t *cur = lv_disp_get_next(NULL); cur; cur = lv_disp_get_next(cur)) {
                            lv_draw_sdl_drv_param_t *param = cur->driver->user_data;
                            SDL_Renderer *renderer = SDL_GetRenderer(window);
                            if (param->renderer != renderer) continue;
                            int w, h;
                            SDL_GetWindowSize(window, &w, &h);
                            sdl_display_resize(cur, w, h);
                        }
                        break;
                    }
                    case SDL_WINDOWEVENT_CLOSE: {
                        for (lv_disp_t *cur = lv_disp_get_next(NULL); cur; ) {
                            lv_disp_t * tmp = cur;
                            cur = lv_disp_get_next(tmp);
                            monitor_t * m = tmp->driver->user_data;
                            SDL_Renderer *renderer = SDL_GetRenderer(window);
                            if (m->drv_param.renderer != renderer) continue;
                            SDL_DestroyTexture(tmp->driver->draw_buf->buf1);
                            SDL_DestroyRenderer(m->drv_param.renderer);
                            lv_disp_remove(tmp);
                        }

                        break;
                    }
                    default:
                        break;
                }
                break;
            }
        }
    }

    /*Run until quit event not arrives*/
    if(sdl_quit_qry) {
        monitor_sdl_clean_up();
        exit(0);
    }
}

static void monitor_sdl_clean_up(void)
{
    for (lv_disp_t *cur = lv_disp_get_next(NULL); cur; ) {
        lv_disp_t * tmp = cur;
        monitor_t * m = tmp->driver->user_data;
        SDL_DestroyTexture(tmp->driver->draw_buf->buf1);
        SDL_DestroyRenderer(m->drv_param.renderer);
        cur = lv_disp_get_next(cur);
        lv_disp_remove(tmp);
    }

    SDL_Quit();
}

static void window_create(monitor_t * m)
{
//    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1");
    m->window = SDL_CreateWindow("TFT Simulator",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SDL_HOR_RES * SDL_ZOOM, SDL_VER_RES * SDL_ZOOM, SDL_WINDOW_RESIZABLE);

    m->drv_param.renderer = SDL_CreateRenderer(m->window, -1, SDL_RENDERER_ACCELERATED);

    m->texture = lv_draw_sdl_create_screen_texture(m->drv_param.renderer, SDL_HOR_RES, SDL_VER_RES);
    /* For first frame */
    SDL_SetRenderTarget(m->drv_param.renderer, m->texture);
}

static void window_update(lv_disp_drv_t *disp_drv, void * buf)
{
    SDL_Renderer *renderer = ((lv_draw_sdl_drv_param_t *) disp_drv->user_data)->renderer;
    SDL_Texture *texture = buf;
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderClear(renderer);
#if LV_COLOR_SCREEN_TRANSP
    SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0xff);
    SDL_Rect r;
    r.x = 0; r.y = 0; r.w = SDL_HOR_RES; r.h = SDL_VER_RES;
    SDL_RenderDrawRect(renderer, &r);
#endif

    /*Update the renderer with the texture containing the rendered image*/
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_RenderSetClipRect(renderer, NULL);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_SetRenderTarget(renderer, texture);
}

#endif /*USE_SDL_GPU*/
