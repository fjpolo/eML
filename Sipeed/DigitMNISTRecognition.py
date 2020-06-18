import sensor,lcd,image
import KPU as kpu
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((224, 224))    #set to 224x224 input
sensor.set_hmirror(0)               #flip camera
task = kpu.load(0x200000)           #load model from flash address 0x200000
sensor.run(1)
while True:
    img = sensor.snapshot()
    lcd.display(img,oft=(0,0))      #display large picture
    img=img.to_grayscale(1)        #convert to gray
    img=img.resize(28,28)           #resize to mnist input 28x28
    a=img.invert()                 #invert picture as mnist need
    a=img.strech_char(1)           #preprocessing pictures, eliminate dark corner
    lcd.display(img,oft=(224,32))  #display small 28x28 picture
    a=img.pix_to_ai();             #generate data for ai
    fmap=kpu.forward(task,img)     #run neural network model
    plist=fmap[:]                   #get result (10 digit's probability)
    pmax=max(plist)                 #get max probability
    max_index=plist.index(pmax)     #get the digit
    lcd.draw_string(224,0,"%d: %.3f"%(max_index,pmax),lcd.WHITE,lcd.BLACK)  #show result

