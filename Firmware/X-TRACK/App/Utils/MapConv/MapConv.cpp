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
#include "MapConv.h"
#include "TileSystem.h"
#include <cmath>
#include <cstdio>

#define LONGITUDE_OFFSET 0.008
#define LATITUDE_OFFSET 0.009

using namespace ::Microsoft_MapPoint;

MapConv::MapConv(bool coordTransformEnable, int level, int tileSize)
    : _longitudeOffset(0)
    , _latitudeOffset(0)
    , _longitudeLast(0)
    , _latitudeLast(0)
    , _level(level)
    , _tileSize(tileSize)
    , _coordTransformEnable(coordTransformEnable)
{
}

void MapConv::setCoordTransformEnable(bool enable)
{
    _coordTransformEnable = enable;
}

bool MapConv::getCoordTransformEnable()
{
    return _coordTransformEnable;
}

void MapConv::setLevel(int level)
{
    _level = level;
}

int MapConv::getLevel()
{
    return _level;
}

MapConv::Tile_t MapConv::getTile(double longitude, double latitude)
{
    Point_t point = getCoordinate(longitude, latitude);
    return posToTile(point.x, point.y);
}

MapConv::Point_t MapConv::getCoordinate(double longitude, double latitude)
{
    int pixelX, pixelY;

    TileSystem::LatLongToPixelXY(
        latitude,
        longitude,
        _level,
        &pixelX,
        &pixelY);

    Point_t point;
    point.x = pixelX;
    point.y = pixelY;
    return point;
};

int MapConv::getPath(char* path, size_t len, int32_t x, int32_t y)
{
    int tileX = x / _tileSize;
    int tileY = y / _tileSize;
    int ret = snprintf(path, len, "/%d/%d/%d", _level, tileX, tileY);
    return ret;
}

MapConv::Tile_t MapConv::posToTile(int32_t x, int32_t y)
{
    Tile_t mapTile;
    mapTile.tileX = x / _tileSize;
    mapTile.tileY = y / _tileSize;
    mapTile.subX = x % _tileSize;
    mapTile.subY = y % _tileSize;
    return mapTile;
}

MapConv::Point_t MapConv::convertPoint(int32_t x, int32_t y, int level)
{
    Point_t point;

    int diff = level - _level;

    if (diff >= 0) {
        point.x = x >> diff;
        point.y = y >> diff;
    } else {
        point.x = x << -diff;
        point.y = y << -diff;
    }

    return point;
}
