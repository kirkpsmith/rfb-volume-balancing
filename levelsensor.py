# this code takes user input to calculate reservoir levels in benchtop flow battery experiments

import cv2
import time
import numpy as np
import json
import datetime
import csv
from urllib.request import urlopen as uo

import timeavg

LOGGING = True

delta_t = 60 # adjust this as necessary


kernel = np.ones((25, 25), np.uint8) # make this larger if droplets are showing up on reservoir walls


reading1 = timeavg.TimeAvg(delta_t)

reading2 = timeavg.TimeAvg(delta_t)

if LOGGING:
    timestamp = datetime.datetime.now().strftime("%m-%d-%Y %H-%M-%S")
    datafile = ('pumps\\levels\\{}.csv').format(timestamp)

    with open(datafile, "a", newline='') as f:
        writer = csv.writer(f, delimiter=",")
        writer.writerow(['Time (seconds since epoch)', 'Left (mL)', 'Right (mL)'])
        f.close()

logTimer = time.perf_counter()

def calculate():
    return reading1.calculate(), reading2.calculate()

def adjust_kernel(new_size):
    kernel = np.ones((new_size, new_size), np.uint8)

def save_picture(path):
    cv2.imwrite(path, frame, [cv2.IMWRITE_JPEG_QUALITY, 90])

vc = cv2.VideoCapture(1)

cv2.namedWindow("feed")
cv2.moveWindow("feed",20,20)

rval, frame = vc.read()
frame = cv2.rotate(frame, cv2.ROTATE_90_CLOCKWISE)


# select two sensing areas on the reservoirs (left, right), and then a rectangle which corresponds to a discrete known volume (ie. graduations on a jar)
# draw the rectange, press enter, repeat three times, then press ESC and follow prompts
r1, r2, h = cv2.selectROIs("feed", frame)


vol = float(input("Enter reference scale volume (mL):")) 
init_vol = float(input("Enter initial volume (total of both reservoirs, mL):"))


scale = vol/h[3] #mL/pixel


height_max = max(r1[1], r2[1])
height_min = min(r1[3], r2[3])

print((height_max-height_min), scale)
r1 = [int(r1[0]), int(height_max), int(r1[2]), int(height_min)]
r2 = [int(r2[0]), int(height_max), int(r2[2]), int(height_min)]
r1_area = r1[2] * r1[3]
r2_area = r2[2] * r2[3]
cv2.destroyAllWindows()

# json.dump({'r1':r1,'r2':r2,'h':h}, open("rects.json", "w"))

i=0

while True:
    rval, frame = vc.read()
    frame = cv2.rotate(frame, cv2.ROTATE_90_CLOCKWISE)
    imCrop1 = frame[height_max:int(height_max +
                                        height_min),
                    int(r1[0]):int(r1[0] + r1[2])]
    imCrop1 = cv2.cvtColor(imCrop1, cv2.COLOR_RGB2GRAY)
    otsu1, thr1 = cv2.threshold(
        imCrop1, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    thr1 = cv2.morphologyEx(
        thr1, cv2.MORPH_CLOSE, kernel)
    thr1 = cv2.GaussianBlur(thr1, (5, 5), 0)


    imCrop2 = frame[height_max:int(height_max +
                                        height_min),
                    int(r2[0]):int(r2[0] + r2[2])]
    imCrop2 = cv2.cvtColor(imCrop2, cv2.COLOR_RGB2GRAY)
    otsu2, thr2 = cv2.threshold(
        imCrop2, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    #ret, thr2 = cv2.threshold(
    #    imCrop2, otsu1, 255, cv2.THRESH_BINARY)
    thr2 = cv2.morphologyEx(
        thr2, cv2.MORPH_CLOSE, kernel)
    thr2 = cv2.GaussianBlur(thr2, (5, 5), 0)
    t = time.time()
    calc1 = scale*(r1_area - cv2.countNonZero(thr1)) * 1.0 / r1[2]
    calc2 = scale*(r2_area - cv2.countNonZero(thr2)) * 1.0 / r2[2]

    # there is some dead volume, both reservoir readings should initially add to initial combined volume
    # init_vol = calc1 + 2*offset1 + calc2
    if(i==300): # offsets are determined after some iterations to allow readings to stabilize from noise
        offset = (init_vol - reading1_short.calculate()-reading2_short.calculate())/2
        reading1_short = timeavg.TimeAvg(delta_t_short)
        reading2_short = timeavg.TimeAvg(delta_t_short)
        reading1 = timeavg.TimeAvg(delta_t)
        reading2 = timeavg.TimeAvg(delta_t)
        print('Offset completed')
    if(i>=300):
        calc1+=offset
        calc2+=offset

    reading1.append([calc1, t])
    reading2.append([calc2, t])
    thr1 = cv2.cvtColor(thr1, cv2.COLOR_GRAY2RGB)
    thr1[np.where((thr1 == [0,0,0]).all(axis = 2))] = [0,33,166]
    thr2 = cv2.cvtColor(thr2, cv2.COLOR_GRAY2RGB)
    thr2[np.where((thr2 == [0,0,0]).all(axis = 2))] = [0,33,166]

    frame[height_max:int(height_max + height_min),
        int(r1[0]):int(r1[0] + r1[2])] = thr1

    frame[height_max:int(height_max + height_min),
        int(r2[0]):int(r2[0] + r2[2])] = thr2
    frame = frame
    cv2.line(frame,(r1[0] + r1[2],height_max),(r2[0],height_max),(255,0,0),2)
    cv2.line(frame,(r1[0] + r1[2],height_max + height_min),(r2[0],height_max + height_min),(255,0,0),2)

    cv2.putText(frame, 'Electrolyte Loss (mL):{: 3.3f}'.format(init_vol - reading2.calculate()-reading1.calculate()), (10,50), cv2.FONT_HERSHEY_SIMPLEX, 
                    1, (255, 0, 0), 2, cv2.LINE_AA)
 
    cv2.imshow("feed", frame)
    #cv2.imshow("r1", thr1)
    #cv2.imshow("r2", thr2)

    print(calculate())
    key = cv2.waitKey(20)
    if key == 27:  # exit on ESC
        break
    if(time.perf_counter()-logTimer > 5):
        timestamp = time.time()

        data = [timestamp, reading1.calculate(), reading2.calculate()]
        logTimer = time.perf_counter()
        with open(datafile, "a", newline='') as f:
            writer = csv.writer(f, delimiter=",")
            writer.writerow(data)
            f.close()

    i+=1

cv2.destroyAllWindows()
vc.release()

