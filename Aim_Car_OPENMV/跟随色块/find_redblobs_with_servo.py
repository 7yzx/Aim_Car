# Single Color RGB565 Blob Tracking Example
#
# This example shows off single color RGB565 tracking using the OpenMV Cam.

import sensor, image, time, math
from pyb import Servo
threshold_index = 0 # 0 for red, 1 for green, 2 for blue

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds track in general red/green/blue things. You may wish to tune them...
thresholds = [(30, 100, 15, 127, 15, 127), # generic_red_thresholds
              (0, 100, -105, 6, -100, 76), # generic_green_thresholds
              (0, 30, 0, 64, -128, 0)] # generic_blue_thresholds
s1=Servo(1)
s2=Servo(2)
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
sensor.set_vflip(True)
sensor.set_hmirror(True)
clock = time.clock()

err_x=0
err_i=0
err_pid=0
err_h=0
err_x2=0
err_i2=0
err_pid2=0

angle2=0

s1.calibration(700, 2400, 1510, 2500, 2000)
s2.calibration(700, 2400, 1250, 2500, 2000)

def pidx(a):#x方向上的pid控制
    global err_i
    kpx=0.2
    kix=0.02
    err_x1=a-0
    if(err_x1>-5 and err_x1<5):
        err_x1=0

    err_i+=err_x1
    if(err_i>200):
        err_i=200
    if(err_i<-200):
        err_i=-200
    pid=err_x1*kpx+err_i*kix

    if pid>80:
        pid=80
    if pid<-80:
        pid=-80
    return pid

def pidh(b):#x方向上的pid控制
    global err_i2
    kpx2=0.15
    kix2=0.01
    err_x2=b-0
    if(err_x2>-7 and err_x2<7):
        pid1=0

    err_i2+=err_x2
    if(err_i2>200):
        err_i2=200
    if(err_i2<-200):
        err_i2=-200
    pid1=err_x2*kpx2+err_i2*kix2

    if pid1>80:
        pid1=80
    if pid1<-80:
        pid1=-80
    return pid1

# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. "merge=True" merges all overlapping blobs in the image.

while(True):
    clock.tick()
    img = sensor.snapshot()
    for blob in img.find_blobs([thresholds[0]], pixels_threshold=200, area_threshold=150, merge=True):
        # These values depend on the blob not being circular - otherwise they will be shaky.
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255,0,0))
            img.draw_line(blob.major_axis_line(), color=(0,255,0))
            img.draw_line(blob.minor_axis_line(), color=(0,0,255))
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
        err_x=blob.cx()-img.width()/2
        err_h=blob.cy()-img.height()/2
        print("x_offset",err_x)
        print("h_offset",err_h)
        err_pid=pidx(err_x)
        err_pid2=pidh(err_h)
        print("pid_x%d,pid_h:%d, s2angle:%d " %(err_pid, err_pid2 ,s2.angle()))
        print("pulse",s2.pulse_width())
        s1.angle(-err_pid)
        angle2=s2.angle()
        s2.pulse_width(s2.pulse_width()+int(err_pid2))
    #print(clock.fps())
