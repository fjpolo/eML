import sensor
import image
import lcd
import KPU as kpu

lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((320, 240))
#sensor.set_vflip(1)
sensor.run(1)
task = kpu.load(0x300000) #使用kfpkg将 kmodel 与 maixpy 固件打包下载到 flash
anchor = (1.889, 2.5245, 2.9465, 3.94056, 3.99987, 5.3658, 5.155437, 6.92275, 6.718375, 9.01025)
a = kpu.init_yolo2(task, 0.5, 0.3, 5, anchor)
clock = time.clock()
while(True):
    clock.tick()
    img = sensor.snapshot()
    img.resize(416,416)
    code = kpu.run_yolo2(task, img)
    if code:
        fps = clock.fps()
        img.draw_string(2,2, ("%2.1ffps" % (fps)), color=(0,128,0), scale = 4)
        for i in code:
            print(i)
            a = img.draw_rectangle(i.rect())
    a = lcd.display(img)
    print (clock.fps())
a = kpu.deinit(task)
