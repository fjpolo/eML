# Image Histogram Info Example
#
# This script computes the histogram of the image and prints it out.

import sensor, image, time, lcd
import KPU as kpu

# Init LCD
lcd.init(freq=15000000)

sensor.reset()
sensor.set_pixformat(sensor.RGB565) #GRAYSCALE or RGB565.
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
sensor.run(1)
#
clock = time.clock()

while(True):
    clock.tick()
    img = sensor.snapshot()
    # Gets the grayscale histogram for the image into 8 bins.
    # Bins defaults to 256 and may be between 2 and 256.
    print(img.get_histogram(bins=8))
    print(clock.fps())
    lcd.display(img)

# You can also pass get_histogram() an "roi=" to get just the histogram of that area.
# get_histogram() allows you to quickly determine the color channel information of
# any any area in the image.
