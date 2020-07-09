#tested with frimware 5-0.22
import sensor,image,lcd
import KPU as kpu

lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224, 224))
#sensor.set_windowing((320, 240))
#sensor.set_vflip(1)
sensor.run(1)
classes = ["penguin"]
print("BSP initialized")
task = kpu.load(0x200000) #change to "/sd/name_of_the_model_file.kmodel" if loading from SD card
print("Task loaded from Flash")
#a = kpu.set_outputs(task, 0, 7,7,30)   #the actual shape needs to match the last layer shape of your model(before Reshape)
#a = kpu.set_outputs(task, 1,30,7,7)
anchor = (0.57273, 0.677385, 1.87446, 2.06253, 3.33843, 5.47434, 7.88282, 3.52778, 9.77052, 9.16828)
a = kpu.init_yolo2(task, 0.3, 0.3, 5, anchor) #tweak the second parameter if you're getting too many false positives
print("Yolo initialized")
while(True):
    #img = sensor.snapshot().rotation_corr(z_rotation=90.0)
    img = sensor.snapshot()
    a = img.pix_to_ai()
    code = kpu.run_yolo2(task, img)
    if code:
        for i in code:
            print("1")
            a=img.draw_rectangle(i.rect(),color = (0, 255, 0))
            a = img.draw_string(i.x(),i.y(), classes[i.classid()], color=(255,0,0), scale=3)
        a = lcd.display(img)
    else:
        print("2")
        a = lcd.display(img)
    #a = lcd.display(img)
a = kpu.deinit(task)

"""
import sensor
import image
import lcd
import KPU as kpu
import time

lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((320, 240))
#sensor.set_vflip(1)
sensor.run(1)
task = kpu.load(0x200000) #使用kfpkg将 kmodel 与 maixpy 固件打包下载到 flash
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
"""
