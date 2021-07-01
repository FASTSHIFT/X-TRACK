#include "TemplateModel.h"

using namespace Page;

uint32_t TemplateModel::GetData()
{
    return lv_tick_get();
}
