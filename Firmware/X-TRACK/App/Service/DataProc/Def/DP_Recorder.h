/*
 * MIT License
 * Copyright (c) 2021 - 2024 _VIFEXTech
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
#ifndef __DATA_PROC_RECORDER_DEF_H
#define __DATA_PROC_RECORDER_DEF_H

#include <cstdint>

namespace DataProc {

enum class RECORDER_CMD {
    NONE,
    ENABLE_AUTO_REC,
    DISABLE_AUTO_REC,
};

typedef struct Recorder_Info {
    Recorder_Info()
        : active(false)
        , cmd(RECORDER_CMD::NONE)
        , time(0)
    {
    }
    bool active;
    bool autoRec;
    RECORDER_CMD cmd;
    uint16_t time;
} Recorder_Info_t;

} // namespace DataProc

#endif // __DATA_PROC_RECORDER_DEF_H
