# X-TRACK
> 开源GPS自行车码表。
> 
> 拥有可显示实时位置的离线地图。
> 
> 支持记录轨迹以及导出标准GPX格式的轨迹文件。
> 
> 全新设计的[页面调度](https://github.com/FASTSHIFT/X-TRACK/tree/main/Software/X-Track/USER/App/Utils/PageManager)和[消息框架](https://github.com/FASTSHIFT/X-TRACK/tree/main/Software/X-Track/USER/App/Utils/DataCenter)。

![image](https://github.com/FASTSHIFT/X-TRACK/blob/main/Images/%E9%AA%91%E8%A1%8C%E6%B5%8B%E8%AF%95.jpg)

## GUI
> [LVGL V8.0.0](https://github.com/lvgl/lvgl/releases/tag/v8.0.0)

## 硬件配置
* 1.主控: AT32F403ACGU7 (主频:240MHz RAM:224KB ROM:1MB)
* 2.屏幕: ST7789 IPS 1.54inch SPI接口 240x240分辨率 60Hz刷新率
* 3.输入设备: 旋转编码器
* 4.RTC: MCU内置RTC时钟
* 5.加速度计: LSM6DSM (支持硬件计步输出)
* 6.地磁计: LIS3MDL
* 7.GPS: ATGM336H (BDS + GPS + GLONASS + GALILEO + QZSS + SBAS)
* 8.电池: Li-ion 3.7V 683030 700mAh
* 9.电源管理: LP5904-3.3 + MCP73831
* 10.外壳: 3D打印 光固化

## 功能
* 1.支持速度、距离、时间、卡路里、航向显示
* 2.拥有**离线地图**，支持显示实时位置，支持缩放
* 3.支持计步
* 4.支持经纬度、海拔显示
* 5.支持RTC自动根据GPS校准
* 6.支持记录轨迹，可导出[GPX](https://zh.wikipedia.org/wiki/GPX)格式的文件
* 7.支持掉电自动保存数据(JSON格式文件)
* 8.4小时续航 (持续工作，始终亮屏)
* 9.待续...

## 实物演示
https://user-images.githubusercontent.com/26767803/120889722-1f8d8e80-c631-11eb-8294-df79f151dedb.mp4

