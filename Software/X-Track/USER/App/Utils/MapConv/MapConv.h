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

#include <stdint.h>
#include "../TileSystem/TileSystem.h"

class MapConv : public Microsoft_MapPoint::TileSystem
{
public:
    MapConv();
    ~MapConv() {}

    typedef struct
    {
        uint32_t tileX;
        uint32_t tileY;
        uint32_t subX;
        uint32_t subY;
    } MapTile_t;

    void SetLevel(int level);
    void SetLevelUp()
    {
        SetLevel(GetLevel() + 1);
    }
    void SetLevelDown()
    {
        SetLevel(GetLevel() - 1);
    }
    void SetMapFilePath(const char* path)
    {
        MapFilePath = path;
    }
    void SetFileName(const char* fileName)
    {
        MapKeyFileName = fileName;
    }
    void SetMapCalibration(double lng, double lat)
    {
        MapLng_Calibration = lng;
        MapLat_Calibration = lat;
    }
    int GetMapInfo(
        double longitude, double latitude,
        char* path, uint32_t len,
        uint32_t* mapX, uint32_t* mapY,
        MapTile_t* mapTile
    );

    int GetLevel()
    {
        return MapLevel;
    }
    uint16_t GetMapTileSize()
    {
        return MapTileSize;
    }

    int ConvertMapPath(uint32_t x, uint32_t y, char* path, uint32_t len);
    void ConvertMapCoordinate(
        double longitude, double latitude,
        uint32_t* mapX, uint32_t* mapY
    );
    void ConvertPosToTile(uint32_t x, uint32_t y, MapTile_t* mapTile);
    void GetMapTile(double longitude, double latitude, MapTile_t* mapTile);

private:
    const int MapLevelMax;
    const int MapLevelMin;
    const uint16_t MapTileSize;
    double MapLng_Calibration;
    double MapLat_Calibration;

    int MapLevel;
    const char* MapFilePath;
    const char* MapKeyFileName;

    uint32_t GetMapSize();
};

#endif
