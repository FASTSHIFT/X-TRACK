## 目录结构
```
 X-Track
    ├─ArduinoAPI                -- 通用 Arduino API 抽象层
    ├─Core                      -- 基于标准库二次封装的抽象层
    ├─Doc                       -- 芯片相关文档
    ├─Libraries                 -- 硬件驱动程序
    │  ├─Adafruit_GFX_Library   -- Adafruit_GFX轻量级图形库
    │  ├─Adafruit_ST7789        -- ST7789屏幕驱动
    │  ├─ButtonEvent            -- 按键事件库
    │  ├─cm_backtrace           -- ARM Cortex-M 系列 MCU 错误追踪库
    │  ├─LIS3MDL                -- LIS3MDL地磁计驱动
    │  ├─LSM6DSM                -- LSM6DSM陀螺仪加速度计驱动
    │  ├─MillisTaskManager      -- 合作式任务调度器
    │  ├─SdFat                  -- 文件系统
    │  ├─StackInfo              -- 栈空间使用统计库
    │  └─TinyGPSPlus            -- NMEA协议解析器
    ├─MDK-ARM_F403A             -- AT32F403A Keil工程
    ├─MDK-ARM_F435              -- AT32F435  Keil工程
    ├─Simulator                 -- Visual Studio LVGL PC模拟器
    ├─Tools                     -- 实用工具
    └─USER                      -- 用户程序
        ├─App                   -- 应用层
        │  ├─Common             -- 通用程序
        │  │  ├─DataProc        -- 应用后台数据处理
        │  │  ├─HAL             -- 硬件抽象层定义/Mock实现
        │  │  └─Music           -- 操作音管理
        │  ├─Config             -- 应用配置文件
        │  ├─Pages              -- 页面
        │  │  ├─Dialplate       -- 表盘页面
        │  │  ├─LiveMap         -- 地图页面
        │  │  ├─Startup         -- 开机页面
        │  │  ├─StatusBar       -- 状态栏
        │  │  ├─SystemInfos     -- 系统信息页面
        │  │  └─_Template       -- 页面模板
        │  ├─Resource           -- 资源池
        │  │  ├─Font            -- 字体
        │  │  └─Image           -- 图片
        │  └─Utils              -- 通用应用层组件
        │      ├─ArduinoJson    -- JSON库
        │      ├─DataCenter     -- 消息发布订阅框架
        │      ├─Filters        -- 常用滤波算法库
        │      ├─GPX            -- GPX生成器
        │      ├─GPX_Parser     -- GPX解析器
        │      ├─lv_allocator   -- 自定义allocator
        │      ├─lv_ext         -- lvgl功能扩展
        │      ├─lv_lib_png     -- PNG解码库
        │      ├─lv_poly_line   -- 多段线控件
        │      ├─MapConv        -- WGS84/GCJ02 地图坐标转换器
        │      ├─new            -- new/delete 重载
        │      ├─PageManager    -- 页面调度器
        │      ├─PointContainer -- 坐标压缩容器
        │      ├─StorageService -- KV储存服务
        │      ├─Stream         -- Arduino Stream 流式库
        │      ├─TileConv       -- 瓦片坐标转换器
        │      ├─Time           -- 时间转换算法库
        │      ├─TonePlayer     -- 异步方波音乐播放器
        │      ├─TrackFilter    -- 流式轨迹坐标拐点/线段提取器
        │      └─WString        -- Arduino WString 字符串库
        ├─HAL                   -- 硬件抽象层
        └─lv_port               -- lvgl与硬件的接口
```
## 编译说明
* MCU固件: 务必使用**Keil v5.25**或以上的版本进行编译（旧版本编译器不能完全支持**C++ 11**的语法）。
* 编译前需要安装[雅特力](https://www.arterytek.com/cn/index.jsp)官方Pack:
> [AT32F4xx标准外设库](http://www.arterytek.com/download/Pack_Keil_AT32F4xx_CH_V1.3.7.zip)
> 
> [AT32F43x标准外设库](https://www.arterytek.com/download/Keil5_AT32MCU_AddOn_CH_V2.0.6.zip)
  ### 注意
  **不要修改芯片选型**，因为修改芯片选型后启动文件会重新生成，堆栈大小会恢复默认值，而使用默认的栈大小会导致**栈溢出**。现象是启动后立即蓝屏，提示发生**HardFault**(如下图所示)，串口会输出详细的错误信息。如果确实需要修改芯片选型，请参考工程原始的启动文件进行修改。

![IMG_20210816_201213](https://user-images.githubusercontent.com/26767803/129562550-5de4f4b6-f96c-481c-9a4e-b2470f7b3477.jpg)

* VS模拟器: 使用**Visual Studio 2019**编译，配置为**Release x86**。在`App/Common/HAL/HAL_GPS.cpp`里修改`CONFIG_TRACK_VIRTUAL_GPX_FILE_PATH`宏定义指定被读取的GPX文件的路径。

## 系统配置文件
系统会在根目录下自动生成`SystemSave.json`的文件，用于储存和配置系统参数:
```C
{
  "sportStatus.totalDistance": 0,              //总里程(m)
  "sportStatus.totalTimeUINT32[0]": 0,         //总行驶时间(ms)，低32位
  "sportStatus.totalTimeUINT32[1]": 0,         //总行驶时间(ms)，高32位
  "sportStatus.speedMaxKph": 0,                //最高时速(km/h)
  "sportStatus.weight": 65,                    //体重(kg)
  "sysConfig.longitude": 116.3913345,          //上次开机记录的位置(经度)
  "sysConfig.latitude": 39.90741348,           //上次开机记录的位置(纬度)
  "sysConfig.soundEnable": 1,                  //系统提示音使能(1:开启，0:关闭)
  "sysConfig.timeZone": 8,                     //时区(GMT+)
  "sysConfig.language": "en-GB",               //语言(尚不支持多语言切换)
  "sysConfig.arrowTheme": "default",           //导航箭头主题(default:全黑，dark:橙底黑边，light:橙底白边)
  "sysConfig.mapDirPath": "/MAP",              //存放地图的文件夹路径
  "sysConfig.mapExtName": "bin",               //地图文件扩展名
  "sysConfig.mapWGS84": 0,                     //坐标系统配置(0:GCJ02, 1:WGS84)
  "sysConfig.designCap": 1000,                 //电池设计容量
  "sysConfig.fullChgCap": 1000                 //电池充满容量(随电池损耗变化，用于持久化电量计数据，不建议修改，值需要小于等于电池设计容量)
}
```
