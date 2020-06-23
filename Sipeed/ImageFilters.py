#
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

# Light removal thresholds
thresholds = (90, 100, -128, 127, -128, 127)

while(True):
    clock.tick()
    img = sensor.snapshot()
    #img = img.binary([thresholds], invert=False, zero=True) # Light removal
    #img = img.histeq()                                  # Histogram equalizer
    #img = img.gaussian(1)                              # Blur
    #img = img.lens_corr(strength = 1.8, zoom = 1.0)    # Lens correction
    #img = img.laplacian(1, sharpen=True)                # Sharpen
    #img = img.gamma_corr(gamma = 0.5, contrast = 1.0, brightness = 0.0)     # Gamma correction
    #img.negate()                                   # Negative
    #img.laplacian(1)                              # Edge detection


    # The only argument is the kernel size. N coresponds to a ((N*2)+1)^2
    # kernel size. E.g. 1 == 3x3 kernel, 2 == 5x5 kernel, etc. Note: You
    # shouldn't ever need to use a value bigger than 2.
    #img.mean(1)                               # Mean filter


    # The first argument to the median filter is the kernel size, it can be
    # either 0, 1, or 2 for a 1x1, 3x3, or 5x5 kernel respectively. The second
    # argument "percentile" is the percentile number to choose from the NxN
    # neighborhood. 0.5 is the median, 0.25 is the lower quartile, and 0.75
    # would be the upper quartile.
    #img.median(1, percentile=0.5)               # Median filter

    # The only argument to the median filter is the kernel size, it can be
    # either 0, 1, or 2 for a 1x1, 3x3, or 5x5 kernel respectively.
    #img.mode(3)         # Mode filter

    # Print FPS
    #print(clock.fps())
    fps = clock.fps()
    img.draw_string(2,2, ("%2.1ffps" %(fps)), color=(0,128,0), scale=2)

    # LCD
    lcd.display(img.resize(320,240))
