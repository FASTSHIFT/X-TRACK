# X-TRACK 更新日志
> https://github.com/FASTSHIFT/X-TRACK

## [v0.1] - 2021-3-21
* 1.框架搭建完成，全新MVP(Model-View-Presenter)架构
* 2.更新3D Model，门板完美安装
* 3.修复串口缓冲队列过载导致GPS解析连续失败的BUG
* 4.添加长按开机功能
* 5.添加计步显示
* 6.速度显示使用kph为单位
* 7.添加新的ButtonEvent库
* 8.添加Filters数字滤波器库 
* 9.添加新的TonePlayer库
* 10.格式化LIS3DMDL库的代码
* 11.格式化LISM6DSM库的代码
* 12.添加新的简易表盘页面
* 13.为LiveMap页面添加group，优化Encoder交互
* 14.LiveMap已可以正常显示航向，使用GPS的航向信息
* 15.重新校准MapConv地图坐标
* 16.添加HAL_AHRS姿态解算
* 17.添加HAL_Encoder接口
* 18.添加HAL_GPS串口透传，可使用上位机对GPS模块进行配置
* 19.添加GNSSToolKit_Lite配置工程
* 20.添加lv_get_indev()
* 21.添加__wfi()，可能省点电

## [v0.2] - 2021-4-19
* 1.添加ArtDesign工程，使用Adobe Illustrator
* 2.添加异步无锁Buffer库：PingPongBuffer
* 3.删除离线图片和字体转换工具
* 4.修改页面ID格式：DialplatePage->Pages/Dialplate
* 5.添加消息发布订阅框架：DataCenter
* 6.将HAL.h转移至应用层
* 7.HAL: Clock_GetValue->Clock_GetInfo
* 8.在PC模拟器上使用PC的电量显示
* 9.更新PageManager
* 10.降低SD热插拔监控扫描速度
* 11.删除Music_Astronomia
* 12.在编码器按下时忽略滚动，减少误触
* 13.更新Dialplate页面
* 14.更新LiveMap页面

## [v0.3] - 2021-5-12
* 1.lvgl迁移至v8.0.0
* 2.PingPongBuffer转移至App/Utils/DataCenter，以提供无锁的线程安全的数据块访问，添加volatile关键字
* 3.VS工程在Debug下启用AddressSanitizer，捕获异常的内存操作
* 4.Heap和Stack调整
* 5.更新ButtonEvent库
* 6.更换新的文件系统接口lv_fs_if
* 7.系统配置文件SysConfig.h -> Config.h
* 8.Utils添加WString for PC
* 9.ResourceManager更新，使用std::vector作为资源池
* 10.PageManager更新，lvgl-v8适配，简化子模块命名，使用std::vector作为页面池，使用std::stack作为页面栈
* 11.地图坐标转换器MapConv更新，但未找到更精确的地图坐标换算算法
* 12.去除lv_theme_custom
* 13.添加GPX库，将来用于生成GPX格式的标准轨迹记录文件
* 14.使用C++ template重写Filters滤波器合集库
* 15.数据发布订阅处理中心DataCenter更新，添加错误码、通知、数据提交等功能
* 16.StatusBar更新，添加背景色柔和渐变、电池充电等特效
* 17.LiveMap页面更新，新的滚动条和info按钮
* 18.Dialplate页面更新，操作按钮已可以使用
* 19.数据处理节点DP_SportStatus更新，更新卡路里换算算法、距离、速度滤波等处理方法
* 20.添加新的数据处理节点DP_Recorder，用于记录轨迹于SD卡中
* 21.电池AD采样使用中断模式，而不使用DMA
* 22.文件系统SD对象不再全局共享
* 23.屏蔽开机启动时Encoder按键事件

## [v0.4] - 2021-5-16
* 1.lvgl更新
* 2.支持导出GPX格式的轨迹记录文件，推荐使用[GPXSee](https://github.com/tumic0/GPXSee)查看轨迹
* 3.Clock_info_t结构体成员单词全拼
* 4.LiveMap页面更新，改进的操作方式与显示特效
* 5.去除MAP数据映射宏，使用lv_map替代
* 6.SD支持时间戳记录，支持自动检测和创建缺失的文件夹

## [v0.5] - 2021-5-23
* 1.添加SystemInfos页面
* 2.添加IMU和地磁MAG数据处理节点
* 3.修改轨迹记录默认文件名
* 4.规范lv_event成员参数获取方式
* 5.修复DataCenter缓冲区读取释放BUG
* 6.添加Version.h存放版本信息
* 7.HAL::GPS_Info_t添加hour_utc成员

## [v0.6] - 2021-5-31
* 1.更新lvgl
* 2.添加App_Uninit()，绑定关机事件
* 3.添加DP_LIST.inc，更优雅的数据处理节点注册方式
* 4.添加IMU/MAG_Commit，向数据中心提交数据
* 5.添加StorageService数据储存服务，使用JSON格式储存数据
* 6.添加ArduinoJson解析库
* 7.运动数据处理节点 sportStatusInfo -> sportStatus，添加储存绑定
* 8.去除HAL::AHRS
* 9.GPS在未定位成功时，经纬度海拔统一置零
* 10.HAL::Power添加关机事件回调
* 11.CONTROLLER -> PRESENTER
* 12.添加Startup页面，展示开机动画
* 13.LiveMap地图页面，在未定位成功时的默认位置为天安门
* 14.StatusBar状态栏，函数AppearAnimStart -> Appear 
* 15.解决SystemInfos页面在自动管理Cache时退出页面造成的Crash
* 16.修复MAG数据显示的bug
* 17.DataCenter添加主账户AccountMain，自动订阅所有的子节点，用于发送通知
* 18.PageManager在删除root时需要使用异步删除lv_obj_del_async()，在root动画生命周期结束后删除
* 19.降低GPS和Sensor的数据更新频率，分别为5Hz和1Hz

## [v0.7] - 2021-6-5
* 1.更新拆分STL模型文件
* 2.TonePlayer转移至App/Utils
* 3.在关机时自动保存正在记录的轨迹文件
* 4.优化轨迹记录操作体验
* 5.DataCenter合并定时器回调到AccountEvent
* 6.AccountEvent添加Account* 参数
* 7.添加DATA_PROC_INIT_DEF节点定义
* 8.添加StatusBar状态栏、MusicPlayer播放器、TzConv时区转换器数据节点
* 9.优化电池电量滤波算法 Hysteresis + MedianQueue
* 10.转移STORAGE_VALUE_REG宏定义至DataProc_Def.h
* 11.在SD插入时自动加载JSON系统配置文件
* 12.添加Common/Music
* 13.优化DataCenter和PageManager日志输出等级
* 14.优化GPX库
* 15.优化PAGE_STASH_POP
* 16.去除PageManager::Pop函数的stash参数
* 17.添加Arduino Time时间转换库
* 18.去除GPS_Info_t的hour_utc成员，使用TzConv进行时区转换

## [v0.8] - 2021-6-15
* 1.格式化所有代码
* 2.APP_Init添加ACCOUNT_SEND_CMD，简化Account通知调用
* 3.dataCenter("Bilibili") -> dataCenter("CENTER")
* 4.添加SysConfig节点，用于保存系统设置
* 5.Power节点使用Account通知方式调用MusicPlayer，而不使用HAL::Audio_PlayMusic
* 6.去除HAL::Backlight_UpdateBKP()和Backlight_GetBKP()
* 7.DialplateModel添加节点名称验证
* 8.LiveMapModel添加节点名称验证
* 9.DataCenter添加Account双buffer动态内存申请检查
* 10.Account::GetPublisherLen() -> GetPublisherSize(), GetSubscribeLen() -> GetSubscribeSize()
* 11.更新lv_anim_timeline，支持设置progress，已提交至lvgl主线
* 12.去除lv_obj_ext_func不常用的函数
* 13.StorageService扩大JSON_BUFFER_SIZE 256 -> 512, 添加字符串类型参数读取返回值检查
* 14.添加硬件定时器配置到Config.h
* 15.修改版权声明遗留拼写错误

## [v0.9] - 2021-6-24
* 1.更新ButtonEvent，支持EVENT_LONG_PRESSED_RELEASED
* 2.调整lvgl内存池，32KB -> 64KB
* 3.HAL_GPS支持读取csv格式的文件，在PC上模拟位置变化
* 4.更新LiveMap页面，支持实时绘制运动轨迹，支持自适应分辨率动态加载地图
* 5.更新Startup页面，移除默认样式，并在开机动画过程中屏蔽Encoder事件
* 6.修复SystemInfos页面因为忘记释放style.focus导致的内存泄漏
* 7.优化DataCenter的Account的cache申请失败判断逻辑
* 8.添加lv_allocator，使用lvgl接管std::vector内存管理实现
* 9.更新MapConv，MapKeyRootName -> MapFilePath，ConvertMapTile() -> ConvertPosToTile()
* 10.添加TileConv，通用的瓦片动态加载算法实现
* 11.添加TrackFilter轨迹过滤器，包含TrackPointFilter曲线关键点提取算法和TrackLineFilter曲线区域裁剪算法
* 12.HAL::Encoder添加Encoder_SetEnable()方法

## [v1.0] - 2021-7-1
* 1.更新lvgl v8.1.0 dev
* 2.添加TrackFilter数据处理节点，储存过滤后的拐点坐标
* 3.优化DP_Clock RTC自动校准
* 4.优化DP_GPS提示音播放，添加cache，使用Publish向订阅者推送数据，减少定时轮询拉取
* 5.添加GPX_Parser，支持直接解析GPX数据还原轨迹
* 6.Config.h转移到App
* 7.页面添加namespace Page命名空间
* 8.优化LiveMap轨迹显示，在轨迹记录时显示轨迹，使用lambda表达式替换部分回调函数
* 9.优化StatusBar充电动画
* 10.DataCenter添加注释
* 11.PageManager添加注释
* 12.lv_allocater添加遗漏的运算符重载
* 13.添加Utils/Stream，来自Arduino
* 14.转移CommmonMacro.h到HAL，去除Basic文件夹

## [v1.1] - 2021-7-7
* 1.支持在GPX中记录海拔和时间，可在GPXSee查看爬升和速度统计
* 2.更新Startup页面，使用匿名函数回调
* 3.更新SystemInfos页面，添加Author署名和Build时间，电池状态 State -> Status
* 4.更新lvgl
* 5.更新GPX生成库，添加遗漏的time_成员
* 6.尝试将lvgl部分函数放在RAM中执行，但是执行速度提升不明显

## [v1.2] - 2021-7-11
* 1.添加StackInfo库，支持在裸机环境下测量栈使用情况
* 2.Stack_Size 0x2500 -> 0x1000
* 3.更新DP_Storage数据储存节点，支持拉取储存器使用情况
* 4.更新Config.h，添加显示器分辨率和缓冲区配置
* 5.添加触控逻辑
* 6.修复StatusBar充电动画闪烁的BUG，添加SD卡图标
* 7.SystemInfos页面添加SD卡状态监测显示

## [v1.3] - 2021-7-18
* 1.Config.h添加传感器使能配置选项
* 2.DataCenter添加cache自动初始化置0
* 3.添加Microsoft官方TileSystem解析库，供mapConv调用

## [v1.4] - 2021-7-24
* 1.更新lvgl
* 2.更新MapConv，统一使用GPS_Transform校准坐标
* 3.更新HAL::GPS模拟，支持航向和速度模拟
* 4.修改HAL::GPS_Info_t的 compass -> course
* 5.使用合入lvgl主线的lv_anim_timeline，使用lv_anim_timeline_wrapper封装动画组
* 6.更新Startup页面，新的开机动画
* 7.LV_IMG_CACHE_DEF_SIZE设置为0

## [v1.5] - 2021-8-1
* 1.lvgl更新
* 2.调整Stack size: 0x1500 -> 0x2000，处理更大的JSON文件
* 3.更新Storage节点，支持获取SD卡容量，支持自动获取地图缩放等级(OSM)
* 4.优化GPS节点，减少在GPS信号不稳定时频繁的提示音
* 5.优化SportStatus节点，支持在JSON文件中配置体重
* 6.优化SysConfig节点，支持在JSON配置默认经纬度，地图源(Bing/OSM)，soundDisable -> soundEnable
* 7.重构MapConv地图坐标转换器，支持动态配置地图源，地图坐标数据类型uint32_t -> int32_t，合并TileSystem和GPS_Transform
* 8.HAL添加SD_GetCardSizeMB()
* 9.更新Config.h，转移部分配置选项到JSON
* 10.更新Dialplate页面，卡路里单位cal -> k，优化编码器交互wrap = false
* 11.更新LiveMap页面，优化level自适应显示，使用SysConfig配置的默认位置
* 12.更新SystemInfos页面，支持显示SD卡容量
* 13.修复lv_fs_sdfat错误的指针类型转换，优化目录读取接口
* 14.lvgl内存池 64K -> 72K
* 15.WString使用lvgl的内存池
* 16.修复GPX轨迹文件<\ele>丢失问题(可能是内存碎片和heap过小导致WString realloc失败)

## [v1.6] - 2021-8-7
* 1.更新ArtDesign，新的导航箭头设计
* 2.更新lv_drivers，去除坐标超界警告
* 3.去除lv_examples，减少工程体积
* 4.调整模拟器的LV_FS_PC_PATH定义，指定到Software所在目录，方便读取地图
* 5.开机页面重命名: StartUp -> Startup
* 6.更新DP_SysConfig，mapSource -> mapDirPath指定地图目录，添加 WGS84/GCJ02 坐标系统切换，添加arrowTheme导航箭头主题切换
* 7.更新DP_Storage，适配新的MapConv，支持所有地图源缩放level范围自适应
* 8.更新DP_TrackFilter，适配新的MapConv
* 9.HAL添加Backlight_ForceLit()，支持强制点亮屏幕背光
* 10.更新模拟器的HAL_GPS，支持获取GPX点之间的时间差
* 11.更新Config.h，去除旧的Bing地图配置
* 12.更新LiveMap页面，支持修改导航箭头主题
* 13.更新GPX_Parser，添加海拔和时间解析
* 14.重构MapConv，废弃旧的Bing地图存放格式和地图下载器。使用[Crimson/地图下载器](https://gitee.com/CrimsonHu/java_map_download/)下载地图，文件存放格式为`/z/x/y.bin`
* 15.更新DisplayFault，在进入HardFault状态时强制点亮背光
* 16.更新HAL_Power，优化电压显示

## [v1.7] - 2021-9-3
* 1.调整Heap_Size 0x00002000 -> 0x00000200
* 2.调整lvgl内存池大小，LV_MEM_SIZE 72K -> 80K
* 3.更新lv_drivers
* 4.更新lvgl
* 5.DataCenter使用lvgl内存池
* 6.使用std::abs()替代宏实现的ABS()
* 7.更新DP_Storage
* 8.更新DP_SysConfig，调整结构体成员，添加timeZone和mapExtName，在关机时保存位置
* 9.更新DP_TrackFilter，使用地图上的Point格式替代旧的经纬度
* 10.更新DP_TzConv，GMT_OffsetHours -> sysCfg.timeZone
* 11.更新HAL.h，添加Memory_DumpInfo()堆栈使用统计
* 12.更新HAL_GPS模拟，修复diffTime = 0导致的gpsInfo.speed = inf
* 13.更新Config.h，添加LiveMap的Debug模式配置，PNG解码库使能配置等。转移Hardware Configuration至HAL_Config.h
* 14.更新Dialplate页面，使用lv_group_get_default()获取统一的group
* 15.重构LiveMap页面，添加LiveMap的Debug模式，使用lv_group_get_default()获取统一的group，适配新的轨迹过滤算法
* 16.更新StartUp页面，关闭LOGO滚动
* 17.更新SystemInfos，使用lv_group_get_default()获取统一的group
* 18.更新DataCenter，统一使用lv_allocator分配内存
* 19.添加lv_lib_png，限于内存大小暂不使用
* 20.添加lv_monkey，实现简单压力测试
* 21.添加lv_multi_line，实现轨迹的多段线绘制，提升lv_line复用率，极大节省内存
* 22.更新MapConv，支持自定义文件扩展名，添加ConvertMapPos实现不同level间的坐标换算
* 23.更新TileSystem，使用namespace替换class，免去无意义的实例化
* 24.添加new、delete重载，统一使用lvgl内存池
* 25.扩展StorageService的内存，JSON_BUFFER_SIZE 1024->2048
* 26.更新TrackFilter，修复二级滤波拐点丢失问题。使用TrackLineFilter实现可视区轨迹裁剪，节省内存，提升绘制效率。
* 27.更新lv_fs_sdfat接口，使用new和delete替换lv_mem_alloc和lv_mem_free
* 28.更新lv_port_indev，去除lv_port_indev_get()，创建全局默认group
* 29.添加HAL::Memory_DumpInfo()，由于要使用__heapstats()读取heap信息，关闭microlib
* 30.添加rt_sys.cpp，实现一些C底层接口
* 31.读取StackInfo从main.cpp转移至HAL::Memory_DumpInfo()

## [v1.8] - 2021-9-14
* 1.StackInfo库 StackInfo_GetSize -> StackInfo_GetTotalSize
* 2.移除多余的RTE/Device
* 3.Common/HAL模拟器实现转移到Simulator\LVGL.Simulator\HAL
* 4.VS模拟器HAL_GPS适配新的GPX_Parser
* 5.更新lv_conf.h LV_CIRCLE_CACHE_SIZE 8 -> 32
* 6.更新lv_fs_pc，去除无用代码
* 7.更新LVGL.Simulator，lv_png_init和lv_monkey_create转移至App_Init初始化
* 8.更新lvgl主程序
* 9.更新App初始化，调整Storage和SysConfig的初始化顺序，修复错误的全屏滑动
* 10.更新DataProc初始化，center实例化转移到函数外部，DP_DEF宏全大写
* 11.更新DataProc声明，ConvTime -> MakeTimeString，去除IMU_Commit和MAG_Commit
* 12.更新Storage_Basic_Info_t，添加type成员，支持获取SD卡类型
* 13.更新DP_Clock，ConvTime -> MakeTimeString
* 14.更新DP_IMU，去除IMU_Commit机制，使用回调进行数据提交
* 15.更新DP_MAG，同上
* 16.更新DP_Recorder，合并变量至Recorder_t结构体，添加RECORDER_GPX宏配置
* 17.更新DP_Storage，适配新的StorageService，添加SD卡类型获取
* 18.更新DP_TrackFilter，PointVector_t不再使用lv_allocator
* 19.更新HAL，添加Display和FaultHandle抽象层，以及IMU和MAG的CommitCallback，获取SD卡类型的函数
* 20.去除lv_allocator依赖，统一使用重载的new和delete进行分配
* 21.更新SystemInfos页面，SD卡状态指示 "Detect" -> "Status"，支持显示SD卡类型，在Sensor Config未开启时数据全部置0
* 22.调整ResourcePool的Font Default设置
* 23.ArduinoJson库的DynamicJsonDocument使用lvgl内存池分配内存
* 24.更新DataCenter/Account，"Push" -> "Publish"
* 25.更新DataCenter和PageManager的LOG写法
* 26.更新GPX_Parser，添加错误码
* 27.lv_lib_png使用lvgl内存池分配内存
* 28.更新lv_monkey，添加新的配置选项
* 29.更新MapConv，ConvertMapPos -> ConvertMapLevelPos
* 30.在VS模拟器上对new/delete进行重载，但是由于ASAN导致重定义冲突，只在Release下重载
* 31.更新PageManager，解决lv_anim_exec_xcb_t回调的lv_coord_t和int32_t混用问题，修复LOAD_ANIM_NONE页面切换的BUG
* 32.更新StorageService，支持静态和动态内存分配切换
* 33.去除USER/Display，转移至HAL层
* 34.优化lv_port，使用统一的初始化函数，与硬件驱动解耦，lv_fs_sdfat -> lv_port_fs_sdfat
* 35.修复HAL_Buzz静音模式下的错误开机音
* 36.更新HAL_Config，支持GPS缓冲区过载检测，支持配置Power的开关机延时时间
* 37.更新HAL_Power，合并变量到Power_t
* 38.更新HAL_SD_CARD，支持获取SD卡类型，SD_CardSizeMB -> SD_CardSize

## [v1.9] - 2021-10-1
* 1.修复ArduinoAPI中未初始化变量
* 2.去除lv_fs_pc中未使用的函数
* 3.去除unistd.h
* 4.更新lvgl主程序
* 5.Resource管理器，Resource. -> ResourcePool::
* 6.添加Utils/PointContainer组件
* 7.DP_Storage添加地图缩放级别错误检查
* 8.DP_TackFilter添加PointContainer组件适配，减少75%内存占用，极大提升轨迹录制容量
* 9.多段线组件lv_multi_line -> lv_poly_line
* 10.更新ArduinoJson
* 11.Display_DumpCrashInfo report -> info
* 12.更新HAL_Power，不使用ADC中断模式

## [v2.0] - 2021-11-14
* 1.Arduino API底层重构
* 2.添加MillisTaskManager任务调度器
* 3.lvgl使用标准memset和memcpy
* 4.更新lvgl v8.1.0
* 5.DataCenter错误码枚举 ERROR_XXX -> RES_XXX
* 6.修改DP节点的结构体初始化方式，不使用默认初始化
* 7.优化DP_SportStatus在临时信号丢时的数据统计处理
* 8.修复StorageService文件结尾读取错误导致的crash
* 9.添加lv_settings，暂未移植
* 10.添加传感器自动扫描，防止未安装传感器导致的开机卡死
* 11.使用event携带user_data
* 12.优化new重载
* 13.修复HAL_Power错误的关机处理
* 14.添加SystemSave.json自动备份功能

## [v2.1] - 2021-12-16
* 1.ArduinoAPI更新
* 2.将LVGL部分绘图函数放到RAM执行，提升运行效率，不删除.sct文件
* 3.Adafruit_ST778驱动更新，适配新的SPI库
* 4.更新lvgl v8.1.1 dev
* 5.lv_conf.h更新，使用新的缩进
* 6.lv_monkey合入LVGL主线
* 7.去除LiveMap进入时的透明度渐变动画，提升流畅度
* 8.更新HAL_Power，充电检测上下拉可选
* 9.优化IMU和MAG初始化

## [v2.2] - 2021-12-30
* 1.添加系统软件架构说明
* 2.去除弃用的_LIS3MDL
* 3.修复使用strncpy()遗漏的'\0'结束符
* 4.添加AT32F435移植
* 5.LV_DISP_DEF_REFR_PERIOD 20ms -> 16ms
* 6.LV_MEM_SIZE 78KB -> 70KB
* 7.默认开启PNG解码器，受限于内存MCU无法使用
* 8.添加轨迹记录状态指示渐变动画
* 9.添加文本动画组件lv_anim_label，替换旧的lv_label_anim_effect
* 10.去除lv_settings

## [v2.3] - 2022-1-30
* 1.Arduino API 更新
* 2.添加wdg看门狗，系统卡死时自动重启
* 3.lvgl主程序更新
* 4.添加低内存开销(<50KB)的PNGdec解码器（暂不启用）
* 5.更新LiveMap，适配新的PNGdec
* 6.更新StatusBar，调整字体，新的轨迹记录状态指示渐变动画
* 7.ResourceManager从PageManager分离
* 8.lv_anim_label组件从lv_ext分离
* 9.添加lv_img_png组件

## [v2.4] - 2022-2-27
* 1.添加指定的Pack
* 2.AT32F435 Pack更新 2.0.0 -> 2.0.6
* 3.__wfi() -> __WFI()
* 4.移除lv_demo_conf.h
* 5.更新lvgl v8.3.0 dev
* 6.AT32F403A Stack size 0x2000 -> 0x1000

## [v2.5] - 2022-3-18
* 1.更新Adafruit_ST7789，支持颜色大小端切换
* 2.更新CommonMacro库
* 3.更新lv_conf.h
* 4.去除lv_lib_png，使用lvgl内置PNG库
* 5.DataProc 添加 DATA_PROC_INIT_STRUCT() 结构体初始化
* 6.StatusBar 统一走 DataCenter 通信，只保留初始化API
* 7.规范 DataProc 的 onEvent 回调函数的返回值
* 8.去除 Page.h 公共空间的 ARRAY_SIZE() 宏定义
* 9.ResourcePool::Font 默认返回 LV_FONT_DEFAULT，关闭不必要的内置字体
* 10.更新PageManager，lv_event_get_target() -> lv_event_get_current_target()

## [v2.6] - 2022-4-10
* 1.ArduinoAPI更新
* 2.整理 Core，统一移动到 MDK-ARM_F4XX/Platform 中
* 3.更新lvgl主程序
* 4.LV_SPRINTF_CUSTOM使能，减少FLASH占用
* 5.account->Pull()操作全部添加返回值检查
* 6.PageManager更新
* 7.页面Template添加生命周期LOG
* 8.DataCenter添加ACCOUNT_DISCARD_READ_DATA
* 9.__WFI() -> __wfi()
* 10.main.cpp复用
* 11.修复[TscanCode](https://github.com/Tencent/TscanCode)静态扫描出来的漏洞
* 12.添加Linux移植

## [v2.7] - 2022-6-3
* 1.Adafruit_ST7789: 添加多分辨率支持; 去除drawRGBBitmap(); 删除Adafruit_ST7789V2
* 2.AT32F403A: 更新Platform; 更新链接脚本，将lv_tlsf和font_bahnschrift_13载入RAM提升访问性能
* 3.AT32F435: 更新Platform
* 4.lv_conf.h: 同步主线配置; 使能LV_COLOR_SCREEN_TRANSP; WIN平台 GetTickCount() -> timeGetTime()
* 5.lvgl: 同步 commit 941d95b0491a1e78fadeeb5ea8aa552f82669336
* 6.App.cpp: 自动创建未设置的默认group; 设置scr的透明度和颜色; 添加rootStyle
* 7.PageManager: 添加PageBase.cpp; 规范成员变量命名; 添加Replace()方法; 支持设置RootDefaultStyle; 修复PM_Drag不生效
* 8.Pages/_Tempalte: 添加LOG输出
* 9.Pages/Dialplate: 同步PageManager更新
* 10.Pages/LiveMap: 同步PageManager更新
* 11.Pages/Startup: 同步PageManager更新; 使用Replace()方法替换Push()方法，支持BackHome()
* 12.Pages/SystemInfos: 同步PageManager更新; 优化item自动添加AttachEvent()
* 13.ResourcePool: 移除无用的gps_pin图片
* 14.lv_anim_label: lv_anim_label_set_text() -> lv_anim_label_push_text()
* 15.lv_img_png: 添加多色深支持
* 16.USER: 添加benchmark.inc，方便测试lvgl以及对比性能
