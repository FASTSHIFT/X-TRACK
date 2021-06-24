#ifndef __TILE_CONV_H
#define __TILE_CONV_H

#include <stdint.h>

class TileConv
{
public:
    typedef struct
    {
        int32_t x;
        int32_t y;
    } Point_t;

    typedef struct
    {
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
    } Rect_t;

public:
    TileConv(uint32_t viewWidth = 1, uint32_t viewHeight = 1, uint32_t tileSize = 1);
    ~TileConv() {}

    void SetTileSize(uint32_t size)
    {
        priv.tileSize = size;
    }

    uint32_t GetTileSize()
    {
        return priv.tileSize;
    }

    void SetViewSize(uint32_t width, uint32_t height)
    {
        priv.viewWidth = width;
        priv.viewHeight = height;
    }

    void SetFocusPos(int32_t x, int32_t y);
    void GetFocusOffset(Point_t* offset);

    uint32_t GetTileContainer(Rect_t* rect);
    void GetTileContainerOffset(Point_t* offset);
    void GetTilePos(uint32_t index, Point_t* pos);

    int32_t FixTile(int32_t x, bool up);
    void GetOffset(Point_t* offset, const Point_t* point);

private:
    struct
    {
        uint32_t viewWidth;
        uint32_t viewHeight;
        uint32_t tileSize;

        Point_t pointFocus;
        Point_t pointView[4];
        Point_t pointTileCont[4];
    } priv;
};

#endif
