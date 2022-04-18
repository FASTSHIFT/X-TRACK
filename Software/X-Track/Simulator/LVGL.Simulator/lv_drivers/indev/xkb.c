/**
 * @file xkb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "xkb.h"
#if USE_XKB

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xkbcommon/xkbcommon.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static struct xkb_context *context = NULL;
static xkb_drv_state_t default_state = { .keymap = NULL, .state = NULL };

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialise the XKB system using the default driver state. Use this function if you only want
 * to connect a single device.
 * @return true if the initialisation was successful
 */
bool xkb_init(void) {
  return xkb_init_state(&default_state);
}

/**
 * Initialise the XKB system using a specific driver state. Use this function if you want to
 * connect multiple devices.
 * @param state XKB driver state to use
 * @return true if the initialisation was successful
 */
bool xkb_init_state(xkb_drv_state_t *state) {
  if (!context) {
    context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (!context) {
      perror("could not create new XKB context");
      return false;
    }
  }

#ifdef XKB_KEY_MAP
  struct xkb_rule_names names = XKB_KEY_MAP;
  return xkb_set_keymap_state(state, names);
#else
  return false; /* Keymap needs to be set manually using xkb_set_keymap_state to complete initialisation */
#endif
}

/**
 * Set a new keymap to be used for processing future key events using the default driver state. Use
 * this function if you only want to connect a single device.
 * @param names XKB rule names structure (use NULL components for default values)
 * @return true if creating the keymap and associated state succeeded
 */
bool xkb_set_keymap(struct xkb_rule_names names) {
  return xkb_set_keymap_state(&default_state, names);
}

/**
 * Set a new keymap to be used for processing future key events using a specific driver state. Use
 * this function if you want to connect multiple devices.
 * @param state XKB driver state to use
 * @param names XKB rule names structure (use NULL components for default values)
 * @return true if creating the keymap and associated state succeeded
 */
bool xkb_set_keymap_state(xkb_drv_state_t *state, struct xkb_rule_names names) {
  if (state->keymap) {
    xkb_keymap_unref(state->keymap);
    state->keymap = NULL;
  }
 
  state->keymap = xkb_keymap_new_from_names(context, &names, XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!state->keymap) {
    perror("could not create XKB keymap");
    return false;
  }

  state->keymap = xkb_keymap_ref(state->keymap);
  if (!state->keymap) {
    perror("could not reference XKB keymap");
    return false;
  }

  if (state->state) {
    xkb_state_unref(state->state);
    state->state = NULL;
  }

  state->state = xkb_state_new(state->keymap);
  if (!state->state) {
    perror("could not create XKB state");
    return false;
  }

  state->state = xkb_state_ref(state->state);
  if (!state->state) {
    perror("could not reference XKB state");
    return false;
  }

  return true;
}

/**
 * Process an evdev scancode using the default driver state. Use this function if you only want to
 * connect a single device.
 * @param scancode evdev scancode to process
 * @param down true if the key was pressed, false if it was releases
 * @return the (first) UTF-8 character produced by the event or 0 if no output was produced
 */
uint32_t xkb_process_key(uint32_t scancode, bool down) {
  return xkb_process_key_state(&default_state, scancode, down);
}

/**
 * Process an evdev scancode using a specific driver state. Use this function if you want to connect
 * multiple devices.
 * @param state XKB driver state to use
 * @param scancode evdev scancode to process
 * @param down true if the key was pressed, false if it was releases
 * @return the (first) UTF-8 character produced by the event or 0 if no output was produced
 */
uint32_t xkb_process_key_state(xkb_drv_state_t *state, uint32_t scancode, bool down) {
  /* Offset the evdev scancode by 8, see https://xkbcommon.org/doc/current/xkbcommon_8h.html#ac29aee92124c08d1953910ab28ee1997 */
  xkb_keycode_t keycode = scancode + 8;

  uint32_t result = 0;

  switch (xkb_state_key_get_one_sym(state->state, keycode)) {
    case XKB_KEY_BackSpace:
      result = LV_KEY_BACKSPACE;
      break;
    case XKB_KEY_Return:
    case XKB_KEY_KP_Enter:
      result = LV_KEY_ENTER;
      break;
    case XKB_KEY_Prior:
    case XKB_KEY_KP_Prior:
      result = LV_KEY_PREV;
      break;
    case XKB_KEY_Next:
    case XKB_KEY_KP_Next:
      result = LV_KEY_NEXT;
      break;
    case XKB_KEY_Up:
    case XKB_KEY_KP_Up:
      result = LV_KEY_UP;
      break;
    case XKB_KEY_Left:
    case XKB_KEY_KP_Left:
      result = LV_KEY_LEFT;
      break;
    case XKB_KEY_Right:
    case XKB_KEY_KP_Right:
      result = LV_KEY_RIGHT;
      break;
    case XKB_KEY_Down:
    case XKB_KEY_KP_Down:
      result = LV_KEY_DOWN;
      break;
    case XKB_KEY_Tab:
    case XKB_KEY_KP_Tab:
      result = LV_KEY_NEXT;
      break;
    case XKB_KEY_ISO_Left_Tab: /* Sent on SHIFT + TAB */
      result = LV_KEY_PREV;
      break;
    default:
      break;
  }

  if (result == 0) {
    char buffer[4] = { 0, 0, 0, 0 };
    int size = xkb_state_key_get_utf8(state->state, keycode, NULL, 0) + 1;
    if (size > 1) {
      xkb_state_key_get_utf8(state->state, keycode, buffer, size);
      memcpy(&result, buffer, 4);
    }
  }

  xkb_state_update_key(state->state, keycode, down ? XKB_KEY_DOWN : XKB_KEY_UP);

  return result;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* USE_XKB */
