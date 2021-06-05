/*
 * MIT License
 * Copyright (c) 2021 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the follo18wing conditions:
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
#include <stdio.h>
#include <math.h>

#ifndef PI
#   define PI 3.1415926535897932384626433832795
#endif

#ifndef constrain
#   define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#endif

MapConv::MapConv()
    : EarthRadius(6378137)
    , LongitudeMin(-180.0)
    , LongitudeMax(180.0)
    , LatitudeMin(-85.05112878)
    , LatitudeMax(85.05112878)
    , MapLevelMax(15)
    , MapLevelMin(3)
    , MapTileSize(256)
    , MapX_Calibration(0)
    , MapY_Calibration(0)
{
    MapLevel = 15;
    MapKeyRootName = "MAP/MapInfos";
    MapKeyFileName = "map.bin";
}

int MapConv::GetMapInfo(
    double longitude, double latitude,
    char* path, uint32_t len,
    uint32_t* mapX, uint32_t* mapY,
    MapTile_t* mapTile
)
{
    uint32_t x, y;
    ConvertMapCoordinate(longitude, latitude, &x, &y);

    *mapX = x;
    *mapY = y;

    ConvertMapTile(x, y, mapTile);
    int ret = ConvertMapPath(x, y, path, len);

    return ret;
}

int MapConv::ConvertMapPath(uint32_t x, uint32_t y, char* path, uint32_t len)
{
    uint32_t tileX = x / MapTileSize;
    uint32_t tileY = y / MapTileSize;
    int ret = snprintf(
        path, len,
        "%s/%lX/%lX/%s", MapKeyRootName, tileX, tileY, MapKeyFileName
    );

    return ret;
}

void MapConv::SetLevel(int level)
{
    MapLevel = constrain(level, MapLevelMin, MapLevelMax);
}

uint32_t MapConv::GetMapSize()
{
    uint32_t seed = MapTileSize;
    return seed << MapLevel;
}

void MapConv::GetMapTile(double longitude, double latitude, MapTile_t* mapTile)
{
    uint32_t x, y;
    ConvertMapCoordinate(longitude, latitude, &x, &y);
    ConvertMapTile(x, y, mapTile);   
}

void MapConv::ConvertMapCoordinate(
    double longitude, double latitude,
    uint32_t* mapX, uint32_t* mapY
)
{
    longitude = constrain(longitude, LongitudeMin, LongitudeMax);
    latitude = constrain(latitude, LatitudeMin, LatitudeMax);

    longitude += MapX_Calibration;
    latitude += MapY_Calibration;

    double x = (longitude + 180.0) / 360.0;
    double sinLatitude = sin(latitude * PI / 180.0);
    double y = 0.5 - log((1.0 + sinLatitude) / (1.0 - sinLatitude)) / (4.0 * PI);

    uint32_t mapSize = GetMapSize();
    double xSize = x * mapSize + 0.5;
    double ySize = y * mapSize + 0.5;
    mapSize--;
    *mapX = (uint32_t)constrain(xSize, 0, mapSize);
    *mapY = (uint32_t)constrain(ySize, 0, mapSize);
}

void MapConv::ConvertMapTile(uint32_t x, uint32_t y, MapTile_t* mapTile)
{
    mapTile->tileX = x / MapTileSize;
    mapTile->tileY = y / MapTileSize;
    mapTile->subX = x % MapTileSize;
    mapTile->subY = y % MapTileSize;
}
