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
#ifndef __TILE_VIEW_H
#define __TILE_VIEW_H

#include <stdint.h>

class TileView {
public:
    typedef struct Point {
        Point(int32_t _x = 0, int32_t _y = 0)
            : x(_x)
            , y(_y)
        {
        }
        int32_t x;
        int32_t y;
    } Point_t;

    typedef struct Rect {
        Rect(int32_t _x = 0, int32_t _y = 0, int32_t _width = 0, int32_t _height = 0)
            : x(_x)
            , y(_y)
            , width(_width)
            , height(_height)
        {
        }
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
    } Rect_t;

public:
    TileView(uint32_t viewWidth = 240, uint32_t viewHeight = 240, uint32_t tileSize = 256);
    ~TileView() { }

    void setViewSize(uint32_t width, uint32_t height);
    int32_t getViewWidth()
    {
        return _viewWidth;
    }
    int32_t getViewHeight()
    {
        return _viewHeight;
    }

    void setFocusPos(int32_t x, int32_t y);
    Point_t getFocusPos();
    Point_t getFocusOffset();

    void setTileSize(uint32_t size);
    uint32_t getTileSize();
    Rect_t getTileRect();
    Point_t getTileRectOffset();
    Point_t getTilePos(uint32_t index);
    uint32_t getTileNum();

    int32_t roundTile(int32_t x, bool up);
    Point_t getOffset(const Point_t* point);

private:
    uint32_t _viewWidth;
    uint32_t _viewHeight;
    uint32_t _tileSize;

    Point_t _pointFocus;
    Point_t _pointView[4];
    Point_t _pointTileCont[4];
};

#endif /* __TILE_VIEW_H */
