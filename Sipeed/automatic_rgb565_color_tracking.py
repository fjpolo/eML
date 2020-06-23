# Automatic RGB565 Color Tracking Example
#
# This example shows off single color automatic RGB565 color tracking using the OpenMV Cam.

import sensor, image, time, lcd
import KPU as kpu

# Init LCD
lcd.init(freq=15000000)

print("Letting auto algorithms run. Don't put anything in front of the camera!")
img = image.Image()
img.draw_string(2,2, ("Calibrating..."), color=(0,128,0), scale=2)
lcd.display(img)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
#sensor.set_auto_gain(False) # must be turned off for color tracking
#sensor.set_auto_whitebal(False) # must be turned off for color tracking
sensor.run(1)
#
clock = time.clock()

# Capture the color thresholds for whatever was in the center of the image.
r = [(320//2)-(50//2), (240//2)-(50//2), 50, 50] # 50x50 center of QVGA.

print("Auto algorithms done. Hold the object you want to track in front of the camera in the box.")
print("MAKE SURE THE COLOR OF THE OBJECT YOU WANT TO TRACK IS FULLY ENCLOSED BY THE BOX!")
img.draw_string(2,2, ("Place color in box!"), color=(0,128,0), scale=2)
lcd.display(img)
for i in range(60):
    img = sensor.snapshot()
    img.draw_string(2,2, ("Place color in box!"), color=(0,128,0), scale=2)
    img.draw_rectangle(r)
    lcd.display(img)

print("Learning thresholds...")
threshold = [50, 50, 0, 0, 0, 0] # Middle L, A, B values.
for i in range(60):
    img = sensor.snapshot()
    hist = img.get_histogram(roi=r)
    lo = hist.get_percentile(0.01) # Get the CDF of the histogram at the 1% range (ADJUST AS NECESSARY)!
    hi = hist.get_percentile(0.99) # Get the CDF of the histogram at the 99% range (ADJUST AS NECESSARY)!
    # Average in percentile values.
    threshold[0] = (threshold[0] + lo.l_value()) // 2
    threshold[1] = (threshold[1] + hi.l_value()) // 2
    threshold[2] = (threshold[2] + lo.a_value()) // 2
    threshold[3] = (threshold[3] + hi.a_value()) // 2
    threshold[4] = (threshold[4] + lo.b_value()) // 2
    threshold[5] = (threshold[5] + hi.b_value()) // 2
    for blob in img.find_blobs([threshold], pixels_threshold=100, area_threshold=100, merge=True, margin=10):
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        img.draw_rectangle(r)
    img.draw_string(2,2, ("PLearning thresholds..."), color=(0,128,0), scale=2)
    lcd.display(img)

print("Thresholds learned...")
print("Tracking colors...")
lcd.display(img)

while(True):
    clock.tick()
    img = sensor.snapshot()
    for blob in img.find_blobs([threshold], pixels_threshold=100, area_threshold=100, merge=True, margin=10):
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
    print(clock.fps())
    lcd.display(img)

