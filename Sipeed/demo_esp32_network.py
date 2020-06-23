import network
from Maix import GPIO
from fpioa_manager import fm, board_info

WIFI_SSID = "Death Star"
WIFI_PASSWD = "D0ct0rWh0&Th3T@rd15"

# IO map for ESP32 on Maixduino
fm.register(25,fm.fpioa.GPIOHS10)   #cs
fm.register(8,fm.fpioa.GPIOHS11)#rst
fm.register(9,fm.fpioa.GPIOHS12)#rdy
fm.register(28,fm.fpioa.GPIOHS13)#mosi
fm.register(26,fm.fpioa.GPIOHS14)#miso
fm.register(27,fm.fpioa.GPIOHS15)#sclk

print("Configuring ESP32...")
nic = network.ESP32_SPI(cs=fm.fpioa.GPIOHS10,rst=fm.fpioa.GPIOHS11,rdy=fm.fpioa.GPIOHS12, mosi=fm.fpioa.GPIOHS13,miso=fm.fpioa.GPIOHS14,sclk=fm.fpioa.GPIOHS15)
print("Connecting to SSID...")
nic.connect(WIFI_SSID, WIFI_PASSWD)
print("Connected!")

print(nic.ifconfig())
print(nic.isconnected())
print("ping baidu.com:", nic.ping("baidu.com"), "ms")
nic.disconnect()




