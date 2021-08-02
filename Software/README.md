## 编译说明
* MCU固件: 务必使用**Keil v5.25**或以上的版本进行编译（因为旧编译器不能完全识别**C++ 11**的语法）,并安装[雅特力](https://www.arterytek.com/cn/index.jsp)官方PACK->[AT32F4xx标准外设库](http://www.arterytek.com/download/Pack_Keil_AT32F4xx_CH_V1.3.4.zip)。
* VS模拟器: 使用**Visual Studio 2019**编译，配置为**Release x86**。将地图下载器生成的**MAP**文件夹和**X-Track**文件夹放在同一个目录下，即可在模拟器中读取地图。录制的GPX轨迹文件也可以放在这个目录内，需要在Config.h里修改```CONFIG_TRACK_VIRTUAL_GPX_FILE_PATH```宏定义指定被读取的GPX文件路径。

## 系统配置文件
系统会在根目录下自动生成`SystemSave.json`的文件，用于储存和配置系统参数
```C
{
  "sportStatus.totalDistance": 0,              //总里程(m)
  "sportStatus.totalTimeUINT32[0]": 0,         //总行驶时间(ms),低32位
  "sportStatus.totalTimeUINT32[1]": 0,         //总行驶时间(ms),高32位
  "sportStatus.speedMaxKph": 0,                //最高时速(km/h)
  "sportStatus.weight": 65,                    //体重(kg)
  "GMT_OffsetHours": 8,                        //时区(GMT+)
  "sysConfig.soundEnable": 1,                  //系统提示音使能
  "sysConfig.longitudeDefault": 116.391332,    //默认位置(经度)
  "sysConfig.latitudeDefault": 39.907415,      //默认位置(纬度)
  "sysConfig.language": "en-GB",               //语言(尚不支持多语言切换)
  "sysConfig.mapSource": "Bing"                //地图源(Bing/OSM)
}
```
