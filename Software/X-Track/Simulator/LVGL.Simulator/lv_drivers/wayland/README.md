# Wayland display and input driver

Wayland display and input driver, with support for keyboard, pointer (i.e. mouse) and touchscreen.
Keyboard support is based on libxkbcommon.

Following shell are supported:

* wl_shell (deprecated)
* xdg_shell

> xdg_shell requires an extra build step; see section _Generate protocols_ below.


Basic client-side window decorations (simple title bar, minimize and close buttons)
are supported, while integration with desktop environments is not.


## Install headers and libraries

### Ubuntu

```
sudo apt-get install libwayland-dev libxkbcommon-dev libwayland-bin wayland-protocols
```

### Fedora

```
sudo dnf install wayland-devel libxkbcommon-devel wayland-utils wayland-protocols-devel
```


## Generate protocols

Support for non-basic shells (i.e. other than _wl_shell_) requires additional
source files to be generated before the first build of the project. To do so,
navigate to the _wayland_ folder (the one which includes this file) and issue
the following commands:

```
cmake .
make
```


## Build configuration under Eclipse

In "Project properties > C/C++ Build > Settings" set the followings:

- "Cross GCC Compiler > Command line pattern"
  - Add ` ${wayland-cflags}` and ` ${xkbcommon-cflags}` to the end (add a space between the last command and this)


- "Cross GCC Linker > Command line pattern"
  - Add ` ${wayland-libs}` and ` ${xkbcommon-libs}`  to the end (add a space between the last command and this)


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
2. Enable the Wayland driver in `lv_drv_conf.h` with `USE_WAYLAND 1` and
   configure its features below, enabling at least support for one shell.
3. `LV_COLOR_DEPTH` should be set either to `32` or `16` in `lv_conf.h`;
   support for `8` and `1` depends on target platform.
4. After `lv_init()` call `lv_wayland_init()`.
5. Add a display (or more than one) using `lv_wayland_create_window()`,
   possibly with a close callback to track the status of each display:
```c
  #define H_RES (800)
  #define V_RES (480)

  /* Create a display */
  lv_disp_t * disp = lv_wayland_create_window(H_RES, V_RES, "Window Title", close_cb);
```
  As part of the above call, the Wayland driver will register four input devices
  for each display:
  - a KEYPAD connected to Wayland keyboard events
  - a POINTER connected to Wayland touch events
  - a POINTER connected to Wayland pointer events
  - a ENCODER connected to Wayland pointer axis events
  Handles for input devices of each display can be get using respectively
  `lv_wayland_get_indev_keyboard()`, `lv_wayland_get_indev_touchscreen()`,
  `lv_wayland_get_indev_pointer()` and `lv_wayland_get_indev_pointeraxis()`, using
  `disp` as argument.
5. After `lv_deinit()` (if used), or in any case during de-initialization, call
  `lv_wayland_deinit()`.

### Fullscreen mode

In order to set one window as fullscreen or restore it as a normal one,
call the `lv_wayland_window_set_fullscreen()` function respectively with `true`
or `false` as `fullscreen` argument.

### Disable window client-side decoration at runtime

Even when client-side decorations are enabled at compile time, they can be
disabled at runtime setting the `LV_WAYLAND_DISABLE_WINDOWDECORATION`
environment variable to `1`.
