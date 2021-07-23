## 编译
* MCU固件使用Keil v5.25或以上的版本进行编译，并安装[雅特力](https://www.arterytek.com/cn/index.jsp)官方PACK->[AT32F4xx标准外设库](http://www.arterytek.com/download/Pack_Keil_AT32F4xx_CH_V1.3.4.zip)
* 模拟器使用Visual Studio 2019编译，配置为**Release x86**

## 硬件修改
* 去除**D4**二极管，防止充电时自动开机，导致充电变慢和发热
* GPS模块需要由**HT1818Z3G5L**更换为**ATGM336H**，否则外壳放不下
* 地磁计**LIS3MDL**和加速度计**LSM6DSM**在该系统中作用不大，又由于芯片涨价导致整机成本过高，可以不安装，但需要在源代码的**Config.h**中的**CONFIG_SENSOR_ENABLE**宏开关设置0，并重新编译固件
* MCU: AT32F403ACGU7如果定不到货，可以尝试[@johnsbark](https://github.com/johnsbark)提供的[改进版硬件](https://github.com/FASTSHIFT/X-TRACK/tree/main/Hardware/X-TRACK%20%E6%94%B9%E8%BF%9B%E7%89%88)

## 特殊器件购买链接
* 电池 (选择 683030) https://detail.tmall.com/item.htm?id=634173891714&spm=a1z09.2.0.0.55112e8dkTgWKa&_u=h33s833p8189
* 旋转编码器 https://item.taobao.com/item.htm?spm=a1z10.3-c.w4002-12260322724.9.731a6469iG1L2z&id=598028191617
* 屏幕 (选择 裸屏) https://item.taobao.com/item.htm?spm=a1z09.2.0.0.55112e8dkTgWKa&id=600653923469&_u=h33s833p5d15
* 调试口 (选择 4P弯针) https://item.taobao.com/item.htm?spm=a1z09.2.0.0.55112e8dkTgWKa&id=522574977704&_u=h33s833p394d
* 码表底座 (选择 竖向按装拓展片孔距M3*20~21mm[塑胶材料 3g]) https://item.taobao.com/item.htm?spm=a1z09.2.0.0.67002e8dpTpXY9&id=599529362799&_u=h33s833p5dc8
* Pad (非实体元件，只是焊接电池线的焊盘)

## 焊接建议
* 不建议新手尝试自己焊接
* SD卡座、旋转编码器、蜂鸣器建议用细尖头烙铁焊接，不容易烫坏元件
