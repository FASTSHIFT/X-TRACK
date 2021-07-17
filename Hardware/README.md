## 硬件修改
* 去除**D4**二极管，防止充电时自动开机，导致充电变慢和发热。
* GPS模块需要由**HT1818Z3G5L**更换为**ATGM336H**，否则外壳放不下。
* 地磁计**LIS3MDL**和加速度计**LSM6DSM**在该系统中作用不大，又由于芯片涨价会导致整机成本过高，可以不安装。但需要在源代码中屏蔽相关函数重新编译。

## 特殊器件购买链接
* 电池 (选择 683030) https://detail.tmall.com/item.htm?id=634173891714&spm=a1z09.2.0.0.55112e8dkTgWKa&_u=h33s833p8189
* 旋转编码器 https://item.taobao.com/item.htm?spm=a1z10.3-c.w4002-12260322724.9.731a6469iG1L2z&id=598028191617
* 屏幕 (选择 裸屏) https://item.taobao.com/item.htm?spm=a1z09.2.0.0.55112e8dkTgWKa&id=600653923469&_u=h33s833p5d15