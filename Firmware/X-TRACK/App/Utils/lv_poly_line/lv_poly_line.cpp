#include "lv_poly_line.h"

lv_poly_line::lv_poly_line(lv_obj_t* par)
    : current_index(0)
    , styleLine(nullptr)
    , parent(par)
{
}

lv_poly_line::~lv_poly_line()
{
    for (auto iter : poly_line) {
        if (iter.line) {
            lv_obj_del(iter.line);
        }

        decltype(iter.points) vec;
        iter.points.swap(vec);
    }
}

void lv_poly_line::add_line()
{
    lv_obj_t* line = lv_line_create(parent);
    lv_obj_remove_style_all(line);

    if (styleLine) {
        lv_obj_add_style(line, styleLine, 0);
    }

    single_line_t single_line;
    single_line.line = line;
    poly_line.push_back(single_line);

    LV_LOG_INFO("line: %p", line);
}

void lv_poly_line::start()
{
    if (current_index >= poly_line.size()) {
        add_line();
    }

    LV_LOG_INFO("show poly_line[%" LV_PRId32 "]", current_index);

    single_line_t* single_line = &poly_line[current_index];

    lv_obj_clear_flag(single_line->line, LV_OBJ_FLAG_HIDDEN);
    lv_line_set_points(single_line->line, nullptr, 0);

    // const lv_palette_t palette[] = { LV_PALETTE_RED, LV_PALETTE_GREEN, LV_PALETTE_GREY };
    // lv_obj_set_style_line_color(single_line->line, lv_palette_main(palette[current_index % 3]), 0);
    current_index++;
}

void lv_poly_line::append(const lv_point_precise_t* point)
{
    if (current_index == 0) {
        LV_LOG_WARN("NOT start");
        return;
    }

    LV_LOG_INFO("poly_line[%" LV_PRId32 "]: (%d, %d)", current_index - 1, (int)point->x, (int)point->y);
    single_line_t* single_line = &poly_line[current_index - 1];
    single_line->points.push_back(*point);
    lv_line_set_points(single_line->line, get_points(single_line), single_line->points.size());
}

void lv_poly_line::reset()
{
    current_index = 0;
    size_t size = poly_line.size();
    LV_LOG_INFO("poly_line.size() = %zu", size);
    LV_LOG_INFO("reset current_index");

    for (size_t i = 0; i < size; i++) {
        single_line_t* single_line = &poly_line[i];
        lv_line_set_points(single_line->line, nullptr, 0);
        single_line->points.clear();
        lv_obj_add_flag(single_line->line, LV_OBJ_FLAG_HIDDEN);
    }
}

lv_poly_line::single_line_t* lv_poly_line::get_end_line()
{
    if (current_index == 0) {
        LV_LOG_INFO("NOT found");
        return nullptr;
    }

    LV_LOG_INFO("end_line index = %" LV_PRId32, current_index - 1);

    return &poly_line[current_index - 1];
}

const lv_point_precise_t* lv_poly_line::get_points(single_line_t* single_line)
{
    const lv_point_precise_t* points = nullptr;

    if (single_line->points.size()) {
        points = &single_line->points[0];
    } else {
        LV_LOG_WARN("NOT found");
    }

    return points;
}

bool lv_poly_line::get_end_point(lv_point_precise_t* point)
{
    single_line_t* single_line = get_end_line();
    if (!single_line) {
        LV_LOG_INFO("end line NOT found");
        return false;
    }

    if (single_line->points.size() == 0) {
        LV_LOG_WARN("single_line->points is EMPTY");
        return false;
    }

    *point = single_line->points.back();
    return true;
}
