#include "TileConv.h"
#include <string.h>

TileConv::TileConv(uint32_t viewWidth, uint32_t viewHeight, uint32_t tileSize)
{
    memset(&priv, 0, sizeof(priv));
    priv.viewWidth = viewWidth;
    priv.viewHeight = viewHeight;
    priv.tileSize = tileSize;
}

void TileConv::SetFocusPos(int32_t x, int32_t y)
{
    priv.pointFocus.x = x;
    priv.pointFocus.y = y;

    const int32_t viewHalfWidth = priv.viewWidth / 2;
    const int32_t viewHalfHeight = priv.viewHeight / 2;
    const uint32_t tileSize = priv.tileSize;

    priv.pointView[0].x = x - viewHalfWidth;
    priv.pointView[0].y = y - viewHalfHeight;
    priv.pointView[1].x = x + viewHalfWidth;
    priv.pointView[1].y = y - viewHalfHeight;
    priv.pointView[2].x = x - viewHalfWidth;
    priv.pointView[2].y = y + viewHalfHeight;
    priv.pointView[3].x = x + viewHalfWidth;
    priv.pointView[3].y = y + viewHalfHeight;

    const int32_t tileContWidth = (priv.viewWidth / tileSize + 2) * tileSize;
    const int32_t tileContHeight = (priv.viewHeight / tileSize + 2) * tileSize;

    priv.pointTileCont[0].x = FixTile(priv.pointView[0].x, false);
    priv.pointTileCont[0].y = FixTile(priv.pointView[0].y, false);
    priv.pointTileCont[1].x = priv.pointTileCont[0].x + tileContWidth;
    priv.pointTileCont[1].y = priv.pointTileCont[0].y;
    priv.pointTileCont[2].x = priv.pointTileCont[0].x;
    priv.pointTileCont[2].y = priv.pointTileCont[0].y + tileContHeight;
    priv.pointTileCont[3].x = priv.pointTileCont[0].x + tileContWidth;
    priv.pointTileCont[3].y = priv.pointTileCont[0].y + tileContHeight;
}

uint32_t TileConv::GetTileContainer(Rect_t* rect)
{
    rect->x = priv.pointTileCont[0].x;
    rect->y = priv.pointTileCont[0].y;
    rect->width = priv.pointTileCont[1].x - priv.pointTileCont[0].x;
    rect->height = priv.pointTileCont[2].y - priv.pointTileCont[0].y;

    uint32_t size = (rect->width / priv.tileSize) * (rect->height / priv.tileSize);
    return size;
}

void TileConv::GetTileContainerOffset(Point_t* offset)
{
    GetOffset(offset, &priv.pointView[0]);
}

void TileConv::GetTilePos(uint32_t index, Point_t* pos)
{
    int32_t width = priv.pointTileCont[1].x - priv.pointTileCont[0].x;
    int32_t widthIndexMax = width / priv.tileSize;
    pos->x = priv.pointTileCont[0].x + priv.tileSize * (index % widthIndexMax);
    pos->y = priv.pointTileCont[0].y + priv.tileSize * (index / widthIndexMax);
}

void TileConv::GetFocusOffset(Point_t* offset)
{
    GetOffset(offset, &priv.pointFocus);
}

int32_t TileConv::FixTile(int32_t x, bool up)
{
    int32_t r = x % priv.tileSize;

    if (r == 0)
    {
        return x;
    }

    int32_t ret = x - r;

    if (up)
    {
        ret += priv.tileSize;
    }

    return ret;
}

void TileConv::GetOffset(Point_t* offset, const Point_t* point)
{
    offset->x = point->x - priv.pointTileCont[0].x;
    offset->y = point->y - priv.pointTileCont[0].y;
}
