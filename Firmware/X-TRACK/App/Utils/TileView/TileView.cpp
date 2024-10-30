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
#include "TileView.h"
#include <string.h>

TileView::TileView(uint32_t viewWidth, uint32_t viewHeight, uint32_t tileSize)
    : _viewWidth(viewWidth)
    , _viewHeight(viewHeight)
    , _tileSize(tileSize)
{
}

void TileView::setViewSize(uint32_t width, uint32_t height)
{
    _viewWidth = width;
    _viewHeight = height;
}

void TileView::setFocusPos(int32_t x, int32_t y)
{
    _pointFocus.x = x;
    _pointFocus.y = y;

    const int32_t viewHalfWidth = _viewWidth / 2;
    const int32_t viewHalfHeight = _viewHeight / 2;
    const uint32_t tileSize = _tileSize;

    _pointView[0].x = x - viewHalfWidth;
    _pointView[0].y = y - viewHalfHeight;
    _pointView[1].x = x + viewHalfWidth;
    _pointView[1].y = y - viewHalfHeight;
    _pointView[2].x = x - viewHalfWidth;
    _pointView[2].y = y + viewHalfHeight;
    _pointView[3].x = x + viewHalfWidth;
    _pointView[3].y = y + viewHalfHeight;

    const int32_t tileContWidth = (_viewWidth / tileSize + 2) * tileSize;
    const int32_t tileContHeight = (_viewHeight / tileSize + 2) * tileSize;

    _pointTileCont[0].x = roundTile(_pointView[0].x, false);
    _pointTileCont[0].y = roundTile(_pointView[0].y, false);
    _pointTileCont[1].x = _pointTileCont[0].x + tileContWidth;
    _pointTileCont[1].y = _pointTileCont[0].y;
    _pointTileCont[2].x = _pointTileCont[0].x;
    _pointTileCont[2].y = _pointTileCont[0].y + tileContHeight;
    _pointTileCont[3].x = _pointTileCont[0].x + tileContWidth;
    _pointTileCont[3].y = _pointTileCont[0].y + tileContHeight;
}

TileView::Point_t TileView::getFocusPos()
{
    return _pointFocus;
}

TileView::Point_t TileView::getFocusOffset()
{
    return getOffset(&_pointFocus);
}

void TileView::setTileSize(uint32_t size)
{
    _tileSize = size;
}

uint32_t TileView::getTileSize()
{
    return _tileSize;
}

TileView::Rect_t TileView::getTileRect()
{
    Rect_t rect;
    rect.x = _pointTileCont[0].x;
    rect.y = _pointTileCont[0].y;
    rect.width = _pointTileCont[1].x - _pointTileCont[0].x;
    rect.height = _pointTileCont[2].y - _pointTileCont[0].y;
    return rect;
}

TileView::Point_t TileView::getTileRectOffset()
{
    return getOffset(&_pointView[0]);
}

TileView::Point_t TileView::getTilePos(uint32_t index)
{
    Point_t pos;
    int32_t width = _pointTileCont[1].x - _pointTileCont[0].x;
    int32_t widthIndexMax = width / _tileSize;
    pos.x = _pointTileCont[0].x + _tileSize * (index % widthIndexMax);
    pos.y = _pointTileCont[0].y + _tileSize * (index / widthIndexMax);
    return pos;
}

uint32_t TileView::getTileNum()
{
    Rect_t rect = getTileRect();
    return (rect.width / _tileSize) * (rect.height / _tileSize);
}

int32_t TileView::roundTile(int32_t x, bool up)
{
    int32_t r = x % _tileSize;

    if (r == 0) {
        return x;
    }

    int32_t ret = x - r;

    if (up) {
        ret += _tileSize;
    }

    return ret;
}

TileView::Point_t TileView::getOffset(const Point_t* point)
{
    Point_t offset;
    offset.x = point->x - _pointTileCont[0].x;
    offset.y = point->y - _pointTileCont[0].y;
    return offset;
}
