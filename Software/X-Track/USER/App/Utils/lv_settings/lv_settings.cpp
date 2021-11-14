#include "lv_settings.h"

typedef struct
{
    lv_btn_t btn;
    lv_settings::item_t * item;
    lv_settings * lv_settings_p;
} list_btn_t;

typedef struct
{
    lv_obj_t cont;
    lv_settings::item_t * item;
    lv_settings * lv_settings_p;
} item_cont_t;

typedef struct
{
    lv_obj_t cont;
    lv_event_cb_t event_cb;
    lv_obj_t * menu_page;
} menu_cont_t;

/**
 * Create a settings application
 * @param root_item descriptor of the settings button. For example:
 * `lv_settings_menu_item_t root_item = {.name = "Settings", .event_cb = root_event_cb};`
 * @return the created settings button
 */
void lv_settings::create(lv_obj_t * parent, lv_event_cb_t event_cb)
{
    act_parent = parent;
    group = lv_group_create();
    
    lv_theme_t *th = lv_theme_set_parent
    if(th)
    {
        lv_style_copy(&style_menu_bg, th->style.cont);
        lv_style_copy(&style_item_cont, th->style.list.btn.rel);
    }
    else
    {
        lv_style_copy(&style_menu_bg, &lv_style_pretty);
        lv_style_copy(&style_item_cont, &lv_style_transp);
    }

    lv_style_copy(&style_bg, &lv_style_transp_tight);
    style_menu_bg.body.radius = 0;
    
    style_item_cont.body.border.part = LV_BORDER_BOTTOM;
    style_item_cont.body.border.color = LV_COLOR_GRAY;

    style_item_cont.body.padding.left = LV_DPI / 5;
    style_item_cont.body.padding.right = LV_DPI / 5;
    style_item_cont.body.padding.top = LV_DPI / 10;
    style_item_cont.body.padding.bottom = LV_DPI / 10;
    style_item_cont.body.padding.inner = LV_DPI / 20;

    create_page(event_cb);
}

/**
 * Add a list element to the page. With `item->name` and `item->value` texts.
 * @param item pointer to a an `lv_settings_item_t` item.
 */
void lv_settings::add(item_t * item)
{
    if(act_cont == NULL) return;

    menu_cont_ext_t * ext = (menu_cont_ext_t *)lv_obj_get_ext_attr(act_cont);
    lv_obj_t * page = ext->menu_page;


    switch(item->type)
    {
    case TYPE_LIST_BTN:
        add_list_btn(page, item);
        break;
    case TYPE_BTN:
        add_btn(page, item);
        break;
    case TYPE_SLIDER:
        add_slider(page, item);
        break;
    case TYPE_SW:
        add_sw(page, item);
        break;
    case TYPE_DDLIST:
        add_ddlist(page, item);
        break;
    case TYPE_NUMSET:
        add_numset(page, item);
        break;
    default:
        break;
    }
}

/**
 * Refresh an item's name value and state.
 * @param item pointer to a an `lv_settings_item _t` item.
 */
void lv_settings::refr(item_t * item)
{
    /*Return if there is nothing to refresh*/
    if(item->cont == NULL) return;

    switch(item->type)
    {
    case TYPE_LIST_BTN:
        refr_list_btn(item);
        break;
    case TYPE_BTN:
        refr_btn(item);
        break;
    case TYPE_SLIDER:
        refr_slider(item);
        break;
    case TYPE_SW:
        refr_sw(item);
        break;
    case TYPE_DDLIST:
        refr_ddlist(item);
        break;
    case TYPE_NUMSET:
        refr_numset(item);
        break;
    default:
        break;
    }
}

/**
 * Create a new settings container with header, hack button ,title and an empty page
 * @param item pointer to a an `lv_settings_item_t` item.
 * `item->name` will be the title of the page.
 * `LV_EVENT_REFRESH` will be sent to `item->event_cb` to create the page again when the back button is pressed.
 */
void lv_settings::create_page(lv_event_cb_t event_cb)
{
    lv_coord_t w = LV_MATH_MIN(lv_obj_get_width(act_parent), max_width);

    act_cont = lv_cont_create(act_parent, NULL);
    lv_cont_set_style(act_cont, LV_CONT_STYLE_MAIN, &style_menu_bg);
    lv_obj_set_size(act_cont, w, lv_obj_get_height(act_parent));

    menu_cont_ext_t * ext = (menu_cont_ext_t *)lv_obj_allocate_ext_attr(act_cont, sizeof(menu_cont_ext_t));
    ext->event_cb = event_cb;
    ext->menu_page = NULL;

    lv_obj_t * page = lv_page_create(act_cont, NULL);
    lv_page_set_style(page, LV_PAGE_STYLE_BG, &style_bg);
    lv_page_set_style(page, LV_PAGE_STYLE_SCRL, &lv_style_transp_tight);
    lv_page_set_scrl_layout(page, LV_LAYOUT_COL_M);
    lv_list_set_edge_flash(page, true);
    lv_obj_set_size(page, lv_obj_get_width(act_cont), lv_obj_get_height(act_parent));
    lv_obj_align(page, NULL, LV_ALIGN_CENTER, 0, 0);

    ext->menu_page = page;
}

void lv_settings::focus(lv_obj_t * obj)
{
    menu_cont_ext_t * ext = (menu_cont_ext_t *)lv_obj_allocate_ext_attr(act_cont, sizeof(menu_cont_ext_t));
    lv_obj_t * par = lv_obj_get_parent(obj);
    lv_page_focus(ext->menu_page, par, LV_ANIM_ON);
}

void lv_settings::scroll(lv_coord_t dist)
{
    menu_cont_ext_t * ext = (menu_cont_ext_t *)lv_obj_allocate_ext_attr(act_cont, sizeof(menu_cont_ext_t));
    lv_page_scroll_ver(ext->menu_page, -dist);
}

/**
 * Add a list element to the page. With `item->name` and `item->value` texts.
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
void lv_settings::add_list_btn(lv_obj_t * page, item_t * item)
{
    lv_obj_t * liste = lv_btn_create(page, NULL);
    lv_btn_set_layout(liste, LV_LAYOUT_COL_L);
    lv_btn_set_fit2(liste, LV_FIT_FLOOD, LV_FIT_TIGHT);
    lv_page_glue_obj(liste, true);
    lv_obj_set_event_cb(liste, list_btn_event_cb);
    if(group) lv_group_add_obj(group, liste);

    list_btn_ext_t * ext = (list_btn_ext_t *)lv_obj_allocate_ext_attr(liste, sizeof(list_btn_ext_t));
    ext->item = item;
    ext->item->cont = liste;
    ext->lv_settings_p = this;

    lv_obj_t * name = lv_label_create(liste, NULL);
    lv_label_set_text(name, item->name);

    lv_obj_t * value = lv_label_create(liste, NULL);
    lv_label_set_text(value, item->value);

    lv_theme_t * th = lv_theme_get_current();
    if(th)
    {
        lv_btn_set_style(liste, LV_BTN_STYLE_REL, th->style.list.btn.rel);
        lv_btn_set_style(liste, LV_BTN_STYLE_PR, th->style.list.btn.pr);
        lv_label_set_style(value, LV_LABEL_STYLE_MAIN, th->style.label.hint);
    }
    
    item->obj = liste;
}


/**
 * Create a button. Write `item->name` on create a button on the right with `item->value` text.
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
void lv_settings::add_btn(lv_obj_t * page, item_t * item)
{
    lv_obj_t * cont = item_cont_create(page, item);

    lv_obj_t * name = lv_label_create(cont, NULL);
    lv_label_set_text(name, item->name);

    lv_obj_t * btn = lv_btn_create(cont, NULL);
    lv_btn_set_fit(btn, LV_FIT_TIGHT);
    lv_obj_set_event_cb(btn, btn_event_cb);
    if(group) lv_group_add_obj(group, btn);

    lv_obj_t * value = lv_label_create(btn, NULL);
    lv_label_set_text(value, item->value);

    lv_obj_align(btn, NULL, LV_ALIGN_IN_RIGHT_MID, -style_item_cont.body.padding.right, 0);
    lv_obj_align(name, NULL, LV_ALIGN_IN_LEFT_MID, style_item_cont.body.padding.left, 0);
    
    item->obj = btn;
}

/**
 * Create a switch with `item->name` text. The state is set from `item->state`.
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
void lv_settings::add_sw(lv_obj_t * page, item_t * item)
{
    lv_obj_t * cont = item_cont_create(page, item);

    lv_obj_t * name = lv_label_create(cont, NULL);
    lv_label_set_text(name, item->name);

    lv_obj_t * value = lv_label_create(cont, NULL);
    lv_label_set_text(value, item->value);

    lv_theme_t * th = lv_theme_get_current();
    if(th)
    {
        lv_label_set_style(value, LV_LABEL_STYLE_MAIN, th->style.label.hint);
    }

    lv_obj_t * sw = lv_sw_create(cont, NULL);
    lv_obj_set_event_cb(sw, sw_event_cb);
    lv_obj_set_size(sw, LV_DPI + 10, LV_DPI / 2 + 5);
    
    if(item->user_data.ptr)
    {
        item->state = *(bool*)item->user_data.ptr;
    }
    
    if(item->state) lv_sw_on(sw, LV_ANIM_OFF);
    if(group) lv_group_add_obj(group, sw);

    lv_obj_align(name, NULL, LV_ALIGN_IN_TOP_LEFT, style_item_cont.body.padding.left, style_item_cont.body.padding.top);
    lv_obj_align(value, name, LV_ALIGN_OUT_BOTTOM_LEFT, 0, style_item_cont.body.padding.inner);
    lv_obj_align(sw, NULL, LV_ALIGN_IN_RIGHT_MID, -style_item_cont.body.padding.right - 10, 0);
    
    item->obj = sw;
}

/**
 * Create a drop down list with `item->name` title and `item->value` options. The `item->state` option will be selected.
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
void lv_settings::add_ddlist(lv_obj_t * page, item_t * item)
{
    lv_obj_t * cont = item_cont_create(page, item);

    lv_obj_t * label = lv_label_create(cont, NULL);
    lv_label_set_text(label, item->name);
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, style_item_cont.body.padding.left, style_item_cont.body.padding.top);

    lv_obj_t * ddlist = lv_ddlist_create(cont, NULL);
    lv_ddlist_set_options(ddlist, item->value);
    lv_ddlist_set_draw_arrow(ddlist, true);
//    lv_ddlist_set_fix_height(ddlist, lv_obj_get_height(page) / 2);
    lv_ddlist_set_fix_width(ddlist, lv_obj_get_width_fit(cont));
    lv_obj_align(ddlist, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, style_item_cont.body.padding.inner);
    lv_obj_set_event_cb(ddlist, ddlist_event_cb);
    
    lv_ddlist_set_selected(ddlist, item->state);
    if(group) lv_group_add_obj(group, ddlist);
    
    item->obj = ddlist;
}

/**
 * Create a drop down list with `item->name` title and `item->value` options. The `item->state` option will be selected.
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
void lv_settings::add_numset(lv_obj_t * page, item_t * item)
{
    lv_obj_t * cont = item_cont_create(page, item);
    lv_cont_set_layout(cont, LV_LAYOUT_PRETTY);

    lv_obj_t * label = lv_label_create(cont, NULL);
    lv_label_set_text(label, item->name);
    lv_obj_set_protect(label, LV_PROTECT_FOLLOW);


    lv_obj_t * btn_dec = lv_btn_create(cont, NULL);
    lv_obj_set_size(btn_dec, LV_DPI, LV_DPI);
    lv_obj_set_event_cb(btn_dec, numset_event_cb);
    if(group) lv_group_add_obj(group, btn_dec);

    label = lv_label_create(btn_dec, NULL);
    lv_label_set_text(label, "-");

    label = lv_label_create(cont, NULL);
    lv_label_set_text(label, item->value);

    lv_obj_t * btn_inc = lv_btn_create(cont, btn_dec);
    lv_obj_set_size(btn_inc, LV_DPI, LV_DPI);
    if(group) lv_group_add_obj(group, btn_inc);

    label = lv_label_create(btn_inc, NULL);
    lv_label_set_text(label, "+");
    
    item->obj = label;
}

/**
 * Create a slider with 0..256 range. Write `item->name` and `item->value` on top of the slider. The current value is loaded from `item->state`
 * @param page pointer to a menu page created by `lv_settings_create_page`
 * @param item pointer to a an `lv_settings_item_t` item.
 */
void lv_settings::add_slider(lv_obj_t * page, item_t * item)
{
    lv_obj_t * cont = item_cont_create(page, item);

    lv_obj_t * name = lv_label_create(cont, NULL);
    lv_label_set_text(name, item->name);

    lv_obj_t * value = lv_label_create(cont, NULL);
    lv_label_set_text(value, item->value);
    lv_obj_set_auto_realign(value, true);

    lv_obj_align(name, NULL, LV_ALIGN_IN_TOP_LEFT, style_item_cont.body.padding.left,
                 style_item_cont.body.padding.top);
    lv_obj_align(value, NULL, LV_ALIGN_IN_TOP_RIGHT, -style_item_cont.body.padding.right,
                 style_item_cont.body.padding.top);
    lv_obj_t * slider = lv_slider_create(cont, NULL);
    lv_obj_set_size(slider, lv_obj_get_width_fit(cont) - LV_DPI * 2, LV_DPI);
    lv_obj_align(slider, NULL, LV_ALIGN_IN_TOP_MID, 0, lv_obj_get_y(name) +
                 lv_obj_get_height(name) +
                 style_item_cont.body.padding.inner);
    lv_obj_set_event_cb(slider, slider_event_cb);
    lv_slider_set_range(slider, 0, 256);
    lv_slider_set_value(slider, item->state, LV_ANIM_OFF);
    lv_slider_set_knob_in(slider, true);
    if(group) lv_group_add_obj(group, slider);
    
    item->obj = slider;
}

void lv_settings::refr_list_btn(item_t * item)
{
    lv_obj_t * name = lv_obj_get_child(item->cont, NULL);
    lv_obj_t * value = lv_obj_get_child(item->cont, name);

    lv_label_set_text(name, item->name);
    lv_label_set_text(value, item->value);
}

void lv_settings::refr_btn(item_t * item)
{
    lv_obj_t * btn = lv_obj_get_child(item->cont, NULL);
    lv_obj_t * name = lv_obj_get_child(item->cont, btn);
    lv_obj_t * value = lv_obj_get_child(btn, NULL);

    lv_label_set_text(name, item->name);
    lv_label_set_text(value, item->value);
}

void lv_settings::refr_sw(item_t * item)
{
    lv_obj_t * sw = lv_obj_get_child(item->cont, NULL);
    lv_obj_t * value = lv_obj_get_child(item->cont, sw);
    lv_obj_t * name = lv_obj_get_child(item->cont, value);

    lv_label_set_text(name, item->name);
    lv_label_set_text(value, item->value);

    bool tmp_state = lv_sw_get_state(sw) ? true : false;
    if(tmp_state != item->state)
    {
        if(tmp_state == false) lv_sw_off(sw, LV_ANIM_OFF);
        else lv_sw_on(sw, LV_ANIM_OFF);
    }
}

void lv_settings::refr_ddlist(item_t * item)
{
    lv_obj_t * name = lv_obj_get_child(item->cont, NULL);
    lv_obj_t * ddlist = lv_obj_get_child(item->cont, name);

    lv_label_set_text(name, item->name);

    lv_ddlist_set_options(ddlist, item->value);

    lv_ddlist_set_selected(ddlist, item->state);
}

void lv_settings::refr_numset(item_t * item)
{
    lv_obj_t * name = lv_obj_get_child_back(item->cont, NULL);
    lv_obj_t * value = lv_obj_get_child_back(item->cont, name); /*It's the minus button*/
    value = lv_obj_get_child_back(item->cont, value);

    lv_label_set_text(name, item->name);
    lv_label_set_text(value, item->value);
}

void lv_settings::refr_slider(item_t * item)
{
    lv_obj_t * slider = lv_obj_get_child(item->cont, NULL);
    lv_obj_t * value = lv_obj_get_child(item->cont, slider);
    lv_obj_t * name = lv_obj_get_child(item->cont, value);

    lv_label_set_text(name, item->name);
    lv_label_set_text(value, item->value);

    if(lv_slider_get_value(slider) != item->state) lv_slider_set_value(slider, item->state, LV_ANIM_OFF);
}

/**
 * List button event callback. The following events are sent:
 * - `LV_EVENT_CLICKED`
 * - `LV_EEVNT_SHORT_CLICKED`
 * - `LV_EEVNT_LONG_PRESSED`
 * @param btn pointer to the back button
 * @param e the event
 */
void lv_settings::list_btn_event_cb(lv_obj_t * btn, lv_event_t e)
{
    /*Save the menu item because the button will be deleted in `menu_cont_create` and `ext` will be invalid */
    list_btn_ext_t * item_ext = (list_btn_ext_t *)lv_obj_get_ext_attr(btn);

    if(e == LV_EVENT_CLICKED ||
            e == LV_EVENT_SHORT_CLICKED ||
            e == LV_EVENT_LONG_PRESSED)
    {
        menu_cont_ext_t * menu_ext = (menu_cont_ext_t *)lv_obj_get_ext_attr(item_ext->lv_settings_p->act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, e, item_ext->item);
    }
    else if(e == LV_EVENT_DELETE)
    {
        item_ext->item->cont = NULL;
    }
}

/**
 * Slider event callback. Call the item's `event_cb` with `LV_EVENT_VALUE_CHANGED`,
 * save the state and refresh the value label.
 * @param slider pointer to the slider
 * @param e the event
 */
void lv_settings::slider_event_cb(lv_obj_t * slider, lv_event_t e)
{
    lv_obj_t * cont = lv_obj_get_parent(slider);
    item_cont_ext_t * item_ext = (item_cont_ext_t *)lv_obj_get_ext_attr(cont);
    item_t * item = item_ext->item;

    if(e == LV_EVENT_VALUE_CHANGED)
    {
        item_ext->item->state = lv_slider_get_value(slider);
        menu_cont_ext_t * menu_ext = (menu_cont_ext_t *)lv_obj_get_ext_attr(item_ext->lv_settings_p->act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, e, item_ext->item);
    }
    else if(e == LV_EVENT_DELETE)
    {
        item_ext->item->cont = NULL;
    }
    else if(e == LV_EVENT_CLICKED)
    {
        item_ext->lv_settings_p->focus_lock = !item_ext->lv_settings_p->focus_lock;
    }
    else if(e == LV_EVENT_INSERT)
    {
        int16_t new_val = *(int16_t*)lv_event_get_data();
        int16_t value = lv_slider_get_value(slider) - new_val;
        lv_slider_set_value(slider, value, LV_ANIM_ON);
        item->state = lv_slider_get_value(slider);
        lv_event_send(slider, LV_EVENT_VALUE_CHANGED, NULL);
        item_ext->lv_settings_p->refr(item);
    }
}

/**
 * Switch event callback. Call the item's `event_cb` with `LV_EVENT_VALUE_CHANGED` and save the state.
 * @param sw pointer to the switch
 * @param e the event
 */
void lv_settings::sw_event_cb(lv_obj_t * sw, lv_event_t e)
{
    lv_obj_t * cont = lv_obj_get_parent(sw);
    item_cont_ext_t * item_ext = (item_cont_ext_t *)lv_obj_get_ext_attr(cont);
    item_t * item = item_ext->item;

    if(e == LV_EVENT_VALUE_CHANGED)
    {
        item_ext->item->state = lv_sw_get_state(sw);
        menu_cont_ext_t * menu_ext = (menu_cont_ext_t *)lv_obj_get_ext_attr(item_ext->lv_settings_p->act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, e, item_ext->item);
    }
    else if(e == LV_EVENT_DELETE)
    {
        item_ext->item->cont = NULL;
    }
    else if(e == LV_EVENT_CLICKED)
    {
        item->state = !item->state;
        if(item->user_data.ptr)
        {
            (*(bool*)item->user_data.ptr) = item->state;
        }
        item->state ? lv_sw_on(sw, LV_ANIM_ON) : lv_sw_off(sw, LV_ANIM_ON);
        lv_event_send(sw, LV_EVENT_VALUE_CHANGED, NULL);
    }
}

/**
 * Button event callback. Call the item's `event_cb`  with `LV_EVENT_VALUE_CHANGED` when clicked.
 * @param obj pointer to the switch or the container in case of `LV_EVENT_REFRESH`
 * @param e the event
 */
void lv_settings::btn_event_cb(lv_obj_t * obj, lv_event_t e)
{
    lv_obj_t * cont = lv_obj_get_parent(obj);
    item_cont_ext_t * item_ext = (item_cont_ext_t *)lv_obj_get_ext_attr(cont);

    if(e == LV_EVENT_CLICKED)
    {
        menu_cont_ext_t * menu_ext = (menu_cont_ext_t *)lv_obj_get_ext_attr(item_ext->lv_settings_p->act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, e, item_ext->item);
    }
    else if(e == LV_EVENT_DELETE)
    {
        item_ext->item->cont = NULL;
    }
}

/**
 * Drop down list event callback. Call the item's `event_cb` with `LV_EVENT_VALUE_CHANGED` and save the state.
 * @param ddlist pointer to the Drop down lsit
 * @param e the event
 */
void lv_settings::ddlist_event_cb(lv_obj_t * ddlist, lv_event_t e)
{
    lv_obj_t * cont = lv_obj_get_parent(ddlist);
    item_cont_ext_t * item_ext = (item_cont_ext_t *)lv_obj_get_ext_attr(cont);
    item_t * item = item_ext->item;

    if(e == LV_EVENT_VALUE_CHANGED)
    {
        item->state = lv_ddlist_get_selected(ddlist);

        menu_cont_ext_t * menu_ext = (menu_cont_ext_t *)lv_obj_get_ext_attr(item_ext->lv_settings_p->act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, e, item);
    }
    else if(e == LV_EVENT_DELETE)
    {
        item->cont = NULL;
    }
    else if(e == LV_EVENT_CLICKED)
    {
        lv_ddlist_ext_t * ext = (lv_ddlist_ext_t *)lv_obj_get_ext_attr(ddlist);
        bool is_open = ext->opened;

        if(is_open)
        {
            lv_ddlist_close(ddlist, LV_ANIM_ON);
            item_ext->lv_settings_p->focus_lock = false;
            item_ext->item->state = lv_ddlist_get_selected(ddlist);
            lv_event_send(ddlist, LV_EVENT_VALUE_CHANGED, NULL);
        }
        else
        {
            lv_ddlist_open(ddlist, LV_ANIM_ON);
            item_ext->lv_settings_p->focus_lock = true;
        }
    }
    else if(e == LV_EVENT_INSERT)
    {
        int16_t new_val = *(lv_coord_t*)lv_event_get_data();
        int sel_opt = lv_ddlist_get_selected(ddlist) + new_val;
        if(sel_opt < 0)sel_opt = 0;
        lv_ddlist_set_selected(ddlist, sel_opt);
    }
    else if(e == LV_EVENT_FOCUSED)
    {
        lv_ddlist_close(ddlist, LV_ANIM_OFF);
    }
}

/**
 * Number set buttons' event callback. Increment/decrement the state and call the item's `event_cb` with `LV_EVENT_VALUE_CHANGED`.
 * @param btn pointer to the plus or minus button
 * @param e the event
 */
void lv_settings::numset_event_cb(lv_obj_t * btn, lv_event_t e)
{
    lv_obj_t * cont = lv_obj_get_parent(btn);
    item_cont_ext_t * item_ext = (item_cont_ext_t *)lv_obj_get_ext_attr(cont);
    if(e == LV_EVENT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT)
    {

        lv_obj_t * label = lv_obj_get_child(btn, NULL);
        if(strcmp(lv_label_get_text(label), "-") == 0) item_ext->item->state--;
        else item_ext->item->state ++;

        menu_cont_ext_t * menu_ext = (menu_cont_ext_t *)lv_obj_get_ext_attr(item_ext->lv_settings_p->act_cont);

        /*Call the button's event handler to create the menu*/
        lv_event_send_func(menu_ext->event_cb, NULL, LV_EVENT_VALUE_CHANGED, item_ext->item);

        /*Get the value label*/
        label = lv_obj_get_child(cont, NULL);
        label = lv_obj_get_child(cont, label);

        lv_label_set_text(label, item_ext->item->value);
    }
    else if(e == LV_EVENT_DELETE)
    {
        item_ext->item->cont = NULL;
    }
}

/**
 * Create a container for the items of a page
 * @param page pointer to a page where to create the container
 * @param item pointer to a `lv_settings_item_t` variable. The pointer will be saved in the container's `ext`.
 * @return the created container
 */
lv_obj_t * lv_settings::item_cont_create(lv_obj_t * page, item_t * item)
{
    lv_obj_t * cont = lv_cont_create(page, NULL);
    lv_cont_set_style(cont, LV_CONT_STYLE_MAIN, &style_item_cont);
    lv_cont_set_fit2(cont, LV_FIT_FLOOD, LV_FIT_TIGHT);
    lv_obj_set_click(cont, false);

    item_cont_ext_t * ext = (item_cont_ext_t *)lv_obj_allocate_ext_attr(cont, sizeof(item_cont_ext_t));
    ext->item = item;
    ext->item->cont = cont;
    ext->lv_settings_p = this;

    return cont;
}

