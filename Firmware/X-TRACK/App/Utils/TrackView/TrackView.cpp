/*
 * MIT License
 * Copyright (c) 2023 _VIFEXTech
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

#include "TrackView.h"
#include "Utils/PointContainer/PointContainer.h"
#include "Utils/lv_poly_line/lv_poly_line.h"

TrackView::TrackView(lv_obj_t* parent)
    : _cont(nullptr)
    , _trackLine(nullptr)
    , _activeLine(nullptr)
    , _activePoint { 0 }
    , _pointContainer(nullptr)
    , _refLevel(0)
    , _curLevel(0)
    , _pointFilter(onPointFilterEvent, &_lineFilter)
    , _lineFilter(onLineFilterEvent, this)
{
    _cont = lv_obj_create(parent);
    lv_obj_remove_style_all(_cont);
    lv_obj_clear_flag(_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(_cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(_cont, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_add_flag(_cont, LV_OBJ_FLAG_HIDDEN);

    _trackLine = new lv_poly_line(_cont);
    _activeLine = lv_line_create(_cont);
    lv_obj_add_flag(_activeLine, LV_OBJ_FLAG_HIDDEN);

    _pointFilter.setSecondFilterModeEnable(true);
}

TrackView::~TrackView()
{
    delete _trackLine;
    lv_obj_del(_cont);
}

void TrackView::setStyle(lv_style_t* style)
{
    _trackLine->set_style(style);
    lv_obj_add_style(_activeLine, style, 0);
    // lv_obj_set_style_line_color(_activeLine, lv_palette_main(LV_PALETTE_BLUE), 0);
}

void TrackView::setArea(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    lv_obj_set_size(_cont, x1 - x0 + 1, y1 - y0 + 1);

    TrackLineFilter::Area_t area = { x0, y0, x1, y1 };
    _lineFilter.setArea(&area);
}

void TrackView::setActivePoint(int32_t x, int32_t y)
{
    if (!_pointContainer) {
        return;
    }

    lv_point_precise_t end;
    if (!_trackLine->get_end_point(&end)) {
        lv_obj_add_flag(_activeLine, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    const TrackLineFilter::Area_t* area = _lineFilter.getArea();

    TrackLineFilter::Point_t relEndPoint = { area->x0 + (int32_t)end.x, area->y0 + (int32_t)end.y };
    auto relActivePoint = convertPoint(x, y, _refLevel, _curLevel);

    if (!_lineFilter.getIsPointInArea(&relEndPoint)
        && !_lineFilter.getIsPointInArea((TrackLineFilter::Point_t*)&relActivePoint)) {
        lv_obj_add_flag(_activeLine, LV_OBJ_FLAG_HIDDEN);
        LV_LOG_INFO("Point out of area");
        return;
    }

    auto offset = getOffset(relActivePoint.x, relActivePoint.y);
    _activePoint[0].x = end.x;
    _activePoint[0].y = end.y;
    _activePoint[1].x = offset.x;
    _activePoint[1].y = offset.y;

    lv_line_set_points(_activeLine, _activePoint, 2);
    lv_obj_clear_flag(_activeLine, LV_OBJ_FLAG_HIDDEN);
}

void TrackView::pushPoint(int32_t x, int32_t y)
{
    if (!_pointContainer) {
        return;
    }

    auto point = convertPoint(x, y, _refLevel, _curLevel);
    _pointFilter.pushPoint(point.x, point.y);
}

void TrackView::setLevel(int level)
{
    if (level == _curLevel) {
        return;
    }

    _curLevel = level;
}

void TrackView::setPointContainer(PointContainer* pointCont, int refLevel)
{
    _pointContainer = pointCont;
    _refLevel = refLevel;
}

bool TrackView::reload()
{
    if (!_pointContainer) {
        lv_obj_add_flag(_cont, LV_OBJ_FLAG_HIDDEN);
        return false;
    }

    _pointFilter.reset();
    _lineFilter.reset();
    _trackLine->reset();

    int32_t x, y;
    _pointContainer->popStart();
    while (_pointContainer->popPoint(&x, &y)) {
        pushPoint(x, y);
    }

    lv_obj_add_flag(_activeLine, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(_cont, LV_OBJ_FLAG_HIDDEN);
    return true;
}

TrackView::Point_t TrackView::convertPoint(int32_t srcX, int32_t srcY, int srcLevel, int destLevel)
{
    Point_t point;

    int diff = srcLevel - destLevel;

    if (diff == 0) {
        return { srcX, srcY };
    }

    if (diff > 0) {
        point.x = srcX >> diff;
        point.y = srcY >> diff;
    } else {
        point.x = srcX << -diff;
        point.y = srcY << -diff;
    }

    return point;
}

TrackView::Point_t TrackView::getOffset(int32_t x, int32_t y)
{
    const TrackLineFilter::Area_t* area = _lineFilter.getArea();
    return { x - area->x0, y - area->y0 };
}

void TrackView::onPointFilterEvent(TrackPointFilter* filter, const TrackPointFilter::Point_t* point)
{
    auto lineFilter = (TrackLineFilter*)filter->getUserData();
    lineFilter->pushPoint(point->x, point->y);
}

void TrackView::onLineFilterEvent(TrackLineFilter* filter, const TrackLineFilter::Event_t* event)
{
    auto self = (TrackView*)filter->getUserData();

    switch (event->eventID) {
    case TrackLineFilter::EVENT_ID::START_LINE:
        self->_trackLine->start();
        break;
    case TrackLineFilter::EVENT_ID::APPEND_POINT: {
        Point_t offset = self->getOffset(event->point->x, event->point->y);
        self->_trackLine->append(offset.x, offset.y);
    } break;
    default:
        break;
    }
}
