## 编译说明
* MCU固件: 务必使用Keil v5.25或以上的版本进行编译（因为旧编译器不能完全识别**C++ 11**的语法）,并安装[雅特力](https://www.arterytek.com/cn/index.jsp)官方PACK->[AT32F4xx标准外设库](http://www.arterytek.com/download/Pack_Keil_AT32F4xx_CH_V1.3.4.zip)
* VS模拟器: 使用Visual Studio 2019编译，配置为**Release x86**。下载地图生成的的**MAP**文件夹和**X-Track**文件夹放在同一个目录下，即可在模拟器中读取地图。录制的GPX轨迹文件也可以放在这个目录内，需要在Config.h里修改```CONFIG_TRACK_VIRTUAL_GPX_FILE_PATH```宏定义指定被读取的GPX文件路径。
