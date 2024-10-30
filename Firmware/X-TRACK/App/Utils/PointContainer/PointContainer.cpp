/*
 * MIT License
 * Copyright (c) 2021 - 2023 _VIFEXTech
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
#include "PointContainer.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

PointContainer::PointContainer()
    : _curPushPoint { 0 }
    , _curPopPoint { 0 }
    , _curPopIndex(0)
{
}

PointContainer::~PointContainer()
{
}

bool PointContainer::isFlag(const DiffPoint_t* point)
{
    if (point->x == FLAG_END_POINT && point->y == FLAG_END_POINT) {
        return true;
    }

    if (point->x == FLAG_FULL_POINT && point->y == FLAG_FULL_POINT) {
        return true;
    }

    return false;
}

void PointContainer::pushPoint(const FullPoint_t* point)
{
    size_t curIndex = vecPoints.size();
    if (curIndex == 0) {
        _curPushPoint = *point;
        pushFullPoint(point);
        return;
    }

    int32_t diffX = point->x - _curPushPoint.x;
    int32_t diffY = point->y - _curPushPoint.y;
    _curPushPoint = *point;

    if (std::abs((int)diffX) > SCHAR_MAX || std::abs((int)diffY) > SCHAR_MAX) {
        pushFullPoint(point);
        return;
    }

    const DiffPoint_t diffPoint = { (int8_t)diffX, (int8_t)diffY };
    if (isFlag(&diffPoint)) {
        pushFullPoint(point);
        return;
    }

    vecPoints.push_back(diffPoint);
}

void PointContainer::pushFullPoint(const FullPoint_t* point)
{
    vecPoints.push_back(makeFlag(FLAG_FULL_POINT));
    const DiffPoint_t* pData = (const DiffPoint_t*)point;
    for (size_t i = 0; i < sizeof(FullPoint_t) / sizeof(DiffPoint_t); i++) {
        vecPoints.push_back(pData[i]);
    }
}

bool PointContainer::popFullPoint(FullPoint_t* point)
{
    size_t size = vecPoints.size();
    if (size - _curPopIndex < 4) {
        return false;
    }

    DiffPoint_t* pData = (DiffPoint_t*)point;
    for (size_t i = 0; i < sizeof(FullPoint_t) / sizeof(DiffPoint_t); i++) {
        pData[i] = vecPoints[_curPopIndex];
        _curPopIndex++;
    }
    return true;
}

bool PointContainer::popPoint(FullPoint_t* point)
{
    size_t size = vecPoints.size();

    if (size - _curPopIndex == 0) {
        return false;
    }

    DiffPoint_t diffPoint = vecPoints[_curPopIndex];

    if (diffPoint.x == FLAG_FULL_POINT && diffPoint.y == FLAG_FULL_POINT) {
        _curPopIndex++;
        if (popFullPoint(point)) {
            _curPopPoint = *point;
        } else {
            return false;
        }
    } else {
        _curPopPoint.x += diffPoint.x;
        _curPopPoint.y += diffPoint.y;
        *point = _curPopPoint;
        _curPopIndex++;
    }

    return true;
}

void PointContainer::popStart()
{
    _curPopIndex = 0;
}
