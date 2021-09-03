#ifndef LV_MULTI_LINE_H
#define LV_MULTI_LINE_H

#include "lvgl/lvgl.h"
#include <vector>
#include "Utils/lv_allocator/lv_allocator.h"

class lv_multi_line
{
public:
    lv_multi_line(lv_obj_t* par);
    ~lv_multi_line();

    void set_style(lv_style_t* style)
    {
        styleLine = style;
    }

    void start();
    void append(const lv_point_t* point);
    void append(lv_coord_t x, lv_coord_t y)
    {
        lv_point_t point = { x, y };
        append(&point);
    }
    void append_to_end(const lv_point_t* point);
    void append_to_end(lv_coord_t x, lv_coord_t y)
    {
        lv_point_t point = { x, y };
        append_to_end(&point);
    }
    void stop();
    void reset();
    bool get_end_point(lv_point_t* point);

private:
    typedef struct
    {
        lv_obj_t* line;
        std::vector<lv_point_t, lv_allocator<lv_point_t>> points;
    } single_line_t;

private:
    void add_line();
    void refresh_line();
    single_line_t* get_end_line();
    const lv_point_t* get_points(single_line_t* single_line);

private:
    std::vector<single_line_t, lv_allocator<single_line_t>> multi_line;
    uint32_t current_index;
    lv_style_t* styleLine;
    lv_obj_t* parent;
};

#endif // !LV_MULTI_LINE_H
