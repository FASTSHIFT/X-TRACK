#include "lv_monkey.h"
#include <stdlib.h>

typedef struct _lv_monkey{
    lv_indev_drv_t indev_drv;
    lv_indev_data_t indev_data;
    lv_timer_t * timer;
    uint32_t timer_interval_min;
    uint32_t timer_interval_max;
    lv_group_t * group;
}lv_monkey_t;

static const lv_key_t lv_key_map[] = {
    LV_KEY_UP,
    LV_KEY_DOWN,
    LV_KEY_RIGHT,
    LV_KEY_LEFT,
    LV_KEY_ESC,
    LV_KEY_DEL,
    LV_KEY_BACKSPACE,
    LV_KEY_ENTER,
    LV_KEY_NEXT,
    LV_KEY_PREV,
    LV_KEY_HOME,
    LV_KEY_END,
};

static void lv_monkey_read_cb(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    lv_monkey_t * monkey = (lv_monkey_t *)indev_drv->user_data;

    data->btn_id = monkey->indev_data.btn_id;
    data->point = monkey->indev_data.point;
    data->enc_diff = monkey->indev_data.enc_diff;
    data->state = monkey->indev_data.state;
}

static int32_t random(int32_t howsmall, int32_t howbig)
{
    if (howsmall >= howbig)
    {
        return howsmall;
    }
    int32_t diff = howbig - howsmall;
    return rand() % diff + howsmall;
}

static void lv_monkey_timer_cb(lv_timer_t * timer)
{
    srand(lv_tick_get());

    lv_monkey_t * monkey = timer->user_data;
    lv_indev_data_t * data = &monkey->indev_data;

    const lv_coord_t hor_res = LV_HOR_RES;
    const lv_coord_t ver_res = LV_VER_RES;

    switch (monkey->indev_drv.type)
    {
    case LV_INDEV_TYPE_POINTER:
        data->point.x = (lv_coord_t)random(0, hor_res);
        data->point.y = (lv_coord_t)random(0, ver_res);
        break;
    case LV_INDEV_TYPE_ENCODER:
        data->enc_diff = (lv_coord_t)random(-5, 5);
        break;
    case LV_INDEV_TYPE_BUTTON:
        data->btn_id = (uint32_t)random(0, 6);
        break;
    case LV_INDEV_TYPE_KEYPAD: {
        int index = random(0, sizeof(lv_key_map) / sizeof(lv_key_t));
        data->key = lv_key_map[index];
        break;
    } 
    default:
        break;
    }

    data->state = random(0, 100) < 50 ? LV_INDEV_STATE_RELEASED : LV_INDEV_STATE_PRESSED;

    lv_timer_set_period(monkey->timer, random(monkey->timer_interval_min, monkey->timer_interval_max));
}

lv_monkey_t * lv_monkey_create(lv_indev_type_t type, uint32_t interval_min, uint32_t interval_max)
{
    lv_monkey_t * monkey = (lv_monkey_t *)lv_mem_alloc(sizeof(lv_monkey_t));
    if (monkey == NULL)
    {
        LV_LOG_ERROR("monkey alloc failed");
        return NULL;
    }

    lv_memset_00(monkey, sizeof(lv_monkey_t));

    lv_indev_drv_t * drv = &monkey->indev_drv;
    lv_indev_drv_init(drv);
    drv->type = type;
    drv->read_cb = lv_monkey_read_cb;
    drv->user_data = monkey;

    monkey->timer = lv_timer_create(lv_monkey_timer_cb, 1000, monkey);

    lv_indev_t * indev = lv_indev_drv_register(drv);

    if (type == LV_INDEV_TYPE_ENCODER || type == LV_INDEV_TYPE_KEYPAD)
    {
        lv_group_t * group = lv_group_create();
        lv_indev_set_group(indev, group);
        lv_group_set_default(group);
        monkey->group = group;
    }

    return monkey;
}

void lv_monkey_del(lv_monkey_t * monkey)
{
    if (monkey->group)
    {
        lv_group_del(monkey->group);
    }
    lv_timer_del(monkey->timer);
    lv_mem_free(monkey);
}
