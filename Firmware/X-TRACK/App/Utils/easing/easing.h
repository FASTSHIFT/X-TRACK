/**
 * @file easing.h
 * @author uYanki (https://github.com/uYanki)
 * @brief https://github.com/uYanki/board-stm32f103rc-berial/blob/main/7.Example/HAL/19.GUI/03%20u8g2/02%20menu/Lib/easing/easing.h
 * @version 0.1
 * @date 2023-01-15
 *
 * @copyright Copyright (c) 2023
 *
 */

// https://blog.csdn.net/z2014z/article/details/120691794

#ifndef __EASING_H__
#define __EASING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

////////////////

typedef float (*easing_calc_t)(const float t);
typedef uint32_t (*easing_tick_cb_t)(void);

typedef float easing_pos_t; // data type

typedef enum {
    EASING_MODE_BITCNT = 4,
    EASING_MODE_MASK = (1 << EASING_MODE_BITCNT) - 1,

    EASING_TIMES_SINGLE = 0 << 0, // 单次(default)
    EASING_TIMES_MANYTIMES = 1 << 0, // 多次
    EASING_TIMES_INFINITE = 1 << 1, // 循环

    EASING_TIMES_SET = EASING_MODE_BITCNT,

    EASING_DIR_FORWARD = 0 << 0, // 正向(default)
    EASING_DIR_REVERSE = 1 << 2, // 反向
    EASING_DIR_BACKANDFORTH = 1 << 3, // 往返

} easing_mode_t;

#define EASING_MODE_DEFAULT ((easing_mode_t)(EASING_TIMES_SINGLE | EASING_DIR_FORWARD))
#define EASING_MODE_NTIMES(n) EASING_TIMES_MANYTIMES | (n << EASING_TIMES_SET)

#define EASING_INTERVAL_NONE 0

typedef struct easing {
    easing_mode_t dwMode;

    // type
    easing_calc_t lpfnCalc; // _easing_calc_xxx

    // position
    easing_pos_t nStart;
    easing_pos_t nStop;
    easing_pos_t nOffset;
    easing_pos_t nDelta;
    easing_pos_t nCurrent; // range: [nStart, nStop]

    // progress
    uint16_t nFrameCount; // range: [2,n], 1: nStart/nStop; n:nStop/nStart;
    uint16_t nFrameIndex; // current frame. range: [0, nFrameCount], 0:nStart/nStop; nFrameCount:nStop/nStart;
    float fProgress; // current progress. range: [0,1]

    int16_t nTimes;
    bool bDirection; // true: reverse, false：forward

    uint32_t nMills;
    uint16_t nInterval; // minimum time interval per frame (ms)
} easing_t;

//////////////// _easing_calc_xxx

float _easing_calc_Linear(const float t); // linear t

float _easing_calc_InQuad(const float t); // quadratic t^2
float _easing_calc_OutQuad(const float t);
float _easing_calc_InOutQuad(const float t);

float _easing_calc_InCubic(const float t); // cubic t^3
float _easing_calc_OutCubic(const float t);
float _easing_calc_InOutCubic(const float t);

float _easing_calc_InQuart(const float t); // quartic t^4
float _easing_calc_OutQuart(const float t);
float _easing_calc_InOutQuart(const float t);

float _easing_calc_InQuint(const float t); // quintic t^5
float _easing_calc_OutQuint(const float t);
float _easing_calc_InOutQuint(const float t);

float _easing_calc_InSine(const float t); // sinusoidal 正弦 sin(t)
float _easing_calc_OutSine(const float t);
float _easing_calc_InOutSine(const float t);

float _easing_calc_InExpo(const float t); // exponential 指数 2^t
float _easing_calc_OutExpo(const float t);
float _easing_calc_InOutExpo(const float t);

float _easing_calc_InCirc(const float t); // circular 圆形
float _easing_calc_OutCirc(const float t);
float _easing_calc_InOutCirc(const float t);

float _easing_calc_InBack(const float t); // elastic 衰减三次幂 (s+1)t^3 - st^2
float _easing_calc_OutBack(const float t);
float _easing_calc_InOutBack(const float t);

float _easing_calc_InElastic(const float t); // elastic 衰减正弦
float _easing_calc_OutElastic(const float t);
float _easing_calc_InOutElastic(const float t);

float _easing_calc_InBounce(const float t); // back 衰减反弹
float _easing_calc_OutBounce(const float t);
float _easing_calc_InOutBounce(const float t);

////////////////

void easing_set_tick_callback(easing_tick_cb_t tick_cb);

void easing_init(
    easing_t* pEasing,
    easing_mode_t dwMode, // default: EASING_TIMES_SINGLE | EASING_DIR_FORWARD
    easing_calc_t lpfnCalc, // default: _easing_calc_Linear
    easing_pos_t nOffset, // default: 0
    uint16_t nFrameCount, // default: 2
    uint16_t nInterval // default: EASING_INTERVAL_NONE
);

void easing_start_absolute(
    easing_t* pEasing,
    easing_pos_t nStart,
    easing_pos_t nStop);

void easing_start_relative(
    easing_t* pEasing,
    easing_pos_t nDistance);

// update easing position
void easing_update(easing_t* easing);

bool easing_isok(easing_t* pEasing);
void easing_stop(easing_t* pEasing, easing_pos_t nCurrent);

easing_pos_t easing_curpos(easing_t* pEasing);

#ifdef __cplusplus
}
#endif

#endif