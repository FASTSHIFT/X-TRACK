#include "lv_multi_line.h"

#ifdef LV_LOG_USER
//#undef LV_LOG_USER
//#define LV_LOG_USER(...)
#endif

lv_multi_line::lv_multi_line(lv_obj_t* par)
    : current_index(0)
    , styleLine(nullptr)
    , parent(par)
{
}

lv_multi_line::~lv_multi_line()
{
    for (auto iter : multi_line)
    {
        if (iter.line)
        {
            lv_obj_del(iter.line);
        }

        decltype(iter.points) vec;
        iter.points.swap(vec);
    }
}

void lv_multi_line::add_line()
{
    lv_obj_t* line = lv_line_create(parent);
    lv_obj_remove_style_all(line);

    if (styleLine != nullptr)
    {
        lv_obj_add_style(line, styleLine, 0);
    }

    single_line_t single_line;
    single_line.line = line;
    multi_line.push_back(single_line);

    LV_LOG_USER("line: %p, parent: %p", line, parent);
}

void lv_multi_line::refresh_line()
{
    size_t size = multi_line.size();

    for (size_t i = 0; i < size; i++)
    {
        single_line_t* single_line = &multi_line[i];
        const lv_point_t* points = get_points(single_line);
        lv_line_set_points(single_line->line, points, (uint16_t)single_line->points.size());
    }
}

void lv_multi_line::start()
{
    if (current_index >= multi_line.size())
    {
        LV_LOG_USER("current_index[%d] >= multi_line.size()[%d]", current_index, multi_line.size());
        add_line();
        refresh_line();
    }

    LV_LOG_USER("show multi_line[%d]", current_index);

    single_line_t* single_line = &multi_line[current_index];

    lv_obj_clear_flag(single_line->line, LV_OBJ_FLAG_HIDDEN);
    lv_line_set_points(single_line->line, nullptr, 0);
}

void lv_multi_line::append(const lv_point_t* point)
{
    if (multi_line.size() == 0)
    {
        LV_LOG_USER("multi_line.size() = 0");
        return;
    }

    LV_LOG_USER("multi_line[%d]: (%d, %d)", current_index, point->x, point->y);
    multi_line[current_index].points.push_back(*point);
}

void lv_multi_line::append_to_end(const lv_point_t* point)
{
    single_line_t* single_line = get_end_line();
    if (single_line == nullptr)
    {
        LV_LOG_ERROR("failed");
        return;
    }

    single_line->points.push_back(*point);

    LV_LOG_USER("end_line: (%d, %d), size = %d", point->x, point->y, single_line->points.size());

    const lv_point_t* points = get_points(single_line);
    lv_line_set_points(single_line->line, points, (uint16_t)single_line->points.size());
}

void lv_multi_line::stop()
{
    if (multi_line.size() == 0)
    {
        LV_LOG_ERROR("multi_line.size() = 0");
        return;
    }

    single_line_t* single_line = &multi_line[current_index];
    const lv_point_t* points = get_points(single_line);
    lv_line_set_points(single_line->line, points, (uint16_t)single_line->points.size());

    LV_LOG_USER("multi_line[%d] size = %d", current_index, single_line->points.size());
    current_index++;
}

void lv_multi_line::reset()
{
    current_index = 0;
    size_t size = multi_line.size();
    LV_LOG_USER("multi_line.size() = %d", size);
    LV_LOG_USER("reset current_index");

    for (size_t i = 0; i < size; i++)
    {
        single_line_t* single_line = &multi_line[i];
        lv_line_set_points(single_line->line, nullptr, 0);
        single_line->points.clear();
        lv_obj_add_flag(single_line->line, LV_OBJ_FLAG_HIDDEN);
    }
}

lv_multi_line::single_line_t* lv_multi_line::get_end_line()
{
    size_t size = multi_line.size();
    if (size == 0)
    {
        LV_LOG_ERROR("NOT found");
        return nullptr;
    }
    else
    {
        LV_LOG_USER("end_line index = %d", current_index - 1);
    }

    return &multi_line[current_index - 1];
}

const lv_point_t* lv_multi_line::get_points(single_line_t* single_line)
{
    const lv_point_t* points =  nullptr;

    if (single_line->points.size())
    {
        points = &single_line->points[0];
    }
    else
    {
        LV_LOG_ERROR("NOT found");
    }

    return points;
}

bool lv_multi_line::get_end_point(lv_point_t* point)
{
    single_line_t* single_line = get_end_line();
    if (single_line == nullptr)
    {
        LV_LOG_ERROR("end line NOT found");
        return false;
    }

    if (single_line->points.size() == 0)
    {
        LV_LOG_ERROR("single_line->points is EMPTY");
        return false;
    }

    *point = single_line->points.back();
    return true;
}
