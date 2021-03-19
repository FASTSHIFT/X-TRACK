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
#include "Basic/SysConfig.h"
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
static bool encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void encoder_handler(void);
static void buzz_handler(int dir);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_indev_t * encoder_indev;
static volatile int32_t encoder_diff;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    lv_indev_drv_t indev_drv;

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
    pinMode(ENCODER_A_PIN, INPUT_PULLUP);
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);
    pinMode(ENCODER_PUSH_PIN, INPUT_PULLUP);
    
    attachInterrupt(ENCODER_A_PIN, encoder_handler, FALLING);
}

/* Will be called by the library to read the encoder */
static bool encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    data->enc_diff = encoder_diff;
    
    bool is_pressed = (digitalRead(ENCODER_PUSH_PIN) == LOW);
    
    data->state = is_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    encoder_diff = 0;
    
    static bool last_state;
    
    if(is_pressed != last_state)
    {
        HAL::Buzz_Tone(is_pressed ? 500 : 700, 20);
        last_state = is_pressed;
    }
    
    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}

static void buzz_handler(int dir)
{
    static const uint16_t freqStart = 2000;
    static uint16_t freq = freqStart;
    static uint32_t lastRotateTime;

    if(lv_tick_elaps(lastRotateTime) > 1000)
    {
        freq = freqStart;
    }
    else
    {
        if(dir > 0)
        {
            freq += 100;
        }
        
        if(dir < 0)
        {
            freq -= 100;
        }
        
        freq = constrain(freq, 100, 20 * 1000);
    }

    lastRotateTime = lv_tick_get();
    HAL::Buzz_Tone(freq, 5);
}

/*Call this function in an interrupt to process encoder events (turn, press)*/
static void encoder_handler(void)
{
    /*Your code comes here*/
    int dir = (digitalRead(ENCODER_B_PIN) == LOW ? +1 : -1);
    encoder_diff += dir;
    buzz_handler(dir);
}

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
