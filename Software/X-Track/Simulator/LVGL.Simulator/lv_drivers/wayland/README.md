# Wayland display and input driver

Wayland display and input driver, with support for keyboard, mouse and touchscreen.
Keyboard support is based on libxkbcommon.

> NOTE: current implementation only supports `wl_shell` shell with no decorations.


## Install headers and libraries

### Ubuntu

```
sudo apt-get install libwayland-dev libxkbcommon-dev
```

### Fedora

```
sudo dnf install wayland-devel libxkbcommon-devel
```


## Build configuration under Eclipse

In "Project properties > C/C++ Build > Settings" set the followings:

- "Cross GCC Compiler > Command line pattern"
  - Add ` ${wayland-cflags}` and ` ${xkbcommon-cflags}` to the end (add a space between the last command and this)


- "Cross GCC Linker > Command line pattern"
  - Add ` ${wayland-libs}` and ` ${xkbcommon-libs}`  to the end (add a space between the last command and this)


- "Cross GCC Linker > Libraries"
  - Add `pthread`


- In "C/C++ Build > Build variables"
  - Configuration: [All Configuration]

  - Add
    - Variable name: `wayland-cflags`
      - Type: `String`
      - Value: `pkg-config --cflags wayland-client`
    - Variable name: `wayland-libs`
      - Type: `String`
      - Value: `pkg-config --libs wayland-client`
    - Variable name: `xkbcommon-cflags`
      - Type: `String`
      - Value: `pkg-config --cflags xkbcommon`
    - Variable name: `xkbcommon-libs`
      - Type: `String`
      - Value: `pkg-config --libs xkbcommon`


## Init Wayland in LVGL

1. In `main.c` `#incude "lv_drivers/wayland/wayland.h"`
2. Enable the Wayland driver in `lv_drv_conf.h` with `USE_WAYLAND 1`
3. `LV_COLOR_DEPTH` should be set either to `32` or `16` in `lv_conf.h`;
   support for `8` and `1` depends on target platform.
4. After `lv_init()` call `wayland_init()`
5. Before `lv_deinit()` call `wayland_deinit()`
6. Add a display:
```c
  static lv_disp_buf_t disp_buf1;
  static lv_color_t buf1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
  lv_disp_buf_init(&disp_buf1, buf1, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);

  /* Create a display */
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.buffer = &disp_buf1;
  disp_drv.flush_cb = wayland_flush;
```
7. Add keyboard:
```c
  lv_indev_drv_t indev_drv_kb;
  lv_indev_drv_init(&indev_drv_kb);
  indev_drv_kb.type = LV_INDEV_TYPE_KEYPAD;
  indev_drv_kb.read_cb = wayland_keyboard_read;
  lv_indev_drv_register(&indev_drv_kb);
```
8. Add touchscreen:
```c
  lv_indev_drv_t indev_drv_touch;
  lv_indev_drv_init(&indev_drv_touch);
  indev_drv_touch.type = LV_INDEV_TYPE_POINTER;
  indev_drv_touch.read_cb = wayland_touch_read;
  lv_indev_drv_register(&indev_drv_touch);
```
9. Add mouse:
```c
  lv_indev_drv_t indev_drv_mouse;
  lv_indev_drv_init(&indev_drv_mouse);
  indev_drv_mouse.type = LV_INDEV_TYPE_POINTER;
  indev_drv_mouse.read_cb = wayland_pointer_read;
  lv_indev_drv_register(&indev_drv_mouse);
```
10. Add mouse wheel as encoder:
```c
  lv_indev_drv_t indev_drv_mousewheel;
  lv_indev_drv_init(&indev_drv_mousewheel);
  indev_drv_mousewheel.type = LV_INDEV_TYPE_ENCODER;
  indev_drv_mousewheel.read_cb = wayland_pointeraxis_read;
  lv_indev_drv_register(&indev_drv_mousewheel);
```
