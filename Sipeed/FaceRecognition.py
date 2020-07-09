import sensor,image,lcd, time
import KPU as kpu

lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((320, 240))
#sensor.set_vflip(1)
sensor.run(1)
classes = ["person"]
task = kpu.load(0x200000) #change to "/sd/name_of_the_model_file.kmodel" if loading from SD card
#a = kpu.set_outputs(task, 0, 7,7,30)   #the actual shape needs to match the last layer shape of your model(before Reshape)
anchor = (0.57273, 0.677385, 1.87446, 2.06253, 3.33843, 5.47434, 7.88282, 3.52778, 9.77052, 9.16828)
a = kpu.init_yolo2(task, 0.3, 0.3, 5, anchor) #tweak the second parameter if you're getting too many false positives
clock = time.clock()
while(True):
    clock.tick()
    img = sensor.snapshot()
    a = img.pix_to_ai()
    code = kpu.run_yolo2(task, img)
    if code:
        for i in code:
            a=img.draw_rectangle(i.rect(),color = (0, 255, 0))
            fps = clock.fps()
            img.draw_string(2,2, ("%2.1ffps" % (fps)), color=(0,128,0), scale = 2)
            a = img.draw_string(i.x(),i.y(), classes[i.classid()], color=(255,0,0), scale=3)
        a = lcd.display(img)
    else:
        fps = clock.fps()
        img.draw_string(2,2, ("%2.1ffps" % (fps)), color=(0,128,0), scale = 2)
        a = lcd.display(img)
a = kpu.deinit(task)
