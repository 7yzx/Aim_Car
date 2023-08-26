import sensor, image, time, os, tf, math, uos, gc ,pyb
from pid import PID
from pyb import Servo,Timer,UART
import struct,ustruct
from image import SEARCH_EX, SEARCH_DS

led1 = pyb.LED(1)
led2 = pyb.LED(2)
led3 = pyb.LED(3)


#从imgae模块引入SEARCH_EX和SEARCH_DS。使用from import仅仅引入SEARCH_EX,
#SEARCH_DS两个需要的部分，而不把image模块全部引入。
uart = UART(3, 115200)
pan_servo=Servo(1)
pan_pid = PID(p=-0.15, i=0, imax=90) #脱机运行或者禁用图像传输，使用这个PID

# Reset sensor
sensor.reset()

# Set sensor settings
# Max resolution for template matching with SEARCH_EX is QQVGA
sensor.set_framesize(sensor.QQVGA) #推荐QQVGA，能提高识别速度

# You can set windowing to reduce the search image.
#sensor.set_windowing(((640-80)//2, (480-60)//2, 80, 60))
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_windowing((240, 240))       # Set 240x240 window.

sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

sensor.skip_frames(time=2000)          # Let the camera adjust.
sensor.set_vflip(True)
sensor.set_hmirror(True)

net = tf.load("trained.tflite", load_to_fb=uos.stat('trained.tflite')[6] > (gc.mem_free() - (64*1024)))
labels = [line.rstrip('\n') for line in open("labels.txt")]

# Load template.
# Template should be a small (eg. 32x32 pixels) grayscale image.
######还未更改
template1 = image.Image("/results/1a.pgm")
template2 = image.Image("/results/2a.pgm")
template3 = image.Image("/results/3a.pgm")
template4 = image.Image("/results/4a.pgm")
template5 = image.Image("/results/a1.pgm")
template6 = image.Image("/results/b1.pgm")
template7 = image.Image("/results/c1.pgm")
template8 = image.Image("/results/d1.pgm")
####################
get_target=0
circle1=1  #第一个循环
circle2=0
flag_432=0


min_confidence = 0.85 #可信度
center_x=[0,0,0,0,0,0,0,0,0]#background 1234ABCD的x坐标
center_y=[0,0,0,0,0,0,0,0,0]#y坐标

get_x=0#识别时的x坐标
get_y=0
count=0
flag_thing=[0,0,0,0,0,0,0,0,0]#是否识别到，*****[i]12345678对应1234ABCD 0不管，对应ground******
flag_getthing=0 #检测的目标
flag_getthing_last=0#上一次的检测目标
flag_getthing_num=0#识别的次数达到50次之后才认为有效
flag_stop=0#传输的停止标志

routation=0
start_width=0#起始值，消除误差
start_angle=0#起始值
pan_output=0

get_uart=[0,0,0]#得到的串口数据
rxcount=0
rxstate=0

flag_move_num=0
flag_move=0

flag_move_r=0
flag_move_f=0
OpenMV_move=0

init1=0  #启动定时器一次
Count1=0
Count2=0

flag_re=0
flag_f=0
errinte=0
send_num=0
mission=0
#1234ABCD位置，1代表左边，8代表右边。通过大小判断往哪边转
position=[0,3,4,5,6,0,0,0,0]
position_num=0#识别到左边的数量，当数量大于2时，其他的默认为右边
colors = [ # Add more colors if you are detecting more than 7 types of classes at once.
    (255,   0,   0),
    (  0, 255,   0),
    (255, 255,   0),
    (  0,   0, 255),
    (255,   0, 255),
    (  0, 255, 255),
    (255, 255, 255),
    (0,0,0),
    (0,0,0),
]


def receive_data():
    global rxstate,rxcount
    if uart.any():  #进行串口数据的接收
        res=uart.read(6)  #表示为读取一个十六进制数,这里的uart必须是例化的
        res = struct.unpack('<bbbbbb', res)
        if(res[0]==0x2C and res[1]==0x12 and res[5]==0x5B):
            for i in range(3):
                get_uart[i]=res[i+2]
            print(get_uart)

def receive_data1():
    if uart.any():
        rx=uart.read(1)
        flag_432=rx[0]

#函数初始化，1：find标志 2：move一些 4：thing
def sending_data(data1,data2,data3):
    global uart
    data = ustruct.pack("<bbbbbb",      #格式为俩个字符俩个短整型(2字节)
                   0x2C,                      #帧头1
                   0x12,                      #帧头2
                   data1, # up sample by 4   #数据1
                   data2, # up sample by 4    #数据2
                   data3,
                   0x5B)
    uart.write(data)   #必须要传入一个字节数组
def change1(string):
    if string=='1':
        m=1
    elif string=='2':
        m=2
    elif string=='3':
        m=3
    elif string=='4':
        m=4
    elif string=='A':
        m=5
    elif string=='B':
        m=6
    elif string=='C':
        m=7
    elif string=='D':
        m=8
    else:
        m=0
    return m






#加载模板图片

clock = time.clock()

# Run template matching
while (True):
    clock.tick()
    img = sensor.snapshot()
    #第一个循环
    if(circle1==1):                          #开始模板匹配识别手持数字
        flag_re=1
        led2.off()
        led1.on()
        #clock.tick()
        get_target=0
        #img = sensor.snapshot()
        r1 = img.find_template(template1, 0.70, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
        if 1:
            #img.draw_rectangle(r1)
            get_target=1
            circle1=0
        r2 = img.find_template(template2, 0.7, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
        if r2:
            img.draw_rectangle(r2)
            get_target=2
            circle1=0
        r3 = img.find_template(template3, 0.7, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
        if r3:
            img.draw_rectangle(r3)
            print('3') #打印模板名字
            get_target=3
            circle1=0
        r4 = img.find_template(template4, 0.7, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
        if r4:
            img.draw_rectangle(r4)
            print('4') #打印模板名字
            get_target=4
            circle1=0
        r5 = img.find_template(template5, 0.7, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
        if r5:
            img.draw_rectangle(r5)
            print('5') #打印模板名字
            get_target=5
            circle1=0
        r6 = img.find_template(template6, 0.7, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
        if r6:
            img.draw_rectangle(r6)
            print('6') #打印模板名字
            get_target=6
            circle1=0
        r7 = img.find_template(template7, 0.7, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
        if r7:
            img.draw_rectangle(r7)
            print('7') #打印模板名字
            get_target=7
            circle1=0
        r8 = img.find_template(template8, 0.7, step=4, search=SEARCH_EX) #, roi=(10, 0, 60, 60))
        if r8:
            img.draw_rectangle(r8)
            print('8') #打印模板名字
            get_target=8
            circle1=0

        if(circle1==0 and get_target!=0):#满足条件跳出
            print(get_target)
            flag_getthing=get_target
            circle2=1
#进入第二个追随的循环了

    #if(get_target==0 or (flag_432==0x51 or flag_432==0x10)):
        #circle2=0
        #flag_getthing=get_target
        #circle1=1
        #print("thething",flag_getthing)
    #else:
        #circle2=1
        #circle1=0
    if(flag_re==1 and circle2==1):
        for i in range(10):
            sending_data(get_target,1,1)
            print("send")
    while(circle2==1):
        led2.on()
        led1.off()
        if uart.any():
            flag_432=uart.read(1)
            flag_432 = struct.unpack('<b', flag_432)
            if(flag_432[0]==0x51):
                circle1=1
                circle2=0
                print("51",51)
            print("flag_432",flag_432)

            break
        clock.tick()
        img = sensor.snapshot()
        for i, detection_list in enumerate(net.detect(img, thresholds=[(math.ceil(min_confidence * 255), 255)])):
            if (i == 0): continue # background class
            if (len(detection_list) == 0): continue # no detections for this class?
            #print('list',detection_list)
            #print("********** %s **********" % labels[i])
            for d in detection_list:
                [x, y, w, h] = d.rect()
                thing=change1(labels[i])
                if thing!=0:
                    flag_thing[thing]=1     #识别到
                    center_x[thing] = math.floor(x + (w / 2))#识别到的每一个的位置都记录下来
                    center_y[thing] = math.floor(y + (h / 2))
                    #print('x %d\ty %d' % (center_x[thing], center_y[thing]))
                    img.draw_string(center_x[thing], center_y[thing], ("%s" %(labels[i])), color=(0,0,0), scale=5)
                    img.draw_circle((center_x[thing], center_y[thing], 5), color=colors[i], thickness=2)
                    count+=1
                    #print("count:",count)#测试进入了多少个for
                    if(center_x[flag_getthing]!=0 and flag_thing[flag_getthing]==1):
                        break
        if center_x[flag_getthing]!=0 and flag_thing[flag_getthing]==1:    #当目标位置得到，并且识别到的时候，转向目标
            #if(send_num<10):
                #sending_data(1,1,flag_getthing_last)
                #send_num=0
            #send_num+=1

            print('x %d\ty %d' % (center_x[flag_getthing], center_y[flag_getthing]))
            pan_error = center_x[flag_getthing]-img.width()/2#获取的坐标
            errlast=pan_error
            errinte+=pan_error
            if(errinte>100):
                errinte=100
            if(errinte<-100):
                errinte=-100
            kp=2
            ki=0.01
            pan_output=kp*pan_error+ki*errinte
            print("err:%d  ,output%d,angle,%d"  %(pan_error,pan_output,pan_servo.angle()))
            if(pan_output<6 and pan_output>-6):
                pan_servo.angle(0)
                pan_servo.pulse_width(1500)
                pan_output=0
            elif(pan_output>160 or pan_output<-160):
                #pan_servo.angle(-pan_output/5)#180
                pan_servo.angle(0)
                pan_output=0
                pan_servo.pulse_width(1500)
            else:
                if(pan_output>135 and pan_output<-135):
                    pan_servo.angle(-pan_output/20)
                elif(pan_output>60 and pan_output<-60):
                    pan_servo.angle(-pan_output/10)#180
                else:
                    pan_servo.angle(-pan_output/3)#180


    print("1%d 2%d" %(circle1,circle2))

