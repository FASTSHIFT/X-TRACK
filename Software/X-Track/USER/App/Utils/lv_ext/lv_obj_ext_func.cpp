#include "lv_obj_ext_func.h"

/**
  * @brief  安全删除对象，当对象为NULL的时候不执行删除
  * @param  obj:对象二级地址
  * @retval true 成功; false 失败
  */
bool lv_obj_del_safe(lv_obj_t** obj)
{
    bool ret = false;
    if(*obj != NULL)
    {
        lv_obj_del(*obj);
        *obj = NULL;
        ret = true;
    }
    return ret;
}

void lv_obj_set_opa_scale(lv_obj_t* obj, lv_opa_t opa)
{
    lv_obj_set_style_local_opa_scale(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, opa);
}

lv_opa_t lv_obj_get_opa_scale(lv_obj_t* obj)
{
    return lv_obj_get_style_opa_scale(obj, LV_OBJ_PART_MAIN);
}

/**
  * @brief  在label后追加字符串
  * @param  label:被追加的对象
  * @param  text:追加的字符串
  * @retval 无
  */
void lv_label_set_text_add(lv_obj_t * label, const char * text)
{
    if(!label)
        return;

    lv_label_ins_text(label, strlen(lv_label_get_text(label)), text);
}

/**
  * @brief  获取对象的X中心坐标
  * @param  obj:对象地址
  * @retval X中心坐标
  */
lv_coord_t lv_obj_get_x_center(lv_obj_t * obj)
{
    return (obj->coords.x2 + obj->coords.x1) / 2;
}

/**
  * @brief  获取对象的Y中心坐标
  * @param  obj:对象地址
  * @retval Y中心坐标
  */
lv_coord_t lv_obj_get_y_center(lv_obj_t * obj)
{
    return (obj->coords.y2 + obj->coords.y1) / 2;
}

/**
  * @brief  设置table对象的所有成员的对齐方式
  * @param  table:对象地址
  * @param  align:对其方式
  * @retval 无
  */
void lv_table_set_align(lv_obj_t * table, lv_label_align_t align)
{
    uint16_t col = lv_table_get_col_cnt(table);
    uint16_t row = lv_table_get_row_cnt(table);
    for(uint16_t i = 0; i < col; i++)
    {
        for(uint16_t j = 0; j < row; j++)
        {
            lv_table_set_cell_align(table, j, i, align);
        }
    }
}

/**
  * @brief  为对象添加动画
  * @param  obj:对象地址
  * @param  a:动画控制器地址
  * @param  exec_cb:控制对象属性的函数地址
  * @param  start:动画的开始值
  * @param  end:动画的结束值
  * @param  time:动画的执行时间
  * @param  delay:动画开始前的延时时间
  * @param  ready_cb:动画结束事件回调
  * @param  path_cb:动画曲线
  * @retval 无
  */
void lv_obj_add_anim(
    lv_obj_t * obj, lv_anim_t * a,
    lv_anim_exec_xcb_t exec_cb,
    int32_t start, int32_t end,
    uint16_t time,
    uint32_t delay,
    lv_anim_ready_cb_t ready_cb,
    lv_anim_path_cb_t path_cb
)
{
    lv_anim_t anim_temp;
    
    if (a == NULL)
    {
        a = &anim_temp;

        /* INITIALIZE AN ANIMATION
        *-----------------------*/
        lv_anim_init(a);
    }
        
    /* MANDATORY SETTINGS
     *------------------*/

    /*Set the "animator" function*/
    lv_anim_set_exec_cb(a, exec_cb);

    /*Set the "animator" function*/
    lv_anim_set_var(a, obj);

    /*Length of the animation [ms]*/
    lv_anim_set_time(a, time);

    /*Set start and end values. E.g. 0, 150*/
    lv_anim_set_values(a, start, end);
    

    /* OPTIONAL SETTINGS
     *------------------*/

    /*Time to wait before starting the animation [ms]*/
    lv_anim_set_delay(a, delay);

    lv_anim_path_t path;
    lv_anim_path_init(&path);
    lv_anim_path_set_cb(&path, path_cb);

    /*Set the path in an animation*/
    lv_anim_set_path(a, &path);

    /*Set a callback to call when animation is ready.*/
    lv_anim_set_ready_cb(a, ready_cb);

    /*Set a callback to call when animation is started (after delay).*/
    lv_anim_set_start_cb(a, ready_cb);

    /* START THE ANIMATION
     *------------------*/
    lv_anim_start(a);                             /*Start the animation*/
}

lv_indev_t* lv_get_indev(lv_indev_type_t type)
{
    lv_indev_t* cur_indev = NULL;
    for (;;)
    {
        cur_indev = lv_indev_get_next(cur_indev);
        if (!cur_indev)
        {
            break;
        }

        if (cur_indev->driver.type == type) {
            return cur_indev;
        }
    }
    return NULL;
}
