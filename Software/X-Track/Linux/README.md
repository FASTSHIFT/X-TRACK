# X-TRACK Linux

## 编译

```sh
make -j
```

## 执行

```sh
sudo ./xtrack /dev/input/event0
```

注意这边的`/dev/input/event0`指的是设备上鼠标或者触摸屏event设备节点，不同设备ID是不同的，如果发现无法输入，可以使用`evtest`工具自行查询和调整。

UI目前是直接渲染framebuffer的，尝试按键盘的`CTRL`+`ALT`+ `F1` 到 `F6`按键，切换framebuffer显示即可看到UI。

## 其他说明

* 1.可以通过修改Makefile的`LV_COLOR_DEPTH`配置屏幕的颜色深度，默认32bpp。
* 2.关机自动保存功能不支持。
* 3.轨迹记录功能未测试，可能不支持。
