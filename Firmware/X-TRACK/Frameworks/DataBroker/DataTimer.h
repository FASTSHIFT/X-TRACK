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
#ifndef __DATA_TIMER_H
#define __DATA_TIMER_H

#include <cstdint>
#include <list>

class DataTimerManager;

class DataTimer {
    friend class DataTimerManager;

public:
    typedef void (*TimerFunction_t)(DataTimer*);

public:
    /**
     * @brief create a timer
     * @param manager the timer manager
     * @param timerFunc the timer function to be invoked
     * @param period the timer period in millisecond
     * @param userData the user data to be passed to the timer function
     */
    DataTimer(DataTimerManager* manager, TimerFunction_t timerFunc, uint32_t period, void* userData = nullptr);

    ~DataTimer();

    /**
     * @brief pause the timer
     */
    void pause();

    /**
     * @brief resume the timer
     */
    void resume();

    /**
     * @brief reset the timer
     */
    void reset();

    /**
     * @brief get the remaining time of the timer
     * @return the remaining time in millisecond
     */
    uint32_t remain();

    /**
     * @brief set the timer period
     * @param period the timer period in millisecond
     */
    void setPeriod(uint32_t period);

    /**
     * @brief get the timer period
     * @return the timer period in millisecond
     */
    uint32_t getPeriod();

    /**
     * @brief get the user data
     * @return the user data
     */
    void* getUserData();

protected:
    /**
     * @brief get the timer is ready to invoke
     * @return true if the timer is ready to invoke, false otherwise
     */
    bool isReady();

    /**
     * @brief get the timer is pause
     * @return true if the timer is pause, false otherwise
     */
    bool isPause();

    /**
     * @brief invoke the timer function
     */
    void invoke();

private:
    DataTimerManager* _manager;
    TimerFunction_t _timerFunc;
    void* _userData;
    uint32_t _period;
    uint32_t _lastTick;
    bool _isPause;
};

class DataTimerManager {
public:
    typedef uint32_t (*TickFunction_t)(void);

public:
    /**
     * @brief create a timer manager
     * @param tickCallback the tick callback function to get the current tick
     */
    DataTimerManager(TickFunction_t tickCallback = nullptr);

    ~DataTimerManager();

    /**
     * @brief set the tick callback function
     * @param tickCallback the tick callback function to get the current tick
     */
    void setTickCallback(TickFunction_t tickCallback);

    /**
     * @brief add a timer
     * @param timer the timer to be added
     */
    void add(DataTimer* timer);

    /**
     * @brief remove a timer
     * @param timer the timer to be removed
     */
    void remove(DataTimer* timer);

    /**
     * @brief get the current tick
     * @return the current tick
     */
    uint32_t getTick();

    /**
     * @brief get the elapsed time from the previous tick
     * @param prevTick the previous tick
     * @return the elapsed time in millisecond
     */
    uint32_t getTickElaps(uint32_t prevTick);

    /**
     * @brief handle the timer events
     * @return time till next timer event in millisecond
     */
    uint32_t handler();

private:
    TickFunction_t _getTick;
    std::list<DataTimer*> _timerList;
};

#endif /* __DATA_TIMER_H */
