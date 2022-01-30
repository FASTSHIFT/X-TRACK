/**
 * @file sdl.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "sdl.h"
#if USE_MONITOR || USE_SDL

#if LV_USE_GPU_SDL
# error "LV_USE_GPU_SDL must not be enabled"
#endif

#if USE_MONITOR
# warning "MONITOR is deprecated, use SDL instead. See lv_drivers/sdl/sdl.c"
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

#if USE_MONITOR && USE_SDL
# error "Cannot enable both MONITOR and SDL at the same time. "
#endif

#if USE_MONITOR
# define SDL_HOR_RES            MONITOR_HOR_RES
# define SDL_VER_RES            MONITOR_VER_RES
# define SDL_ZOOM               MONITOR_ZOOM
# define SDL_DOUBLE_BUFFERED    MONITOR_DOUBLE_BUFFERED
# define SDL_INCLUDE_PATH       MONITOR_SDL_INCLUDE_PATH
# define SDL_VIRTUAL_MACHINE    MONITOR_VIRTUAL_MACHINE
# define SDL_DUAL_DISPLAY       MONITOR_DUAL
#endif

#ifndef SDL_FULLSCREEN
# define SDL_FULLSCREEN        0
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * texture;
    volatile bool sdl_refr_qry;
#if SDL_DOUBLE_BUFFERED
    uint32_t * tft_fb_act;
#else
    uint32_t * tft_fb;
#endif
}monitor_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void window_create(monitor_t * m);
static void window_update(monitor_t * m);
int quit_filter(void * userdata, SDL_Event * event);
static void monitor_sdl_clean_up(void);
static void sdl_event_handler(lv_timer_t * t);
static void monitor_sdl_refr(lv_timer_t * t);
static void mouse_handler(SDL_Event * event);
static void mousewheel_handler(SDL_Event * event);
static uint32_t keycode_to_ctrl_key(SDL_Keycode sdl_key);
static void keyboard_handler(SDL_Event * event);

#if LV_TICK_CUSTOM == 0
static int tick_thread(void *data);
#endif

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/
monitor_t monitor;

#if SDL_DUAL_DISPLAY
monitor_t monitor2;
#endif

static volatile bool sdl_quit_qry = false;

static bool left_button_down = false;
static int16_t last_x = 0;
static int16_t last_y = 0;

static int16_t wheel_diff = 0;
static lv_indev_state_t wheel_state = LV_INDEV_STATE_RELEASED;

static char buf[KEYBOARD_BUFFER_SIZE];

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

    window_create(&monitor);
#if SDL_DUAL_DISPLAY
    window_create(&monitor2);
    int x, y;
    SDL_GetWindowPosition(monitor2.window, &x, &y);
    SDL_SetWindowPosition(monitor.window, x + (SDL_HOR_RES * SDL_ZOOM) / 2 + 10, y);
    SDL_SetWindowPosition(monitor2.window, x - (SDL_HOR_RES * SDL_ZOOM) / 2 - 10, y);
#endif

    SDL_StartTextInput();

#if LV_TICK_CUSTOM == 0
    /* Tick init.
     * You have to call 'lv_tick_inc()' in periodically to inform LittelvGL about
     * how much time were elapsed Create an SDL thread to do this*/
    SDL_CreateThread(tick_thread, "tick", NULL);
#endif
    lv_timer_create(sdl_event_handler, 10, NULL);
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

#if SDL_DOUBLE_BUFFERED
    monitor.tft_fb_act = (uint32_t *)color_p;
#else /*SDL_DOUBLE_BUFFERED*/

    int32_t y;
#if LV_COLOR_DEPTH != 24 && LV_COLOR_DEPTH != 32    /*32 is valid but support 24 for backward compatibility too*/
    int32_t x;
    for(y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            monitor.tft_fb[y * disp_drv->hor_res + x] = lv_color_to32(*color_p);
            color_p++;
        }

    }
#else
    uint32_t w = lv_area_get_width(area);
    for(y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++) {
        memcpy(&monitor.tft_fb[y * SDL_HOR_RES + area->x1], color_p, w * sizeof(lv_color_t));
        color_p += w;
    }
#endif
#endif /*SDL_DOUBLE_BUFFERED*/

    monitor.sdl_refr_qry = true;

    /* TYPICALLY YOU DO NOT NEED THIS
     * If it was the last part to refresh update the texture of the window.*/
    if(lv_disp_flush_is_last(disp_drv)) {
        monitor_sdl_refr(NULL);
    }

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_disp_flush_ready(disp_drv);

}


#if SDL_DUAL_DISPLAY

/**
 * Flush a buffer to the marked area
 * @param disp_drv pointer to driver where this function belongs
 * @param area an area where to copy `color_p`
 * @param color_p an array of pixels to copy to the `area` part of the screen
 */
void sdl_display_flush2(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_coord_t hres = disp_drv->hor_res;
    lv_coord_t vres = disp_drv->ver_res;

    /*Return if the area is out the screen*/
    if(area->x2 < 0 || area->y2 < 0 || area->x1 > hres - 1 || area->y1 > vres - 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

#if SDL_DOUBLE_BUFFERED
    monitor2.tft_fb_act = (uint32_t *)color_p;

    monitor2.sdl_refr_qry = true;

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_disp_flush_ready(disp_drv);
#else

    int32_t y;
#if LV_COLOR_DEPTH != 24 && LV_COLOR_DEPTH != 32    /*32 is valid but support 24 for backward compatibility too*/
    int32_t x;
    for(y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            monitor2.tft_fb[y * disp_drv->hor_res + x] = lv_color_to32(*color_p);
            color_p++;
        }

    }
#else
    uint32_t w = lv_area_get_width(area);
    for(y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++) {
        memcpy(&monitor2.tft_fb[y * disp_drv->hor_res + area->x1], color_p, w * sizeof(lv_color_t));
        color_p += w;
    }
#endif

    monitor2.sdl_refr_qry = true;

    /* TYPICALLY YOU DO NOT NEED THIS
     * If it was the last part to refresh update the texture of the window.*/
    if(lv_disp_flush_is_last(disp_drv)) {
        monitor_sdl_refr(NULL);
    }

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_disp_flush_ready(disp_drv);
#endif
}
#endif


/**
 * Get the current position and state of the mouse
 * @param indev_drv pointer to the related input device driver
 * @param data store the mouse data here
 */
void sdl_mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    (void) indev_drv;      /*Unused*/

    /*Store the collected data*/
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = left_button_down ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}


/**
 * Get encoder (i.e. mouse wheel) ticks difference and pressed state
 * @param indev_drv pointer to the related input device driver
 * @param data store the read data here
 */
void sdl_mousewheel_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    (void) indev_drv;      /*Unused*/

    data->state = wheel_state;
    data->enc_diff = wheel_diff;
    wheel_diff = 0;
}

/**
 * Get input from the keyboard.
 * @param indev_drv pointer to the related input device driver
 * @param data store the red data here
 */
void sdl_keyboard_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    (void) indev_drv;      /*Unused*/

    static bool dummy_read = false;
    const size_t len = strlen(buf);

    /*Send a release manually*/
    if (dummy_read) {
        dummy_read = false;
        data->state = LV_INDEV_STATE_RELEASED;
        data->continue_reading = len > 0;
    }
    /*Send the pressed character*/
    else if (len > 0) {
        dummy_read = true;
        data->state = LV_INDEV_STATE_PRESSED;
        data->key = buf[0];
        memmove(buf, buf + 1, len);
        data->continue_reading = true;
    }
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

        if((&event)->type == SDL_WINDOWEVENT) {
            switch((&event)->window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                case SDL_WINDOWEVENT_EXPOSED:
                    window_update(&monitor);
#if SDL_DUAL_DISPLAY
                    window_update(&monitor2);
#endif
                    break;
                default:
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

/**
 * SDL main thread. All SDL related task have to be handled here!
 * It initializes SDL, handles drawing and the mouse.
 */

static void monitor_sdl_refr(lv_timer_t * t)
{
    (void)t;

    /*Refresh handling*/
    if(monitor.sdl_refr_qry != false) {
        monitor.sdl_refr_qry = false;
        window_update(&monitor);
    }

#if SDL_DUAL_DISPLAY
    if(monitor2.sdl_refr_qry != false) {
        monitor2.sdl_refr_qry = false;
        window_update(&monitor2);
    }
#endif
}

int quit_filter(void * userdata, SDL_Event * event)
{
    (void)userdata;

    if(event->type == SDL_WINDOWEVENT) {
        if(event->window.event == SDL_WINDOWEVENT_CLOSE) {
            sdl_quit_qry = true;
        }
    }
    else if(event->type == SDL_QUIT) {
        sdl_quit_qry = true;
    }

    return 1;
}

static void monitor_sdl_clean_up(void)
{
    SDL_DestroyTexture(monitor.texture);
    SDL_DestroyRenderer(monitor.renderer);
    SDL_DestroyWindow(monitor.window);

#if SDL_DUAL_DISPLAY
    SDL_DestroyTexture(monitor2.texture);
    SDL_DestroyRenderer(monitor2.renderer);
    SDL_DestroyWindow(monitor2.window);

#endif

    SDL_Quit();
}

static void window_create(monitor_t * m)
{

    int flag = 0;
#if SDL_FULLSCREEN
    flag |= SDL_WINDOW_FULLSCREEN;
#endif

    m->window = SDL_CreateWindow("TFT Simulator",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SDL_HOR_RES * SDL_ZOOM, SDL_VER_RES * SDL_ZOOM, flag);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

    m->renderer = SDL_CreateRenderer(m->window, -1, SDL_RENDERER_SOFTWARE);
    m->texture = SDL_CreateTexture(m->renderer,
                                SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SDL_HOR_RES, SDL_VER_RES);
    SDL_SetTextureBlendMode(m->texture, SDL_BLENDMODE_BLEND);

    /*Initialize the frame buffer to gray (77 is an empirical value) */
#if SDL_DOUBLE_BUFFERED
    SDL_UpdateTexture(m->texture, NULL, m->tft_fb_act, SDL_HOR_RES * sizeof(uint32_t));
#else
    m->tft_fb = (uint32_t *)malloc(sizeof(uint32_t) * SDL_HOR_RES * SDL_VER_RES);
    memset(m->tft_fb, 0x44, SDL_HOR_RES * SDL_VER_RES * sizeof(uint32_t));
#endif

    m->sdl_refr_qry = true;

}

static void window_update(monitor_t * m)
{
#if SDL_DOUBLE_BUFFERED == 0
    SDL_UpdateTexture(m->texture, NULL, m->tft_fb, SDL_HOR_RES * sizeof(uint32_t));
#else
    if(m->tft_fb_act == NULL) return;
    SDL_UpdateTexture(m->texture, NULL, m->tft_fb_act, SDL_HOR_RES * sizeof(uint32_t));
#endif
    SDL_RenderClear(m->renderer);
#if LV_COLOR_SCREEN_TRANSP
    SDL_SetRenderDrawColor(m->renderer, 0xff, 0, 0, 0xff);
    SDL_Rect r;
    r.x = 0; r.y = 0; r.w = SDL_HOR_RES; r.h = SDL_VER_RES;
    SDL_RenderDrawRect(m->renderer, &r);
#endif

    /*Update the renderer with the texture containing the rendered image*/
    SDL_RenderCopy(m->renderer, m->texture, NULL, NULL);
    SDL_RenderPresent(m->renderer);
}

static void mouse_handler(SDL_Event * event)
{
    switch(event->type) {
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_LEFT)
                left_button_down = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_LEFT) {
                left_button_down = true;
                last_x = event->motion.x / SDL_ZOOM;
                last_y = event->motion.y / SDL_ZOOM;
            }
            break;
        case SDL_MOUSEMOTION:
            last_x = event->motion.x / SDL_ZOOM;
            last_y = event->motion.y / SDL_ZOOM;
            break;

        case SDL_FINGERUP:
            left_button_down = false;
            last_x = LV_HOR_RES * event->tfinger.x / SDL_ZOOM;
            last_y = LV_VER_RES * event->tfinger.y / SDL_ZOOM;
            break;
        case SDL_FINGERDOWN:
            left_button_down = true;
            last_x = LV_HOR_RES * event->tfinger.x / SDL_ZOOM;
            last_y = LV_VER_RES * event->tfinger.y / SDL_ZOOM;
            break;
        case SDL_FINGERMOTION:
            last_x = LV_HOR_RES * event->tfinger.x / SDL_ZOOM;
            last_y = LV_VER_RES * event->tfinger.y / SDL_ZOOM;
            break;
    }

}


/**
 * It is called periodically from the SDL thread to check mouse wheel state
 * @param event describes the event
 */
static void mousewheel_handler(SDL_Event * event)
{
    switch(event->type) {
        case SDL_MOUSEWHEEL:
            // Scroll down (y = -1) means positive encoder turn,
            // so invert it
#ifdef __EMSCRIPTEN__
            /*Escripten scales it wrong*/
            if(event->wheel.y < 0) wheel_diff++;
            if(event->wheel.y > 0) wheel_diff--;
#else
            wheel_diff = -event->wheel.y;
#endif
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_MIDDLE) {
                wheel_state = LV_INDEV_STATE_PRESSED;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_MIDDLE) {
                wheel_state = LV_INDEV_STATE_RELEASED;
            }
            break;
        default:
            break;
    }
}


/**
 * Called periodically from the SDL thread, store text input or control characters in the buffer.
 * @param event describes the event
 */
static void keyboard_handler(SDL_Event * event)
{
    /* We only care about SDL_KEYDOWN and SDL_TEXTINPUT events */
    switch(event->type) {
        case SDL_KEYDOWN:                       /*Button press*/
            {
                const uint32_t ctrl_key = keycode_to_ctrl_key(event->key.keysym.sym);
                if (ctrl_key == '\0')
                    return;
                const size_t len = strlen(buf);
                if (len < KEYBOARD_BUFFER_SIZE - 1) {
                    buf[len] = ctrl_key;
                    buf[len + 1] = '\0';
                }
                break;
            }
        case SDL_TEXTINPUT:                     /*Text input*/
            {
                const size_t len = strlen(buf) + strlen(event->text.text);
                if (len < KEYBOARD_BUFFER_SIZE - 1)
                    strcat(buf, event->text.text);
            }
            break;
        default:
            break;

    }
}


/**
 * Convert a SDL key code to it's LV_KEY_* counterpart or return '\0' if it's not a control character.
 * @param sdl_key the key code
 * @return LV_KEY_* control character or '\0'
 */
static uint32_t keycode_to_ctrl_key(SDL_Keycode sdl_key)
{
    /*Remap some key to LV_KEY_... to manage groups*/
    switch(sdl_key) {
        case SDLK_RIGHT:
        case SDLK_KP_PLUS:
            return LV_KEY_RIGHT;

        case SDLK_LEFT:
        case SDLK_KP_MINUS:
            return LV_KEY_LEFT;

        case SDLK_UP:
            return LV_KEY_UP;

        case SDLK_DOWN:
            return LV_KEY_DOWN;

        case SDLK_ESCAPE:
            return LV_KEY_ESC;

        case SDLK_BACKSPACE:
            return LV_KEY_BACKSPACE;

        case SDLK_DELETE:
            return LV_KEY_DEL;

        case SDLK_KP_ENTER:
        case '\r':
            return LV_KEY_ENTER;

        case SDLK_TAB:
        case SDLK_PAGEDOWN:
            return LV_KEY_NEXT;

        case SDLK_PAGEUP:
            return LV_KEY_PREV;

        default:
            return '\0';
    }
}


#if LV_TICK_CUSTOM == 0
/**
 * A task to measure the elapsed time for LVGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void *data)
{
    (void)data;

    while(1) {
        SDL_Delay(5);
        lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
    }

    return 0;
}
#endif

#endif /*USE_MONITOR || USE_SDL*/
