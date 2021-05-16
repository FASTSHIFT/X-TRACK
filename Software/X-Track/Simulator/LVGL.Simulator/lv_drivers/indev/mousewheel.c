/**
 * @file mousewheel.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "mousewheel.h"
#if USE_MOUSEWHEEL

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
static int16_t enc_diff = 0;
static lv_indev_state_t state = LV_INDEV_STATE_RELEASED;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the mousewheel
 */
void mousewheel_init(void)
{
    /*Nothing to init*/
}

/**
 * Get encoder (i.e. mouse wheel) ticks difference and pressed state
 * @param indev_drv pointer to the related input device driver
 * @param data store the read data here
 */
void mousewheel_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    (void) indev_drv;      /*Unused*/

    data->state = state;
    data->enc_diff = enc_diff;
    enc_diff = 0;
}

/**
 * It is called periodically from the SDL thread to check mouse wheel state
 * @param event describes the event
 */
void mousewheel_handler(SDL_Event * event)
{
    switch(event->type) {
        case SDL_MOUSEWHEEL:
            // Scroll down (y = -1) means positive encoder turn,
            // so invert it
#ifdef __EMSCRIPTEN__
            /*Escripten scales it wrong*/
            if(event->wheel.y < 0) enc_diff++;
            if(event->wheel.y > 0) enc_diff--;
#else
            enc_diff = -event->wheel.y;
#endif
            break;
        case SDL_MOUSEBUTTONDOWN:
            if(event->button.button == SDL_BUTTON_MIDDLE) {
                state = LV_INDEV_STATE_PRESSED;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if(event->button.button == SDL_BUTTON_MIDDLE) {
                state = LV_INDEV_STATE_RELEASED;
            }
            break;
        default:
            break;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
