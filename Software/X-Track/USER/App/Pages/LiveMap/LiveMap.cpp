#include "LiveMap.h"

#define MAP_TILE_SIZE 256
#define MAP_TILE_POS(pos) ((pos) - ((pos) % MAP_TILE_SIZE))

LiveMap::LiveMap()
{
}

LiveMap::~LiveMap()
{

}

void LiveMap::onCustomAttrConfig()
{
}

void LiveMap::onViewLoad()
{
    View.Create(root);
    AttachEvent(root);
    AttachEvent(View.ui.map.cont);
    AttachEvent(View.ui.zoom.slider);

    group = lv_group_create();
    lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), group);
    lv_group_add_obj(group, View.ui.zoom.slider);
    lv_group_set_editing(group, View.ui.zoom.slider);

    Model.mapConv.SetLevel(15);
    Model.mapConv.SetRootName("/MAP/Mapinfos");
    lv_obj_add_flag(View.ui.map.cont, LV_OBJ_FLAG_HIDDEN);
}

void LiveMap::onViewDidLoad()
{

}

void LiveMap::onViewWillAppear()
{
    Model.Init();
    StatusBar::SetStyle(StatusBar::STYLE_BLACK);
    SportInfoUpdate();
    lv_obj_clear_flag(View.ui.labelInfo, LV_OBJ_FLAG_HIDDEN);
    lv_obj_fade_in(root, 250, 0);
}

void LiveMap::onViewDidAppear()
{
    timer = lv_timer_create(onTimerUpdate, 100, this);
    lastMapUpdateTime = 0;
    lv_obj_clear_flag(View.ui.map.cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(View.ui.labelInfo, LV_OBJ_FLAG_HIDDEN);
}

void LiveMap::onViewWillDisappear()
{
    lv_timer_del(timer);
    lv_obj_add_flag(View.ui.map.cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_fade_out(root, 250, 250);
}

void LiveMap::onViewDidDisappear()
{
    Model.Deinit();
}

void LiveMap::onViewDidUnload()
{
    lv_group_del(group);
}

void LiveMap::AttachEvent(lv_obj_t* obj)
{
    obj->user_data = this;
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

void LiveMap::Update()
{
    if (lv_tick_elaps(lastMapUpdateTime) >= 1000)
    {
        MapUpdate();
        SportInfoUpdate();
        lastMapUpdateTime = lv_tick_get();
    }
    else if (lv_tick_elaps(lastContShowTime) >= 3000)
    {
        lv_obj_add_state(View.ui.zoom.cont, LV_STATE_USER_1);
    }
}

void LiveMap::MapUpdateWait(uint32_t ms)
{
    lastMapUpdateTime = lv_tick_get() - 1000 + ms;
}

void LiveMap::MapUpdate()
{
    HAL::GPS_Info_t gpsInfo;
    Model.GetGPS_Info(&gpsInfo);

    int32_t zoomVal = lv_slider_get_value(View.ui.zoom.slider);

    MapConv* mapConv = &(Model.mapConv);
    mapConv->SetLevel(zoomVal);

    uint32_t mapX, mapY;
    mapConv->ConvertMapCoordinate(gpsInfo.longitude, gpsInfo.latitude, &mapX, &mapY);

    LoadMap(mapX, mapY);
    mapPos.localX = mapX;
    mapPos.localY = mapY;
    mapPos.focusX = mapX;
    mapPos.focusY = mapY;

    mapPos.contLastX = lv_obj_get_x(View.ui.map.cont);
    mapPos.contLastY = lv_obj_get_y(View.ui.map.cont);

    int map0_X = MAP_TILE_POS(mapPos.focusX) - MAP_TILE_SIZE;
    int map0_Y = MAP_TILE_POS(mapPos.focusY) - MAP_TILE_SIZE;

    lv_obj_t* img = View.ui.map.imgArrow;
    lv_obj_set_pos(
        img,
        mapX - map0_X - lv_obj_get_width(img) / 2,
        mapY - map0_Y - lv_obj_get_height(img) / 2
    );
    View.SetImgArrowAngle(gpsInfo.compass);

    FocusMap(mapX, mapY);
}

void LiveMap::SportInfoUpdate()
{
    lv_label_set_text_fmt(View.ui.sportInfo.labelSpeed, "%02d", (int)Model.sportStatusInfo.speedKph);

    lv_label_set_text_fmt(View.ui.sportInfo.labelTrip, "%0.1f km", Model.sportStatusInfo.singleDistance / 1000);

    char buf[16];
    lv_label_set_text(
        View.ui.sportInfo.labelTime,
        DataProc::ConvTime(Model.sportStatusInfo.singleTime, buf, sizeof(buf))
    );
}

void LiveMap::onTimerUpdate(lv_timer_t* timer)
{
    LiveMap* instance = (LiveMap*)timer->user_data;
    instance->Update();
}

void LiveMap::onEvent(lv_event_t* event)
{
    lv_obj_t* obj = lv_event_get_target(event);
    lv_event_code_t code = lv_event_get_code(event);
    LiveMap* instance = (LiveMap*)lv_obj_get_user_data(obj);

    if (code == LV_EVENT_LEAVE)
    {
        instance->Manager->Pop();
        return;
    }

    if (obj == instance->View.ui.map.cont)
    {
        if (code == LV_EVENT_RELEASED)
        {
            instance->ReCalibrateMapCont();
        }
    }

    if (obj == instance->View.ui.zoom.slider)
    {
        if (code == LV_EVENT_VALUE_CHANGED)
        {
            int32_t zoomVal = lv_slider_get_value(obj);
            lv_label_set_text_fmt(instance->View.ui.zoom.labelInfo, "%d/13", zoomVal - 2);

            lv_obj_clear_state(instance->View.ui.zoom.cont, LV_STATE_USER_1);
            instance->lastContShowTime = lv_tick_get();
            instance->MapUpdateWait(200);
        }
        else if (code == LV_EVENT_CLICKED)
        {
            instance->Manager->Pop();
        }
    }
}

void LiveMap::LoadMap(uint32_t x, uint32_t y)
{
    char path[128];
    MapConv* mapConv = &Model.mapConv;
    const int mapOffset[3] = { -MAP_TILE_SIZE, 0, MAP_TILE_SIZE };

    for (int i = 0; i < ARRAY_SIZE(View.ui.map.imgMap); i++)
    {
        mapConv->ConvertMapPath(x + mapOffset[i % 3], y + mapOffset[i / 3], path, sizeof(path));
        lv_img_set_src(View.ui.map.imgMap[i], path);
    }
}

void LiveMap::FocusMap(uint32_t x, uint32_t y)
{
    lv_coord_t centerOffsetX = MAP_TILE_SIZE / 2 - x % MAP_TILE_SIZE;
    lv_coord_t centerOffsetY = MAP_TILE_SIZE / 2 - y % MAP_TILE_SIZE;
    lv_obj_align(View.ui.map.cont, LV_ALIGN_CENTER, centerOffsetX, centerOffsetY);
}

void LiveMap::ReCalibrateMapCont()
{
    lv_obj_t* contMap = View.ui.map.cont;

    lv_coord_t oriX = lv_obj_get_x(contMap);
    lv_coord_t oriY = lv_obj_get_y(contMap);

    int moveOffsetX = oriX - mapPos.contLastX;
    int moveOffsetY = oriY - mapPos.contLastY;

    int mapCenter_lastX = MAP_TILE_POS(mapPos.focusX);
    int mapCenter_lastY = MAP_TILE_POS(mapPos.focusY);

    mapPos.focusX -= moveOffsetX;
    mapPos.focusY -= moveOffsetY;

    int mapCenter_X = MAP_TILE_POS(mapPos.focusX);
    int mapCenter_Y = MAP_TILE_POS(mapPos.focusY);

    LoadMap(mapPos.focusX, mapPos.focusY);
    lv_obj_set_pos(
        contMap,
        oriX + (mapCenter_X - mapCenter_lastX),
        oriY + (mapCenter_Y - mapCenter_lastY)
    );
    lv_obj_set_pos(
        View.ui.map.imgArrow,
        mapPos.localX - (mapCenter_X - MAP_TILE_SIZE),
        mapPos.localY - (mapCenter_Y - MAP_TILE_SIZE)
    );
}
