# X-TRACK_LQFP
感谢@FASTSHIFT的开源项目X-TRACK
鉴于问遍了淘宝都没有AT32F403ACGU7，为了白嫖强行更换封装

X-TRACK的LQFP封装版本，将原先的AT32F403ACGU7更换为AT32F403ACGT7（即STM32F103C8T6那个最常见的48-pin LQFP 7X7封装） 重新布线过，第一版亲测正常工作，之后又优化了部分布线，减少焊接时可能出现的问题
![1626970879579 jpg](https://user-images.githubusercontent.com/22606145/126675080-a4306fdd-40eb-498b-b788-4767517e41be.jpg)
![9F54E02A8FF0067CA164CD360D0CCE11](https://user-images.githubusercontent.com/22606145/126675325-b29ca6bf-5b27-42a8-be5b-b546829c5d8c.jpg)
![2DA712428BFF5B875828A8FB9A96AAC1](https://user-images.githubusercontent.com/22606145/126675353-4889a1bf-c2f8-4c87-9307-81280d53ea30.jpg)
![QQ图片20210723003431](https://user-images.githubusercontent.com/22606145/126675481-624ab595-de1d-410d-9447-8f4ed1dc7945.jpg)

焊接过程中极易出现短路等故障，需要重点注意的有：  
* 1.风枪必备，有锡浆更好。焊接注意顺序，背面不建议上风枪，手焊足以  
* 2.TF卡位置注意引脚焊接短路问题（用锡浆的要注意残留问题，容易引起短路），有问题的话插TF卡会低沉的发出滴滴两声，成功的话则是有一段插入音效  
* 3.编码器要焊牢！要焊牢！要焊牢！此板子大面积的铺铜带来的缺点就是焊接时温度上不去会造成虚焊，“尤其是用锡浆的”，我调试的时候掰下来了，干坏个焊盘，现版本编码器处的焊盘以进行加粗等优化处理。  
* 4.加速度计可以用低配版LSM6DS3代，因为引脚寄存器都一样，只是功耗高一倍，(买模块要便宜些大概16元——优信电子，磁罗盘在CJMCU长江智动58元)  
