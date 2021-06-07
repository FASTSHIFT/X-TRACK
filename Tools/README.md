# 地图下载说明

* 在MapDownloadGen文件夹里打开maptile.exe执行文件，打开后根据你自己所在地区勾选范围（注意范围不要选太大，否则后续下载会等很久），鼠标左键单击勾选范围，选定后双击左键。点击确认后将地图保存在 X-TRACK 目录下。

https://user-images.githubusercontent.com/33819388/120956796-f0893100-c786-11eb-9e0b-aac6e733e459.mp4

* 打开Downloader.exe执行文件，将上一步保存的MapInfos.xmp拖入程序里，点击Download

https://user-images.githubusercontent.com/33819388/120956786-ea935000-c786-11eb-9e22-5ec11aec1194.mp4



### ＳＴＡＴＵＳ提示信息

|  状态       | 含义        |
|  ----     | ----         |
|   S:E     |   文件已存在                   |
|   S:F     |   文件保存失败                  |
|   C:数字    |   网络问题，数字为HTTP状态码     |
|   L:0     |   下载下来的文件长度为零         |


* 下载完成后，再使用Tools文件夹下的MapConv.bat和RenameBin.bat脚本转化地图格式，上述步骤无误后，便可将MAP文件夹拷贝到SD卡里


   