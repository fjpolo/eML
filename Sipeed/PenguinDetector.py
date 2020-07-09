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
#task = kpu.load(0x300000) #change to "/sd/name_of_the_model_file.kmodel" if loading from SD card
#Update after 02.2020
model_addr = 0x300000
batch_size = 0x10000
spi_speed = 80000000
task=kpu.load_flash(model_addr,1, batch_size, spi_speed)
a = kpu.set_outputs(task, 0, 7,7,30)   #the actual shape needs to match the last layer shape of your model(before Reshape)
anchor = (0.57273, 0.677385, 1.87446, 2.06253, 3.33843, 5.47434, 7.88282, 3.52778, 9.77052, 9.16828)
a = kpu.init_yolo2(task, 0.3, 0.3, 5, anchor) #tweak the second parameter if you're getting too many false positives
while(True):
    img = sensor.snapshot()
    a = img.pix_to_ai()
    code = kpu.run_yolo2(task, img)
    if code:
        for i in code:
            a=img.draw_rectangle(i.rect(),color = (0, 255, 0))
            a = img.draw_string(i.x(),i.y(), classes[i.classid()], color=(255,0,0), scale=2)
            a = img.draw_string(i.x()+4,i.y()+4, code.], color=(255,0,0), scale=2)
        a = lcd.display(img)
    else:
        a = lcd.display(img)
a = kpu.deinit(task)
