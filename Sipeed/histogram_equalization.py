# Histogram Equalization
#
# This example shows off how to use histogram equalization to improve
# the contrast in the image.

import sensor, image, time, lcd

# Init LCD
lcd.init(freq=15000000)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
#sensor.set_windowing((128, 96))    #set to 224x224 input
sensor.skip_frames(time = 2000)
sensor.run(1)
#
clock = time.clock()

while(True):
    clock.tick()
    #img = sensor.snapshot()
    img = sensor.snapshot().histeq()
    #img = img.gaussian(1)           # Blur
    print(clock.fps())
    lcd.display(img.resize(320,240))
