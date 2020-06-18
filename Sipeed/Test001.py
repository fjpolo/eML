# Untitled - By: root - Wed. Jun. 10 2020

import sensor, image, time, lcd

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)

clock = time.clock()

while(True):
    clock.tick()
    img = sensor.snapshot()
    #print(clock.fps())
    #img.invert()
    lcd.display(img)
