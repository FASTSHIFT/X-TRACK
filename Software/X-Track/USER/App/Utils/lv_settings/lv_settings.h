#ifndef LV_SETTINGS_H
#define LV_SETTINGS_H

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

class lv_settings
{
public:
    lv_settings(lv_coord_t width)
    {
        max_width = width;
    }
    ~lv_settings()
    {
        del();
    }

    typedef enum
    {
        TYPE_LIST_BTN,
        TYPE_BTN,
        TYPE_SW,
        TYPE_DDLIST,
        TYPE_NUMSET,
        TYPE_SLIDER,

        TYPE_NONE = 0xFF,
    } type_t;

    typedef struct
    {
        type_t type;
        const char * name;    /*Name or title of the item*/
        const char * value;   /*The current value as string*/
        int32_t state;        /*The current state, e.g. slider's value, switch state as a number */
        lv_obj_t * cont;
        lv_obj_t * obj;
        union
        {
            void * ptr;
            int32_t int32;
        } user_data;
    } item_t;

    /**
     * Create a settings application
     * @param root_item descriptor of the settings button. For example:
     * `lv_settings_menu_item_t root_item = {.name = "Settings", .event_cb = root_event_cb};`
     * @return the created settings button
     */
    void create(lv_obj_t * parent,lv_event_cb_t event_cb);
    
    void del()
    {
        if(act_cont != NULL)
        {
            lv_obj_del(act_cont);
            lv_group_del(group);
        }
    }

    /**
     * Automatically add the item to a group to allow navigation with keypad or encoder.
     * Should be called before `lv_settings_create`
     * The group can be change at any time.
     * @param g the group to use. `NULL` to not use this feature.
     */
    lv_group_t * group;

    /**
     * Change the maximum width of settings dialog object
     * @param max_width maximum width of the settings container page
     */
    void set_width(lv_coord_t width)
    {
        max_width = width;
    }

    /**
     * Add a list element to the page. With `item->name` and `item->value` texts.
     * @param page pointer to a menu page created by `lv_settings_create_page`
     */
    void add(item_t * item);
    void add(item_t * item, uint32_t size)
    {
        for(uint32_t i = 0; i < size; i++)
        {
            add(&item[i]);
        }
    }


    /**
     * Refresh an item's name value and state.
     * @param item pointer to a an `lv_settings_item _t` item.
     */
    void refr(item_t * item);
    
    void focus(lv_obj_t * obj);
    
    void move(int16_t dist)
    {
        lv_obj_t * obj = lv_group_get_focused(group);
        if(!focus_lock)
        {
            dist > 0 ? lv_group_focus_next(group) : lv_group_focus_prev(group);
            lv_obj_t * new_obj = lv_group_get_focused(group);
            focus(new_obj);
        }
        else
        {
            lv_event_send(obj, LV_EVENT_INSERT, &dist);
        }
    }
    
    void scroll(lv_coord_t dist);
    
    void click()
    {
        lv_obj_t * obj = lv_group_get_focused(group);
        lv_event_send(obj, LV_EVENT_CLICKED, NULL);
    }
    
    bool back()
    {
        bool ret = false;
        if(focus_lock)
        {
            click();
            ret = true;
        }
        return ret;
    }
    
    lv_obj_t * act_cont;

private:
    void create_page(lv_event_cb_t event_cb);

    void add_list_btn(lv_obj_t * page, item_t * item);
    void add_btn(lv_obj_t * page, item_t * item);
    void add_sw(lv_obj_t * page, item_t * item);
    void add_ddlist(lv_obj_t * page, item_t * item);
    void add_numset(lv_obj_t * page, item_t * item);
    void add_slider(lv_obj_t * page, item_t * item);

    void refr_list_btn(item_t * item);
    void refr_btn(item_t * item);
    void refr_sw(item_t * item);
    void refr_ddlist(item_t * item);
    void refr_numset(item_t * item);
    void refr_slider(item_t * item);

    static void list_btn_event_cb(lv_obj_t * btn, lv_event_t e);
    static void slider_event_cb(lv_obj_t * slider, lv_event_t e);
    static void sw_event_cb(lv_obj_t * sw, lv_event_t e);
    static void btn_event_cb(lv_obj_t * btn, lv_event_t e);
    static void ddlist_event_cb(lv_obj_t * ddlist, lv_event_t e);
    static void numset_event_cb(lv_obj_t * btn, lv_event_t e);

    lv_obj_t * item_cont_create(lv_obj_t * page, item_t * item);
    
    lv_obj_t * act_parent;
    lv_style_t style_menu_bg;
    lv_style_t style_bg;
    lv_style_t style_item_cont;
    
    lv_coord_t max_width;
    bool focus_lock;
};

#endif /*LV_SETTINGS_H*/
