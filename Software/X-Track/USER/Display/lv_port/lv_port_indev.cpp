/**
 * @file lv_port_indev_templ.c
 *
 */

 /*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "Arduino.h"
#include "Config/Config.h"
#include "HAL/HAL.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_indev_t * encoder_indev;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;

    /*------------------
     * Encoder
     * -----------------*/

    /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    encoder_indev = lv_indev_drv_register(&indev_drv);

    /* Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     * add objects to the group with `lv_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `lv_indev_set_group(indev_encoder, group);` */
}

lv_indev_t * lv_port_indev_get(void)
{
    return encoder_indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Encoder
 * -----------------*/

/* Initialize your keypad */
static void encoder_init(void)
{
    /*Your code comes here*/
}

/* Will be called by the library to read the encoder */
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static bool lastState;
    data->enc_diff = HAL::Encoder_GetDiff();
    
    bool isPush = HAL::Encoder_GetIsPush();
    
    data->state = isPush ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    
    if(isPush != lastState)
    {
        HAL::Buzz_Tone(isPush ? 500 : 700, 20);
        lastState = isPush;
    }
}

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
