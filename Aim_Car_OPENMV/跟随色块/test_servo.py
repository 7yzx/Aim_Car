import sensor, image, time, os, tf, math, uos, gc
from pid import PID
from pyb import Servo,Timer,UART
from pyb import Timer
tick=0#1ms
ms=0
s=0
k=0
flag=False
pan_servo=Servo(1)
s2=Servo(2)
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)
#Timer
def init():
    tim7 = Timer(7, freq=1)#Timer(100ms)
    tim7.callback(systick)
def systick(tim7):
    global s
    s=s+5

s2.calibration(700, 2400, 1250, 2500, 2000)
angle1=0
#Loop
init()
while(True):
    img=sensor.snapshot()
    print(s2.pulse_width())
    if(s==20):
        s=0
    print(s)
    s2.angle(-10+s)
    print("angle",s2.angle())
    #for i in range(50):
    #for i in range(50):
        #s2.angle(-10-i)
    #print(s2.angle())
    #if(s%2==0):
        #flag=True
    #else:
        #flag=False
    #print(s)
    #if(k==0):
        #init()
        #k=1
    #print(flag)
    ##pan_servo.angle(0)
    #if(flag==True):
        #pan_servo.angle(20)
        ##pan_servo.pulse_width(1500)
    #else:
        #pan_servo.angle(-20)
        ##pan_servo.pulse_width(1500)

