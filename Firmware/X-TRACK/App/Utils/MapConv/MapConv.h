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
#ifndef __MAP_CONV_H
#define __MAP_CONV_H

#include <stddef.h>
#include <stdint.h>

class MapConv {
public:
    typedef struct
    {
        uint32_t tileX;
        uint32_t tileY;
        uint32_t subX;
        uint32_t subY;
    } Tile_t;

    typedef struct
    {
        int32_t x;
        int32_t y;
    } Point_t;

public:
    MapConv(bool coordTransformEnable, int level, int tileSize = 256);
    ~MapConv() { }

    void setCoordTransformEnable(bool enable);
    bool getCoordTransformEnable();
    void setLevel(int level);
    int getLevel();
    Tile_t getTile(double longitude, double latitude);
    int getPath(char* path, size_t len, int32_t x, int32_t y);
    Point_t getCoordinate(double longitude, double latitude);
    Tile_t posToTile(int32_t x, int32_t y);
    Point_t convertPoint(int32_t x, int32_t y, int level);

private:
    double _longitudeOffset;
    double _latitudeOffset;
    double _longitudeLast;
    double _latitudeLast;
    int _level;
    int _tileSize;
    bool _coordTransformEnable;
};

#endif
