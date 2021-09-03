## 编译说明
* MCU固件: 务必使用**Keil v5.25**或以上的版本进行编译（因为旧编译器不能完全支持**C++ 11**的语法）,并安装[雅特力](https://www.arterytek.com/cn/index.jsp)官方PACK->[AT32F4xx标准外设库](http://www.arterytek.com/download/Pack_Keil_AT32F4xx_CH_V1.3.7.zip)。
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
  "sysConfig.mapWGS84": 0                      //坐标系统配置(0:GCJ02, 1:WGS84)
}
```
