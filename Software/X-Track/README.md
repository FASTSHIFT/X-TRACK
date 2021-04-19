# X-TRACK 更新日志
> https://github.com/FASTSHIFT/X-TRACK

## [v0.1] - 2021-3-21
* 1.框架搭建完成，全新系统架构
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
