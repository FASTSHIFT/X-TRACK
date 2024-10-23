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
#include "TonePlayer.h"

TonePlayer::TonePlayer()
{
    CallbackFunction = nullptr;
    CurrentMusic = nullptr;
    Speed = SPEED_NORMAL;
}

void TonePlayer::SetMusic(const MusicNode_t* music, uint16_t length)
{
    if (music == CurrentMusic)
    {
        return;
    }

    CurrentMusic = music;
    Length = length;
    CurrentPos = length;
}

void TonePlayer::SetSpeed(uint16_t speed)
{
    Speed = speed;
}

void TonePlayer::Play()
{
    CurrentPos = 0;
}

void TonePlayer::Stop()
{
    CurrentPos = Length;
}

bool TonePlayer::Update(uint32_t tick)
{
    if (CallbackFunction == nullptr)
    {
        return false;
    }

    if(CurrentPos < Length)
    {
        if(tick > NextTime)
        {
            CallbackFunction(CurrentMusic[CurrentPos].Freq, CurrentMusic[CurrentPos].Volume);

            NextTime = tick + CurrentMusic[CurrentPos].Time * Speed / SPEED_NORMAL;
            CurrentPos++;
        }
        return true;
    }
    else if(CurrentPos == Length && tick > NextTime)
    {
        CallbackFunction(0, 0);
        CurrentPos++;
    }

    return false;
}
