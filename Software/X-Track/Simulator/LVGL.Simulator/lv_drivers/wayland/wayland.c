/**
 * @file wayland.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "wayland.h"

#if USE_WAYLAND

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <sys/mman.h>

#include <linux/input.h>

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct input {
    struct {
        lv_coord_t x;
        lv_coord_t y;
        lv_indev_state_t left_button;
        lv_indev_state_t right_button;
        lv_indev_state_t wheel_button;
        int16_t wheel_diff;
    } mouse;

    struct {
        lv_key_t key;
        lv_indev_state_t state;
    } keyboard;

    struct {
        lv_coord_t x;
        lv_coord_t y;
        lv_indev_state_t state;
    } touch;
};

struct seat {
    struct application *application;
    struct wl_seat *wl_seat;
    struct wl_touch *wl_touch;
    struct wl_pointer *wl_pointer;
    struct wl_keyboard *wl_keyboard;

    struct {
        struct xkb_keymap *keymap;
        struct xkb_state *state;
    } xkb;
};

struct application {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_shm *shm;
    struct wl_buffer *buffer;
    struct wl_surface *surface;

    struct wl_shell *shell;
    struct wl_shell_surface *shell_surface;

    int width;
    int height;
    uint32_t format;
    void *data;

    struct xkb_context *xkb_context;
    struct seat seat;
    struct input input;

    pthread_t thread;
    pthread_mutex_t mutex;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void * wayland_dispatch_handler(void *data);
static void handle_global(void *data, struct wl_registry *registry, uint32_t name,
                          const char *interface, uint32_t version);
static void handle_global_remove(void *data, struct wl_registry *registry, uint32_t name);

static void shm_format(void *data, struct wl_shm *wl_shm, uint32_t format);

static void shell_handle_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial);
static void shell_handle_configure(void *data, struct wl_shell_surface *shell_surface,
                                   uint32_t edges, int32_t width, int32_t height);
static void shell_handle_popup_done(void *data, struct wl_shell_surface *shell_surface);
static void seat_handle_capabilities(void *data, struct wl_seat *wl_seat, enum wl_seat_capability caps);

static void pointer_handle_enter(void *data, struct wl_pointer *pointer,
                                 uint32_t serial, struct wl_surface *surface,
                                 wl_fixed_t sx, wl_fixed_t sy);
static void pointer_handle_leave(void *data, struct wl_pointer *pointer,
                                 uint32_t serial, struct wl_surface *surface);
static void pointer_handle_motion(void *data, struct wl_pointer *pointer,
                                  uint32_t time, wl_fixed_t sx, wl_fixed_t sy);
static void pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
                                  uint32_t serial, uint32_t time, uint32_t button,
                                  uint32_t state);
static void pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                                uint32_t time, uint32_t axis, wl_fixed_t value);

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
                                   uint32_t format, int fd, uint32_t size);
static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
                                  uint32_t serial, struct wl_surface *surface,
                                  struct wl_array *keys);
static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
                                  uint32_t serial, struct wl_surface *surface);
static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
                                uint32_t serial, uint32_t time, uint32_t key,
                                uint32_t state);
static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
                                      uint32_t serial, uint32_t mods_depressed,
                                      uint32_t mods_latched, uint32_t mods_locked,
                                      uint32_t group);

static void touch_handle_down(void *data, struct wl_touch *wl_touch,
                              uint32_t serial, uint32_t time, struct wl_surface *surface,
                              int32_t id, wl_fixed_t x_w, wl_fixed_t y_w);
static void touch_handle_up(void *data, struct wl_touch *wl_touch,
                            uint32_t serial, uint32_t time, int32_t id);
static void touch_handle_motion(void *data, struct wl_touch *wl_touch,
                                uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w);
static void touch_handle_frame(void *data, struct wl_touch *wl_touch);
static void touch_handle_cancel(void *data, struct wl_touch *wl_touch);

static lv_key_t keycode_xkb_to_lv(uint32_t xkb_key);

/**********************
 *  STATIC VARIABLES
 **********************/
static const struct wl_registry_listener registry_listener = {
    handle_global,
    handle_global_remove
};

struct wl_shm_listener shm_listener = {
    shm_format
};

static const struct wl_shell_surface_listener shell_surface_listener = {
    shell_handle_ping,
    shell_handle_configure,
    shell_handle_popup_done
};

static const struct wl_seat_listener seat_listener = {
    seat_handle_capabilities,
};

static const struct wl_pointer_listener pointer_listener = {
    pointer_handle_enter,
    pointer_handle_leave,
    pointer_handle_motion,
    pointer_handle_button,
    pointer_handle_axis,
};

static const struct wl_keyboard_listener keyboard_listener = {
    keyboard_handle_keymap,
    keyboard_handle_enter,
    keyboard_handle_leave,
    keyboard_handle_key,
    keyboard_handle_modifiers,
};

static const struct wl_touch_listener touch_listener = {
    touch_handle_down,
    touch_handle_up,
    touch_handle_motion,
    touch_handle_frame,
    touch_handle_cancel,
};

static struct application application;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * Initialize Wayland driver
 */
void wayland_init(void)
{
    struct wl_shm_pool *pool;

    int stride;
    int size;

    static const char template[] = "/lvgl-wayland-XXXXXX";
    const char *path;
    char *name;
    int fd;
    int ret;

    // Create XKB context
    application.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    assert(application.xkb_context);
    if (application.xkb_context == NULL) {
        LV_LOG_ERROR("failed to create XKB context\n");
        return;
    }

    // Connect to Wayland display
    application.display = wl_display_connect(NULL);
    assert(application.display);

    // Create compositor surface
    application.width = WAYLAND_HOR_RES;
    application.height = WAYLAND_VER_RES;

    /* Add registry listener and wait for registry reception */
    application.format = 0xFFFFFFFF;
    application.registry = wl_display_get_registry(application.display);
    wl_registry_add_listener(application.registry, &registry_listener, &application);
    wl_display_dispatch(application.display);
    wl_display_roundtrip(application.display);

    assert(application.format != 0xFFFFFFFF);
    if (application.format == 0xFFFFFFFF) {
        LV_LOG_ERROR("WL_SHM_FORMAT not available\n");
        return;
    }

    // Create buffer
    stride = application.width * ((LV_COLOR_DEPTH + 7) / 8);
    size = stride * application.height;

    path = getenv("XDG_RUNTIME_DIR");
    if (!path) {
        LV_LOG_ERROR("cannot get XDG_RUNTIME_DIR: %s\n", strerror(errno));
        return;
    }

    name = malloc(strlen(path) + sizeof(template));
    if (!name) {
        LV_LOG_ERROR("cannot malloc name: %s\n", strerror(errno));
        return;
    }

    strcpy(name, path);
    strcat(name, template);

    fd = mkstemp(name);
    if (fd >= 0) {
        long flags = fcntl(fd, F_GETFD);
        if ((flags == -1) || (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)) {
            LV_LOG_ERROR("cannot set FD_CLOEXEC\n");
            close(fd);
            fd = -1;
        }
        unlink(name);
    }

    free(name);

    if (fd < 0) {
        LV_LOG_ERROR("cannot create tmpfile: %s\n", strerror(errno));
        return;
    }

    do {
        ret = ftruncate(fd, size);
    } while ((ret < 0) && (errno == EINTR));
    if (ret < 0) {
        LV_LOG_ERROR("ftruncate failed: %s\n", strerror(errno));
        close(fd);
        return;
    }

    application.data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (application.data == MAP_FAILED) {
        LV_LOG_ERROR("mmap failed: %s\n", strerror(errno));
        close(fd);
        return;
    }

    pool = wl_shm_create_pool(application.shm, fd, size);
    application.buffer = wl_shm_pool_create_buffer(pool, 0,
                                                   application.width, application.height,
                                                   stride,
                                                   application.format);
    wl_shm_pool_destroy(pool);

    close(fd);

    // Create compositor surface
    application.surface = wl_compositor_create_surface(application.compositor);
    wl_surface_set_user_data(application.surface, &application);

    // Create shell surface
    application.shell_surface = wl_shell_get_shell_surface(application.shell, application.surface);
    assert(application.shell_surface);

    wl_shell_surface_add_listener(application.shell_surface, &shell_surface_listener, &application);
    wl_shell_surface_set_toplevel(application.shell_surface);
    wl_shell_surface_set_title(application.shell_surface, WAYLAND_SURF_TITLE);

    pthread_mutex_init(&application.mutex, NULL);
    pthread_create(&application.thread, NULL, wayland_dispatch_handler, &application);
}

/**
 * De-initialize Wayland driver
 */
void wayland_deinit(void)
{
    pthread_cancel(application.thread);

    pthread_join(application.thread, NULL);

    pthread_mutex_destroy(&application.mutex);

    if (application.shm) {
        wl_shm_destroy(application.shm);
    }

    if (application.shell) {
        wl_shell_destroy(application.shell);
    }

    if (application.seat.wl_seat) {
        wl_seat_destroy(application.seat.wl_seat);
    }

    if (application.compositor) {
        wl_compositor_destroy(application.compositor);
    }

    wl_registry_destroy(application.registry);
    wl_display_flush(application.display);
    wl_display_disconnect(application.display);
}

/**
 * Flush a buffer to the marked area
 * @param drv pointer to driver where this function belongs
 * @param area an area where to copy `color_p`
 * @param color_p an array of pixel to copy to the `area` part of the screen
 */
void wayland_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_coord_t hres = (disp_drv->rotated == 0) ? (disp_drv->hor_res) : (disp_drv->ver_res);
    lv_coord_t vres = (disp_drv->rotated == 0) ? (disp_drv->ver_res) : (disp_drv->hor_res);

    /* Return if the area is out the screen */
    if ((area->x2 < 0) || (area->y2 < 0) || (area->x1 > hres - 1) || (area->y1 > vres - 1)) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    int32_t x;
    int32_t y;
    for (y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++) {
        for (x = area->x1; x <= area->x2 && x < disp_drv->hor_res; x++) {
            int offset = (y * disp_drv->hor_res) + x;
#if (LV_COLOR_DEPTH == 32)
            uint32_t * const buf = (uint32_t *)application.data + offset;
            *buf = color_p->full;
#elif (LV_COLOR_DEPTH == 16)
            uint16_t * const buf = (uint16_t *)application.data + offset;
            *buf = color_p->full;
#elif (LV_COLOR_DEPTH == 8)
            uint8_t * const buf = (uint8_t *)application.data + offset;
            *buf = color_p->full;
#elif (LV_COLOR_DEPTH == 1)
            uint8_t * const buf = (uint8_t *)application.data + offset;
            *buf = ((0x07 * color_p->ch.red)   << 5) |
                   ((0x07 * color_p->ch.green) << 2) |
                   ((0x03 * color_p->ch.blue)  << 0);
#endif
            color_p++;
        }
    }

    wl_surface_attach(application.surface, application.buffer, 0, 0);
    wl_surface_damage(application.surface, area->x1, area->y1,
                      (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1));
    wl_surface_commit(application.surface);

    wl_display_flush(application.display);

    lv_disp_flush_ready(disp_drv);
}

/**
 * Read pointer input
 * @param drv pointer to driver where this function belongs
 * @param data where to store input data
 */
bool wayland_pointer_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    (void) drv; /* Unused */

    pthread_mutex_lock(&application.mutex);

    data->point.x = application.input.mouse.x;
    data->point.y = application.input.mouse.y;
    data->state = application.input.mouse.left_button;

    pthread_mutex_unlock(&application.mutex);

    return false;
}

/**
 * Read axis input
 * @param drv pointer to driver where this function belongs
 * @param data where to store input data
 */
bool wayland_pointeraxis_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    (void) drv; /* Unused */

    pthread_mutex_lock(&application.mutex);

    data->state = application.input.mouse.wheel_button;
    data->enc_diff = application.input.mouse.wheel_diff;

    application.input.mouse.wheel_diff = 0;

    pthread_mutex_unlock(&application.mutex);

    return false;
}

/**
 * Read keyboard input
 * @param drv pointer to driver where this function belongs
 * @param data where to store input data
 */
bool wayland_keyboard_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    (void) drv; /* Unused */

    pthread_mutex_lock(&application.mutex);

    data->key = application.input.keyboard.key;
    data->state = application.input.keyboard.state;

    pthread_mutex_unlock(&application.mutex);

    return false;
}

/**
 * Read touch input
 * @param drv pointer to driver where this function belongs
 * @param data where to store input data
 */
bool wayland_touch_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
    (void) drv; /* Unused */

    pthread_mutex_lock(&application.mutex);

    data->point.x = application.input.touch.x;
    data->point.y = application.input.touch.y;
    data->state = application.input.touch.state;

    pthread_mutex_unlock(&application.mutex);

    return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void * wayland_dispatch_handler(void *data)
{
    struct application *app = data;

    while (wl_display_dispatch(app->display) >= 0) {
        // Do nothing
    }

    return (void *)0;
}

static void handle_global(void *data, struct wl_registry *registry,
                          uint32_t name, const char *interface, uint32_t version)
{
    struct application *app = data;

    if (strcmp(interface, "wl_compositor") == 0) {
        app->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
    } else if (strcmp(interface, "wl_shell") == 0) {
        app->shell = wl_registry_bind(registry, name, &wl_shell_interface, 1);
    } else if (strcmp(interface, "wl_shm") == 0) {
        app->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
        wl_shm_add_listener(app->shm, &shm_listener, app);
    } else if (strcmp(interface, "wl_seat") == 0) {
        app->seat.application = app;
        app->seat.wl_seat = wl_registry_bind(app->registry, name, &wl_seat_interface, 1);
        wl_seat_add_listener(app->seat.wl_seat, &seat_listener, &app->seat);
    }
}

static void handle_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
}


static void shm_format(void *data, struct wl_shm *wl_shm, uint32_t format)
{
    struct application *app = data;

    switch (format) {
#if (LV_COLOR_DEPTH == 32)
    case WL_SHM_FORMAT_ARGB8888:
        app->format = format;
        break;
    case WL_SHM_FORMAT_XRGB8888:
        if (app->format != WL_SHM_FORMAT_ARGB8888) {
            app->format = format;
        }
        break;
#elif (LV_COLOR_DEPTH == 16)
    case WL_SHM_FORMAT_RGB565:
        app->format = format;
        break;
#elif (LV_COLOR_DEPTH == 8)
    case WL_SHM_FORMAT_RGB332:
        app->format = format;
        break;
#elif (LV_COLOR_DEPTH == 1)
    case WL_SHM_FORMAT_RGB332:
        app->format = format;
        break;
#endif
    default:
        break;
    }
}

static void shell_handle_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial)
{
    wl_shell_surface_pong(shell_surface, serial);
}

static void shell_handle_configure(void *data, struct wl_shell_surface *shell_surface,
                                   uint32_t edges, int32_t width, int32_t height)
{
}

static void shell_handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
}

static void seat_handle_capabilities(void *data, struct wl_seat *wl_seat, enum wl_seat_capability caps)
{
    struct seat *seat = data;

    if ((caps & WL_SEAT_CAPABILITY_POINTER) && !seat->wl_pointer) {
        seat->wl_pointer = wl_seat_get_pointer(wl_seat);
        wl_pointer_add_listener(seat->wl_pointer, &pointer_listener, seat->application);
    } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && seat->wl_pointer) {
        wl_pointer_destroy(seat->wl_pointer);
        seat->wl_pointer = NULL;
    }

    if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !seat->wl_keyboard) {
        seat->wl_keyboard = wl_seat_get_keyboard(wl_seat);
        wl_keyboard_add_listener(seat->wl_keyboard, &keyboard_listener, seat->application);
    } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && seat->wl_keyboard) {
        wl_keyboard_destroy(seat->wl_keyboard);
        seat->wl_keyboard = NULL;
    }

    if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !seat->wl_touch) {
        seat->wl_touch = wl_seat_get_touch(wl_seat);
        wl_touch_add_listener(seat->wl_touch, &touch_listener, seat->application);
    } else if (!(caps & WL_SEAT_CAPABILITY_TOUCH) && seat->wl_touch) {
        wl_touch_destroy(seat->wl_touch);
        seat->wl_touch = NULL;
    }
}

static void pointer_handle_enter(void *data, struct wl_pointer *pointer,
                                 uint32_t serial, struct wl_surface *surface,
                                 wl_fixed_t sx, wl_fixed_t sy)
{
}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer,
                                 uint32_t serial, struct wl_surface *surface)
{
}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer,
                                  uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    struct application *app = data;

    pthread_mutex_lock(&app->mutex);

    app->input.mouse.x = wl_fixed_to_int(sx);
    app->input.mouse.y = wl_fixed_to_int(sy);

    pthread_mutex_unlock(&app->mutex);
}

static void pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
                                  uint32_t serial, uint32_t time, uint32_t button,
                                  uint32_t state)
{
    struct application *app = data;
    const lv_indev_state_t lv_state =
        (state == WL_POINTER_BUTTON_STATE_PRESSED) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    pthread_mutex_lock(&app->mutex);

    switch (button & 0xF) {
    case 0:
        app->input.mouse.left_button = lv_state;
        break;
    case 1:
        app->input.mouse.right_button = lv_state;
        break;
    case 2:
        app->input.mouse.wheel_button = lv_state;
        break;
    default:
        break;
    }

    pthread_mutex_unlock(&app->mutex);
}

static void pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                                uint32_t time, uint32_t axis, wl_fixed_t value)
{
    struct application *app = data;
    const int diff = wl_fixed_to_int(value);

    if (axis == 0) {
        pthread_mutex_lock(&app->mutex);
        if (diff > 0) {
            app->input.mouse.wheel_diff++;
        }
        else if (diff < 0) {
            app->input.mouse.wheel_diff--;
        }
        pthread_mutex_unlock(&app->mutex);
    }
}

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
                                   uint32_t format, int fd, uint32_t size)
{
    struct application *app = data;

    struct xkb_keymap *keymap;
    struct xkb_state *state;
    char *map_str;

    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        return;
    }

    map_str = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_str == MAP_FAILED) {
        close(fd);
        return;
    }

    /* Set up XKB keymap */
    keymap = xkb_keymap_new_from_string(app->xkb_context, map_str,
                                        XKB_KEYMAP_FORMAT_TEXT_V1, 0);
    munmap(map_str, size);
    close(fd);

    if (!keymap) {
        LV_LOG_ERROR("failed to compile keymap\n");
        return;
    }

    /* Set up XKB state */
    state = xkb_state_new(keymap);
    if (!state) {
        LV_LOG_ERROR("failed to create XKB state\n");
        xkb_keymap_unref(keymap);
        return;
    }

    xkb_keymap_unref(app->seat.xkb.keymap);
    xkb_state_unref(app->seat.xkb.state);
    app->seat.xkb.keymap = keymap;
    app->seat.xkb.state = state;
}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
                                  uint32_t serial, struct wl_surface *surface,
                                  struct wl_array *keys)
{
}

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
                                  uint32_t serial, struct wl_surface *surface)
{
}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
                                uint32_t serial, uint32_t time, uint32_t key,
                                uint32_t state)
{
    struct application *app = data;

    const uint32_t code = (key + 8);
    const xkb_keysym_t *syms;
    xkb_keysym_t sym = XKB_KEY_NoSymbol;

    if (!app->seat.xkb.state)
        return;

    if (xkb_state_key_get_syms(app->seat.xkb.state, code, &syms) == 1) {
        sym = syms[0];
    }

    const lv_key_t lv_key = keycode_xkb_to_lv(sym);
    const lv_indev_state_t lv_state =
        (state == WL_KEYBOARD_KEY_STATE_PRESSED) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    if (lv_key != 0) {
        pthread_mutex_lock(&app->mutex);
        app->input.keyboard.key = lv_key;
        app->input.keyboard.state = lv_state;
        pthread_mutex_unlock(&app->mutex);
    }
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
                                      uint32_t serial, uint32_t mods_depressed,
                                      uint32_t mods_latched, uint32_t mods_locked,
                                      uint32_t group)
{
    struct application *app = data;

    /* If we're not using a keymap, then we don't handle PC-style modifiers */
    if (!app->seat.xkb.keymap)
        return;

    xkb_state_update_mask(app->seat.xkb.state,
                          mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

static void touch_handle_down(void *data, struct wl_touch *wl_touch,
                              uint32_t serial, uint32_t time, struct wl_surface *surface,
                              int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    struct application *app = data;

    pthread_mutex_lock(&app->mutex);

    app->input.touch.x = wl_fixed_to_int(x_w);
    app->input.touch.y = wl_fixed_to_int(y_w);
    app->input.touch.state = LV_INDEV_STATE_PR;

    pthread_mutex_unlock(&app->mutex);
}

static void touch_handle_up(void *data, struct wl_touch *wl_touch,
                            uint32_t serial, uint32_t time, int32_t id)
{
    struct application *app = data;

    pthread_mutex_lock(&app->mutex);

    app->input.touch.state = LV_INDEV_STATE_REL;

    pthread_mutex_unlock(&app->mutex);
}

static void touch_handle_motion(void *data, struct wl_touch *wl_touch,
                                uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    struct application *app = data;

    pthread_mutex_lock(&app->mutex);

    app->input.touch.x = wl_fixed_to_int(x_w);
    app->input.touch.y = wl_fixed_to_int(y_w);

    pthread_mutex_unlock(&app->mutex);
}

static void touch_handle_frame(void *data, struct wl_touch *wl_touch)
{
}

static void touch_handle_cancel(void *data, struct wl_touch *wl_touch)
{
}

static lv_key_t keycode_xkb_to_lv(xkb_keysym_t xkb_key)
{
    lv_key_t key = 0;

    if (((xkb_key >= XKB_KEY_space) && (xkb_key <= XKB_KEY_asciitilde))) {
        key = xkb_key; 
    } else if (((xkb_key >= XKB_KEY_KP_0) && (xkb_key <= XKB_KEY_KP_9))) {
        key = (xkb_key & 0x003f);
    } else {
        switch (xkb_key)
        {
        case XKB_KEY_BackSpace:
            key = LV_KEY_BACKSPACE;
            break;
        case XKB_KEY_Return:
        case XKB_KEY_KP_Enter:
            key = LV_KEY_ENTER;
            break;
        case XKB_KEY_Escape:
            key = LV_KEY_ESC;
            break;
        case XKB_KEY_Delete:
        case XKB_KEY_KP_Delete:
            key = LV_KEY_DEL;
            break;
        case XKB_KEY_Home:
        case XKB_KEY_KP_Home:
            key = LV_KEY_HOME;
            break;
        case XKB_KEY_Left:
        case XKB_KEY_KP_Left:
            key = LV_KEY_LEFT;
            break;
        case XKB_KEY_Up:
        case XKB_KEY_KP_Up:
            key = LV_KEY_UP;
            break;
        case XKB_KEY_Right:
        case XKB_KEY_KP_Right:
            key = LV_KEY_RIGHT;
            break;
        case XKB_KEY_Down:
        case XKB_KEY_KP_Down:
            key = LV_KEY_DOWN;
            break;
        case XKB_KEY_Prior:
        case XKB_KEY_KP_Prior:
            key = LV_KEY_PREV;
            break;
        case XKB_KEY_Next:
        case XKB_KEY_KP_Next:
        case XKB_KEY_Tab:
        case XKB_KEY_KP_Tab:
            key = LV_KEY_NEXT;
            break;
        case XKB_KEY_End:
        case XKB_KEY_KP_End:
            key = LV_KEY_END;
            break;
        default:
            break;
        }
    }

    return key;
}

#endif /* USE_WAYLAND */
