/**
 * @file xkb.h
 *
 */

#ifndef XKB_H
#define XKB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifndef LV_DRV_NO_CONF
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_drv_conf.h"
#else
#include "../../lv_drv_conf.h"
#endif
#endif

#if USE_XKB

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct xkb_rule_names;

typedef struct {
  struct xkb_keymap *keymap;
  struct xkb_state *state;
} xkb_drv_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialise the XKB system using the default driver state. Use this function if you only want
 * to connect a single device.
 * @return true if the initialisation was successful
 */
bool xkb_init(void);
/**
 * Initialise the XKB system using a specific driver state. Use this function if you want to
 * connect multiple devices.
 * @param state XKB driver state to use
 * @return true if the initialisation was successful
 */
bool xkb_init_state(xkb_drv_state_t *state);
/**
 * Set a new keymap to be used for processing future key events using the default driver state. Use
 * this function if you only want to connect a single device.
 * @param names XKB rule names structure (use NULL components for default values)
 * @return true if creating the keymap and associated state succeeded
 */
bool xkb_set_keymap(struct xkb_rule_names names);
/**
 * Set a new keymap to be used for processing future key events using a specific driver state. Use
 * this function if you want to connect multiple devices.
 * @param state XKB driver state to use
 * @param names XKB rule names structure (use NULL components for default values)
 * @return true if creating the keymap and associated state succeeded
 */
bool xkb_set_keymap_state(xkb_drv_state_t *state, struct xkb_rule_names names);
/**
 * Process an evdev scancode using the default driver state. Use this function if you only want to
 * connect a single device.
 * @param scancode evdev scancode to process
 * @param down true if the key was pressed, false if it was releases
 * @return the (first) UTF-8 character produced by the event or 0 if no output was produced
 */
uint32_t xkb_process_key(uint32_t scancode, bool down);
/**
 * Process an evdev scancode using a specific driver state. Use this function if you want to connect
 * multiple devices.
 * @param state XKB driver state to use
 * @param scancode evdev scancode to process
 * @param down true if the key was pressed, false if it was releases
 * @return the (first) UTF-8 character produced by the event or 0 if no output was produced
 */
uint32_t xkb_process_key_state(xkb_drv_state_t *state, uint32_t scancode, bool down);

/**********************
 *      MACROS
 **********************/

#endif /* USE_XKB */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* XKB_H */
