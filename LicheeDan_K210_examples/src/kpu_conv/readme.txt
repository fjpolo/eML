kernel_conv.bin
made by Sipeed
Demonstrate	how 3x3 convolution kernel influence image
suit for MAIX Bit/Dock/Go.

Usage:
Poweron, press Boot Key to switch convolution kernel.
Kernel value will show on top left corner.
We preset 4 kernels:
0: original output
1: edge detecting
2: sharpen
3: relievo

If you want test your own kernel, you can input via serial(baudrate 115200).
input 0~3 to select preset kernel.
input 9 numbers divide by space to enable your kernel. it support float number.
press enter to end your command.

本固件由Sipeed出品，演示3x3卷积核如何作用于图像，适合MAIX系列开发板。
使用方法：
上电，按boot键切换预置的4种卷积核，卷积核参数会显示在左上角。
0. 原始输出
1. 边缘检测
2. 锐化
3. 浮雕

如果你想测试自己的卷积核，可以通过串口输入（波特率115200）
输入0~3选择预置的卷积核。
输入以空格分割的9个数字来使能你的卷积核，支持浮点数。
按回车来结束你的命令。
