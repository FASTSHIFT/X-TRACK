/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __TONE_PLAYER_H
#define __TONE_PLAYER_H

#include <stdint.h>

class TonePlayer
{
    typedef void(*CallbackFunction_t)(uint32_t, uint16_t);
public:
    typedef struct
    {
        uint16_t Freq;
        uint16_t Time;
        uint16_t Volume;
    } MusicNode_t;

    enum Speed
    {
        SPEED_HALF = 512,
        SPEED_NORMAL = 256,
        SPEED_DOUBLE = 128
    };

    TonePlayer();
    ~TonePlayer() {}
    void SetMusic(const MusicNode_t* music, uint16_t length);
    void SetCallback(CallbackFunction_t func)
    {
        CallbackFunction = func;
    }
    void Play();
    void Play(const MusicNode_t* music, uint16_t length)
    {
        SetMusic(music, length);
        Play();
    }
    void Stop();
    void SetSpeed(uint16_t speed);
    bool Update(uint32_t tick);
private:
    uint16_t Length;
    uint16_t CurrentPos;
    uint32_t NextTime;
    uint16_t Speed;
    const MusicNode_t* CurrentMusic;
    CallbackFunction_t CallbackFunction;
};

#endif
