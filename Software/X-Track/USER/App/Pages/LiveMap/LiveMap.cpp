#include "LiveMap.h"
#include "Config/Config.h"

using namespace Page;

uint16_t LiveMap::mapLevel = CONFIG_LIVE_MAP_LEVEL_DEFAULT;

LiveMap::LiveMap()
{
    memset(&priv, 0, sizeof(priv));
}

LiveMap::~LiveMap()
{

}

void LiveMap::onCustomAttrConfig()
{
}

void LiveMap::onViewLoad()
{
    const uint32_t tileSize = 256;

    Model.tileConv.SetTileSize(tileSize);
    Model.tileConv.SetViewSize(
        CONFIG_LIVE_MAP_VIEW_WIDTH,
        CONFIG_LIVE_MAP_VIEW_HEIGHT
    );
    Model.tileConv.SetFocusPos(0, 0);

    TileConv::Rect_t rect;
    uint32_t tileNum = Model.tileConv.GetTileContainer(&rect);

    View.Create(root, tileNum);
    lv_slider_set_range(
        View.ui.zoom.slider,
        Model.mapConv.GetLevelMin(),
        Model.mapConv.GetLevelMax()
    );
    View.SetMapTile(tileSize, rect.width / tileSize);

#if 0
    lv_obj_t* contView = lv_obj_create(root);
    lv_obj_center(contView);
    lv_obj_set_size(contView, MAP_VIEW_WIDTH, MAP_VIEW_HEIGHT);
    lv_obj_set_style_border_color(contView, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_border_width(contView, 1, 0);
#endif

    AttachEvent(root);
    AttachEvent(View.ui.zoom.slider);
    AttachEvent(View.ui.sportInfo.cont);

    priv.group = lv_group_create();
    lv_indev_set_group(lv_get_indev(LV_INDEV_TYPE_ENCODER), priv.group);
    lv_group_add_obj(priv.group, View.ui.zoom.slider);
    lv_group_set_editing(priv.group, View.ui.zoom.slider);

    lv_slider_set_value(View.ui.zoom.slider, mapLevel, LV_ANIM_OFF);
    Model.mapConv.SetLevel(mapLevel);
    lv_obj_add_flag(View.ui.map.cont, LV_OBJ_FLAG_HIDDEN);

    Model.pointFilter.SetOffsetThreshold(CONFIG_TRACK_FILTER_OFFSET_THRESHOLD);
    Model.pointFilter.SetOutputPointCallback([](TrackPointFilter * filter, const TrackPointFilter::Point_t* point)
    {
        LiveMap* instance = (LiveMap*)filter->userData;
        instance->Model.TrackAddPoint((int32_t)point->x, (int32_t)point->y);
        instance->TrackViewLineUpdate((int32_t)point->x, (int32_t)point->y);
    });


    Model.pointFilter.userData = this;
}

void LiveMap::onViewDidLoad()
{

}

void LiveMap::onViewWillAppear()
{
    Model.Init();

    char theme[16];
    Model.GetArrowTheme(theme, sizeof(theme));
    View.SetArrowTheme(theme);

    priv.isTrackAvtive = Model.TrackGetFilterActive();

    StatusBar::SetStyle(StatusBar::STYLE_BLACK);
    SportInfoUpdate();
    lv_obj_clear_flag(View.ui.labelInfo, LV_OBJ_FLAG_HIDDEN);
    lv_obj_fade_in(root, 250, 0);
}

void LiveMap::onViewDidAppear()
{
    priv.timer = lv_timer_create([](lv_timer_t* timer)
    {
        LiveMap* instance = (LiveMap*)timer->user_data;
        instance->Update();
    },
    100,
    this);
    priv.lastMapUpdateTime = 0;
    lv_obj_clear_flag(View.ui.map.cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(View.ui.labelInfo, LV_OBJ_FLAG_HIDDEN);

    priv.lastTileContOriPoint.x = 0;
    priv.lastTileContOriPoint.y = 0;
    priv.trackReloadReq = true;
}

void LiveMap::onViewWillDisappear()
{
    lv_timer_del(priv.timer);
    lv_obj_add_flag(View.ui.map.cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_fade_out(root, 250, 250);
}

void LiveMap::onViewDidDisappear()
{
    Model.Deinit();
}

void LiveMap::onViewDidUnload()
{
    View.Delete();
    lv_group_del(priv.group);
}

void LiveMap::AttachEvent(lv_obj_t* obj)
{
    obj->user_data = this;
    lv_obj_add_event_cb(obj, onEvent, LV_EVENT_ALL, this);
}

void LiveMap::Update()
{
    if (lv_tick_elaps(priv.lastMapUpdateTime) >= CONFIG_GPS_REFR_PERIOD)
    {
        MapUpdate();
        SportInfoUpdate();
        priv.lastMapUpdateTime = lv_tick_get();
    }
    else if (lv_tick_elaps(priv.lastContShowTime) >= 3000)
    {
        lv_obj_add_state(View.ui.zoom.cont, LV_STATE_USER_1);
    }
}

void LiveMap::MapUpdateWait(uint32_t ms)
{
    priv.lastMapUpdateTime = lv_tick_get() - 1000 + ms;
}

void LiveMap::MapUpdate()
{
    HAL::GPS_Info_t gpsInfo;
    Model.GetGPS_Info(&gpsInfo);

    mapLevel = lv_slider_get_value(View.ui.zoom.slider);
    if (mapLevel != Model.mapConv.GetLevel())
    {
        priv.trackReloadReq = true;
    }

    Model.mapConv.SetLevel(mapLevel);

    int32_t mapX, mapY;
    Model.mapConv.ConvertMapCoordinate(
        gpsInfo.longitude, gpsInfo.latitude,
        &mapX, &mapY
    );
    Model.tileConv.SetFocusPos(mapX, mapY);

    TileConv::Point_t offset;
    TileConv::Point_t oriPoint = { mapX, mapY };
    Model.tileConv.GetOffset(&offset, &oriPoint);

    /* track line */
    if (priv.isTrackAvtive)
    {
        if (priv.trackReloadReq)
        {
            Model.TrackReload();
            priv.trackReloadReq = false;
        }

        if (TrackCheckTileContReload())
        {
            TrackViewLineReload();
        }

        Model.pointFilter.PushPoint(mapX, mapY);
        View.TrackSetActivePoint(offset.x, offset.y);
    }

    /* arrow */
    lv_obj_t* img = View.ui.map.imgArrow;
    Model.tileConv.GetFocusOffset(&offset);
    lv_coord_t x = offset.x - lv_obj_get_width(img) / 2;
    lv_coord_t y = offset.y - lv_obj_get_height(img) / 2;
    View.SetImgArrowState(x, y, gpsInfo.course);

    /* map cont */
    Model.tileConv.GetTileContainerOffset(&offset);

    lv_coord_t baseX = (LV_HOR_RES - CONFIG_LIVE_MAP_VIEW_WIDTH) / 2;
    lv_coord_t baseY = (LV_VER_RES - CONFIG_LIVE_MAP_VIEW_HEIGHT) / 2;
    lv_obj_set_pos(View.ui.map.cont, baseX - offset.x, baseY - offset.y);

    /* tile src */
    for (uint32_t i = 0; i < View.ui.map.tileNum; i++)
    {
        TileConv::Point_t pos;
        Model.tileConv.GetTilePos(i, &pos);

        char path[64];
        Model.mapConv.ConvertMapPath(pos.x, pos.y, path, sizeof(path));

        lv_img_set_src(View.ui.map.imgTiles[i], path);
    }
}

void LiveMap::TrackViewLineUpdate(int32_t x, int32_t y)
{
    TileConv::Point_t oriPoint = { x, y };
    TileConv::Point_t offset;

    Model.tileConv.GetOffset(&offset, &oriPoint);
    View.TrackAddPoint(offset.x, offset.y);
}

void LiveMap::TrackViewLineReload()
{
    uint32_t cnt = Model.TrackGetCnt();
    TileConv::Point_t* points = Model.TrackGetPoints();
    View.TrackReset();
    for (uint32_t i = 0; i < cnt; i++)
    {
        TrackViewLineUpdate(points[i].x, points[i].y);
    }
}

bool LiveMap::TrackCheckTileContReload()
{
    TileConv::Point_t pos;
    Model.tileConv.GetTilePos(0, &pos);

    bool ret = (pos.x != priv.lastTileContOriPoint.x || pos.y != priv.lastTileContOriPoint.y);

    priv.lastTileContOriPoint = pos;

    return ret;
}

void LiveMap::SportInfoUpdate()
{
    lv_label_set_text_fmt(
        View.ui.sportInfo.labelSpeed,
        "%02d",
        (int)Model.sportStatusInfo.speedKph
    );

    lv_label_set_text_fmt(
        View.ui.sportInfo.labelTrip,
        "%0.1f km",
        Model.sportStatusInfo.singleDistance / 1000
    );

    char buf[16];
    lv_label_set_text(
        View.ui.sportInfo.labelTime,
        DataProc::ConvTime(Model.sportStatusInfo.singleTime, buf, sizeof(buf))
    );
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

    if (obj == instance->View.ui.zoom.slider)
    {
        if (code == LV_EVENT_VALUE_CHANGED)
        {
            int32_t level = lv_slider_get_value(obj);
            int32_t levelMax = instance->Model.mapConv.GetLevelMax();
            lv_label_set_text_fmt(instance->View.ui.zoom.labelInfo, "%d/%d", level + 1, levelMax + 1);

            lv_obj_clear_state(instance->View.ui.zoom.cont, LV_STATE_USER_1);
            instance->priv.lastContShowTime = lv_tick_get();
            instance->MapUpdateWait(200);
        }
        else if (code == LV_EVENT_PRESSED)
        {
            instance->Manager->Pop();
        }
    }

    if (obj == instance->View.ui.sportInfo.cont)
    {
        if (code == LV_EVENT_PRESSED)
        {
            instance->Manager->Pop();
        }
    }
}
