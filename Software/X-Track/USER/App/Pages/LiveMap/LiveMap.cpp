#include "LiveMap.h"

#define MAP_TILE_SIZE 256
#define MAP_TILE_POS(pos) ((pos) - ((pos) % MAP_TILE_SIZE))

LiveMap::LiveMap()
    : mapUpdateIntervalTime(1000)
{
}

LiveMap::~LiveMap()
{

}

void LiveMap::onViewLoad()
{
    lv_obj_set_style_local_value_str(root, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "LOADING...");

    View.Create(root);
    AttachEvent(root);
    AttachEvent(View.ui.sliderLevel);
    AttachEvent(View.ui.map.cont);
    AttachEvent(View.ui.btnInfo);
    AttachEvent(View.ui.btnBack);

    group = lv_group_create();

    lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), group);

    lv_group_add_obj(group, View.ui.sliderLevel);
    lv_group_add_obj(group, View.ui.btnInfo);
    lv_group_add_obj(group, View.ui.btnBack);

    Model.mapConv.SetLevel(15);
    Model.mapConv.SetRootName("S:MAP/Mapinfos");
    lv_slider_set_value(View.ui.sliderLevel, Model.mapConv.GetLevel(), LV_ANIM_OFF);
    lv_obj_set_hidden(View.ui.map.cont, true);
}

void LiveMap::onViewDidLoad()
{
    
}

void LiveMap::onViewWillAppear()
{
    
}

void LiveMap::onViewDidAppear()
{
    taskUpdate = lv_task_create(TaskUpdate, 100, LV_TASK_PRIO_MID, this);
    lv_obj_set_style_local_value_str(root, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "");
    MapUpdate();
    lv_obj_set_hidden(View.ui.map.cont, false);
}

void LiveMap::onViewWillDisappear()
{
    lv_task_del(taskUpdate);
    lv_obj_set_hidden(View.ui.map.cont, true);
}

void LiveMap::onViewDidDisappear()
{
}

void LiveMap::onViewDidUnload()
{
    lv_group_del(group);  
}

void LiveMap::AttachEvent(lv_obj_t* obj)
{
    lv_obj_set_user_data(obj, this);
    lv_obj_set_event_cb(obj, EventHandler);
}

void LiveMap::Update()
{
    if (lv_tick_elaps(lastUpdateTime) >= mapUpdateIntervalTime)
    {
        MapUpdate();
        lastUpdateTime = lv_tick_get();
    }
}

void LiveMap::MapUpdate()
{
    int level = lv_slider_get_value(View.ui.sliderLevel);
    HAL::GPS_Info_t gpsInfo;
    Model.GetGPSInfo(&gpsInfo);
    MapConv* mapConv = &(Model.mapConv);

    uint32_t mapX, mapY;

    mapConv->SetLevel(level);
    mapConv->ConvertMapCoordinate(gpsInfo.longitude, gpsInfo.latitude, &mapX, &mapY);

    LoadMap(mapX, mapY);
    mapLocalX = mapX;
    mapLocalY = mapY;
    mapFocusX = mapX;
    mapFocusY = mapY;

    contLastX = lv_obj_get_x(View.ui.map.cont);
    contLastY = lv_obj_get_y(View.ui.map.cont);

    int map0_X = MAP_TILE_POS(mapFocusX) - MAP_TILE_SIZE;
    int map0_Y = MAP_TILE_POS(mapFocusY) - MAP_TILE_SIZE;

    lv_obj_t* img = View.ui.map.imgArrow;
    lv_obj_set_pos(
        img,
        mapX - map0_X - lv_obj_get_width(img) / 2,
        mapY - map0_Y - lv_obj_get_height(img) / 2
    );
    View.SetImgArrowAngle(gpsInfo.compass);

    FocusMap(mapX, mapY);

    lv_label_set_text_fmt(
        View.ui.labelInfo,
        "satel:%d\n"
        "lng: %lf\n"
        "lat:%lf\n"
        "alt: %0.2f\n"
        "compass: %0.1f\n"
        "spd: %0.2fkm/h\n"
        "%02d:%02d:%02d\n"
        "Batt: %d%%\n"
        "Steps: %d",
        gpsInfo.satellites,
        gpsInfo.longitude,
        gpsInfo.latitude,
        gpsInfo.altitude,
        gpsInfo.compass,
        gpsInfo.speed,
        gpsInfo.clock.hour,
        gpsInfo.clock.min,
        gpsInfo.clock.sec,
        HAL::Power_GetBattUsage(),
        HAL::IMU_GetSteps()
    );
}

void LiveMap::TaskUpdate(lv_task_t* task)
{
    LiveMap* instance = (LiveMap*)task->user_data;
    instance->Update();
}

void LiveMap::EventHandler(lv_obj_t* obj, lv_event_t event)
{
    LiveMap* instance = (LiveMap*)obj->user_data;

    if (obj == instance->root)
    {
        if (event == LV_EVENT_LEAVE)
        {
            instance->Manager->Pop();
        }
    }

    if (obj == instance->View.ui.btnInfo)
    {
        if (event == LV_EVENT_CLICKED)
        {
            lv_obj_t* label = instance->View.ui.labelInfo;
            lv_obj_set_hidden(label, !lv_obj_get_hidden(label));
        }
    }

    if (obj == instance->View.ui.btnBack)
    {
        if (event == LV_EVENT_CLICKED)
        {
            lv_event_send(instance->root, LV_EVENT_LEAVE, instance);
        }
    }

    if (obj == instance->View.ui.sliderLevel)
    {
        if (event == LV_EVENT_VALUE_CHANGED)
        {
            instance->lastUpdateTime = lv_tick_get() - (instance->mapUpdateIntervalTime - 50);
        }
    }

    if (obj == instance->View.ui.map.cont)
    {
        if (event == LV_EVENT_RELEASED)
        {
            lv_obj_t* contMap = instance->View.ui.map.cont;

            lv_coord_t oriX = lv_obj_get_x(contMap);
            lv_coord_t oriY = lv_obj_get_y(contMap);

            int moveOffsetX = oriX - instance->contLastX;
            int moveOffsetY = oriY - instance->contLastY;

            //LV_LOG_USER("moveOffset = (%d, %d)", moveOffsetX, moveOffsetY);

            int mapCenter_lastX = MAP_TILE_POS(instance->mapFocusX);
            int mapCenter_lastY = MAP_TILE_POS(instance->mapFocusY);

            instance->mapFocusX -= moveOffsetX;
            instance->mapFocusY -= moveOffsetY;

            int mapCenter_X = MAP_TILE_POS(instance->mapFocusX);
            int mapCenter_Y = MAP_TILE_POS(instance->mapFocusY);

            //LV_LOG_USER("mapCenter offset = (%d, %d)", mapCenter_X - mapCenter_lastX, mapCenter_Y - mapCenter_lastY);

            instance->LoadMap(instance->mapFocusX, instance->mapFocusY);
            lv_obj_set_pos(
                contMap,
                oriX + (mapCenter_X - mapCenter_lastX), 
                oriY + (mapCenter_Y - mapCenter_lastY)
            );
            lv_obj_set_pos(
                instance->View.ui.map.imgArrow, 
                instance->mapLocalX - (mapCenter_X - MAP_TILE_SIZE), 
                instance->mapLocalY - (mapCenter_Y - MAP_TILE_SIZE)
            );
        }
    }
}

void LiveMap::LoadMap(uint32_t x, uint32_t y)
{
    //LV_LOG_USER("LoadMap = (%d, %d)", x, y);

    char path[128];
    MapConv* mapConv = &(Model.mapConv);
    const int mapOffset[3] = { -MAP_TILE_SIZE, 0, MAP_TILE_SIZE };

    for (int i = 0; i < ARRAY_SIZE(View.ui.map.imgMap); i++)
    {
        mapConv->ConvertMapPath(x + mapOffset[i % 3], y + mapOffset[i / 3], path, sizeof(path));
        lv_img_set_src(View.ui.map.imgMap[i], path);
        //LV_LOG_USER("#MapPath = %s", path);
    }
}

void LiveMap::FocusMap(uint32_t x, uint32_t y)
{
    lv_coord_t centerOffsetX = MAP_TILE_SIZE / 2 - x % MAP_TILE_SIZE;
    lv_coord_t centerOffsetY = MAP_TILE_SIZE / 2 - y % MAP_TILE_SIZE;
    lv_obj_align(View.ui.map.cont, NULL, LV_ALIGN_CENTER, centerOffsetX, centerOffsetY);
}
