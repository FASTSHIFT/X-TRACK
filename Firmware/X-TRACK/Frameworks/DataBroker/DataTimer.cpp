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
#include "DataTimer.h"

#ifndef UINT32_MAX
#define UINT32_MAX 0xFFFFFFFF
#endif

DataTimer::DataTimer(DataTimerManager* manager, TimerFunction_t timerFunc, uint32_t period, void* userData)
    : _manager(manager)
    , _timerFunc(timerFunc)
    , _userData(userData)
    , _period(period)
    , _lastTick(0)
    , _isPause(false)
{
    _manager->add(this);
}

DataTimer::~DataTimer()
{
    _manager->remove(this);
}

void DataTimer::pause()
{
    _isPause = true;
}

void DataTimer::resume()
{
    _isPause = false;
}

void DataTimer::reset()
{
    _lastTick = _manager->getTick();
}

uint32_t DataTimer::remain()
{
    auto elaps = _manager->getTickElaps(_lastTick);
    return elaps > _period ? 0 : _period - elaps;
}

void DataTimer::setPeriod(uint32_t period)
{
    _period = period;
}

uint32_t DataTimer::getPeriod()
{
    return _period;
}

void* DataTimer::getUserData()
{
    return _userData;
}

bool DataTimer::isReady()
{
    if (_isPause) {
        return false;
    }

    if (remain() > 0) {
        return false;
    }

    return true;
}

bool DataTimer::isPause()
{
    return _isPause;
}

void DataTimer::invoke()
{
    _timerFunc(this);
}

DataTimerManager::DataTimerManager(TickFunction_t tickCallback)
{
    _getTick = tickCallback;
}

DataTimerManager::~DataTimerManager()
{
}

void DataTimerManager::setTickCallback(TickFunction_t tickCallback)
{
    _getTick = tickCallback;
}

void DataTimerManager::add(DataTimer* timer)
{
    _timerList.push_back(timer);
}

void DataTimerManager::remove(DataTimer* timer)
{
    _timerList.remove(timer);
}

uint32_t DataTimerManager::getTick()
{
    return _getTick();
}

uint32_t DataTimerManager::getTickElaps(uint32_t prevTick)
{
    uint32_t act_time = getTick();

    /*If there is no overflow in sys_time simple subtract*/
    if (act_time >= prevTick) {
        prevTick = act_time - prevTick;
    } else {
        prevTick = UINT32_MAX - prevTick + 1;
        prevTick += act_time;
    }

    return prevTick;
}

uint32_t DataTimerManager::handler()
{
    /* invoke all timers that are ready */
    for (const auto& iter : _timerList) {
        if (iter->isReady()) {
            iter->reset();
            iter->invoke();
        }
    }

    uint32_t timeTillNext = UINT32_MAX;

    /* Find the minimum remaining time */
    for (const auto& iter : _timerList) {
        /* Ignore paused timers */
        if (iter->isPause()) {
            continue;
        }

        uint32_t remain = iter->remain();
        if (remain < timeTillNext) {
            timeTillNext = remain;
        }
    }

    return timeTillNext;
}
