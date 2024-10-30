/**
 * @file easing.c
 * @author uYanki (https://github.com/uYanki)
 * @brief https://github.com/uYanki/board-stm32f103rc-berial/blob/main/7.Example/HAL/19.GUI/03%20u8g2/02%20menu/Lib/easing/easing.c
 * @version 0.1
 * @date 2023-01-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "easing.h"
#include <string.h>

#define PI 3.1415926f

static easing_tick_cb_t g_easing_tick_cb = 0;

#define easing_mills() g_easing_tick_cb()

static const float DH = 1.0f / 22.0f;
static const float D1 = 1.0f / 11.0f;
static const float D2 = 2.0f / 11.0f;
static const float D3 = 3.0f / 11.0f;
// static const float D4 = 4.0f / 11.0f;
static const float D5 = 5.0f / 11.0f;
static const float D7 = 7.0f / 11.0f;
static const float IH = 1.0f / (1.0f / 22.0f);
static const float I1 = 1.0f / (1.0f / 11.0f);
static const float I2 = 1.0f / (2.0f / 11.0f);
static const float I4D = 1.0f / (4.0f / 11.0f) / (4.0f / 11.0f);
// static const float IH  = 1.0f / DH;
// static const float I1  = 1.0f / D1;
// static const float I2  = 1.0f / D2;
// static const float I4D = 1.0f / D4 / D4;

float _easing_calc_InBounce(const float t)
{
    float s;
    if (t < D1) {
        s = t - DH;
        s = DH - s * s * IH;
    } else if (t < D3) {
        s = t - D2;
        s = D1 - s * s * I1;
    } else if (t < D7) {
        s = t - D5;
        s = D2 - s * s * I2;
    } else {
        s = t - 1;
        s = 1 - s * s * I4D;
    }
    return s;
}

float _easing_calc_OutBounce(const float t)
{
    return 1.0f - _easing_calc_InBounce(1.0f - t);
}

float _easing_calc_InOutBounce(const float t)
{
    return (t < 0.5f) ? _easing_calc_InBounce(t * 2.0f) * 0.5f : 1 - _easing_calc_InBounce(2.0f - t * 2.0f) * 0.5f;
}

float _easing_calc_InCirc(const float t)
{
    return 1.0f - sqrtf(1.0f - t * t);
}

float _easing_calc_OutCirc(const float t)
{
    return 1.0f - _easing_calc_InCirc(1.0f - t);
}

float _easing_calc_InOutCirc(const float t)
{
    return (t < 0.5f) ? _easing_calc_InCirc(t * 2.0f) * 0.5f : 1 - _easing_calc_InCirc(2.0f - t * 2.0f) * 0.5f;
}

float _easing_calc_InCubic(const float t)
{
    return t * t * t;
}

float _easing_calc_OutCubic(const float t)
{
    return 1.0f - _easing_calc_InCubic(1.0f - t);
}

float _easing_calc_InOutCubic(const float t)
{
    return (t < 0.5f) ? _easing_calc_InCubic(t * 2.0f) * 0.5f : 1 - _easing_calc_InCubic(2.0f - t * 2.0f) * 0.5f;
}

float _easing_calc_OutElastic(const float t)
{
    float s = 1 - t;
    return 1 - powf(s, 8) + sinf(t * t * 6 * PI) * s * s;
}

float _easing_calc_InElastic(const float t)
{
    return 1.0f - _easing_calc_OutElastic(1.0f - t);
}

float _easing_calc_InOutElastic(const float t)
{
    return (t < 0.5f) ? _easing_calc_InElastic(t * 2.0f) * 0.5f : 1 - _easing_calc_InElastic(2.0f - t * 2.0f) * 0.5f;
}

float _easing_calc_InExpo(const float t)
{
    return powf(2, 10 * (t - 1));
}

float _easing_calc_OutExpo(const float t)
{
    return 1.0f - powf(2, -10 * t);
}

float _easing_calc_InOutExpo(const float t)
{
    return (t < 0.5f) ? _easing_calc_InExpo(t * 2.0f) * 0.5f : 1 - _easing_calc_InExpo(2.0f - t * 2.0f) * 0.5f;
}

float _easing_calc_Linear(const float t)
{
    return t;
}

float _easing_calc_InQuad(const float t)
{
    return t * t;
}

float _easing_calc_OutQuad(const float t)
{
    return 1.0f - _easing_calc_InQuad(1.0f - t);
}

float _easing_calc_InOutQuad(const float t)
{
    return (t < 0.5f) ? _easing_calc_InQuad(t * 2.0f) * 0.5f : 1 - _easing_calc_InQuad(2.0f - t * 2.0f) * 0.5f;
}

float _easing_calc_InQuart(const float t)
{
    return t * t * t * t;
}

float _easing_calc_OutQuart(const float t)
{
    return 1.0f - _easing_calc_InQuart(1.0f - t);
}

float _easing_calc_InOutQuart(const float t)
{
    return (t < 0.5f) ? _easing_calc_InQuart(t * 2.0f) * 0.5f : 1 - _easing_calc_InQuart(2.0f - t * 2.0f) * 0.5f;
}

float _easing_calc_InQuint(const float t)
{
    return t * t * t * t * t;
}

float _easing_calc_OutQuint(const float t)
{
    return 1.0f - _easing_calc_InQuint(1.0f - t);
}

float _easing_calc_InOutQuint(const float t)
{
    return (t < 0.5f) ? _easing_calc_InQuint(t * 2.0f) * 0.5f : 1 - _easing_calc_InQuint(2.0f - t * 2.0f) * 0.5f;
}

float _easing_calc_InSine(const float t)
{
    return 1.0f - cosf(t * (PI / 2));
}

float _easing_calc_OutSine(const float t)
{
    return 1.0f - _easing_calc_InSine(1.0f - t);
}

float _easing_calc_InOutSine(const float t)
{
    return (t < 0.5f) ? _easing_calc_InSine(t * 2.0f) * 0.5f : 1 - _easing_calc_InSine(2.0f - t * 2.0f) * 0.5f;
}

float _easing_calc_InBack(const float t)
{
    return 3 * t * t * t - 2 * t * t;
}

float _easing_calc_OutBack(const float t)
{
    return 1.0f - _easing_calc_InBack(1.0f - t);
}

float _easing_calc_InOutBack(const float t)
{
    return (t < 0.5f) ? _easing_calc_InBack(t * 2.0f) * 0.5f : 1 - _easing_calc_InBack(2.0f - t * 2.0f) * 0.5f;
}

////////////////////////////////////////////////////////////////////////////////////

void easing_set_tick_callback(easing_tick_cb_t tick_cb)
{
    g_easing_tick_cb = tick_cb;
}

void easing_init(
    easing_t* pEasing,
    easing_mode_t dwMode,
    easing_calc_t lpfnCalc,
    easing_pos_t nOffset,
    uint16_t nFrameCount,
    uint16_t nInterval)
{
    memset(pEasing, 0, sizeof(easing_t));
    pEasing->dwMode = dwMode;
    pEasing->lpfnCalc = lpfnCalc == 0 ? _easing_calc_Linear : lpfnCalc;
    pEasing->nOffset = nOffset;
    pEasing->nFrameCount = (nFrameCount < 2) ? 2 : nFrameCount;
    pEasing->nInterval = nInterval;
    pEasing->bDirection = dwMode & EASING_DIR_REVERSE;
}

void easing_start_absolute(
    easing_t* pEasing,
    easing_pos_t nStart,
    easing_pos_t nStop)
{
    pEasing->nStart = nStart;
    pEasing->nStop = nStop;
    pEasing->nDelta = nStop - nStart;

    pEasing->nFrameIndex = 0; // first frame is nStart
    pEasing->fProgress = 0.0f;

    pEasing->bDirection = pEasing->dwMode & EASING_DIR_REVERSE;

    if (pEasing->dwMode & EASING_TIMES_INFINITE) {
        pEasing->nTimes = -1;
    } else {
        pEasing->nTimes = (pEasing->dwMode & EASING_TIMES_MANYTIMES) ? (pEasing->dwMode >> EASING_TIMES_SET) : 1;
        if (pEasing->dwMode & EASING_DIR_BACKANDFORTH)
            pEasing->nTimes *= 2;
    }

#ifdef easing_mills
    pEasing->nMills = easing_mills();
#endif
}

void easing_start_relative(
    easing_t* pEasing,
    easing_pos_t nDistance)
{
    easing_start_absolute(
        pEasing,
#if 1
        pEasing->nCurrent, // from current pos
#else
        easing->nStop, // from stop pos
#endif
        pEasing->nStop + nDistance);
}

void easing_update(easing_t* pEasing)
{
    // isok
    if (pEasing->nTimes == 0)
        return;

#ifdef easing_mills
    if (pEasing->nInterval > 0) {
        if (easing_mills() < pEasing->nMills)
            return;
        pEasing->nMills = easing_mills() + pEasing->nInterval;
    }
#endif

    // next frame
    ++pEasing->nFrameIndex;

    if (pEasing->nFrameIndex > pEasing->nFrameCount) {
        if (pEasing->dwMode & EASING_DIR_BACKANDFORTH) {
            // reverse direction
            pEasing->bDirection = !pEasing->bDirection;
            // skip once nStart/nStop pos
            pEasing->nFrameIndex = 2;
        } else {
            // at first frame
            pEasing->nFrameIndex = 1;
        }
    }

    if (pEasing->nFrameIndex == pEasing->nFrameCount) {
        // at last frame
        pEasing->fProgress = 1.0f;
        pEasing->nCurrent = pEasing->bDirection ? pEasing->nStart : pEasing->nStop;
        // decrease times
        if (!(pEasing->dwMode & EASING_TIMES_INFINITE))
            if (--pEasing->nTimes)
                return;
    } else {
        // calculate progress
        pEasing->fProgress = (float)(pEasing->nFrameIndex - 1) / (pEasing->nFrameCount - 1);
        // calculate position
        pEasing->nCurrent = pEasing->bDirection ? (pEasing->nStop - pEasing->nDelta * pEasing->lpfnCalc(pEasing->fProgress)) : (pEasing->nStart + pEasing->nDelta * pEasing->lpfnCalc(pEasing->fProgress));
    }
}

bool easing_isok(easing_t* pEasing)
{
    return pEasing->nTimes == 0;
}

void easing_stop(easing_t* pEasing, easing_pos_t nCurrent)
{
    pEasing->nTimes = 0;
    pEasing->nCurrent = nCurrent;
}

easing_pos_t easing_curpos(easing_t* pEasing)
{
    return pEasing->nCurrent + pEasing->nOffset;
}
