import serial, time, os
from struct import *
import pandas as pd
import random

# Data collection with arduino uno R3 is 700 rows every 2.37 seconds
# 700 rows -> 1400 samples -> 590sps / 295sps each channel

# globals ########################
g_iter = 0
file_iter = 0
label_iter = 0
sample_data = []
timer = time.time()

# Number of frames before saving (~110ms per frame)
INTERVAL = 700

CHANNELS = ['ch1', 'ch2']
CH_DATA = {ch: [] for ch in CHANNELS}
NB_CHANNELS = len(CH_DATA.keys())

LABELS = ["OneFinger", "TwoFinger", "fourFinger"]
LABEL_COUNT = {label:0 for label in LABELS}
NB_LABELS = len(LABELS)

# Pull an integer value from serial.
def getValueFromSerial():
    serial_line = ser.readline().split(b'\r')[0]
    value = ''.join(chr(i) for i in serial_line)
    data = value.split(',')
    for x in range(NB_CHANNELS):                            # 'i'th set, 'x'th channel * LSB Microvolt value
        CH_DATA['ch{}'.format(x + 1)].append(round(float(data[x]) * 7.8125, 2))
   # return value

def saveDatafromSerial():
    print("ch1,ch2")
    #value = getValueFromSerial()
    while 1:
        global label_iter
        global LABEL_COUNT
        global CH_DATA
        global file_iter
        global g_iter
        global timer

        label = LABELS[label_iter]
        # A new sample set starts at every INERVAL number of frames.
        if g_iter % INTERVAL == 0:
            print("\u001b[3" + str(label_iter+1) + "m")
            print("Frame start: {} - {}  ".format(label, LABEL_COUNT[label]))
        # Get a new value and push it into the history, pop one off history to prevent our list from growing
        getValueFromSerial()
        g_iter += 1
        #print(value)
        if g_iter % INTERVAL == 0:
            df = pd.DataFrame(CH_DATA)
            df.to_csv("data/{}/{}.txt".format(label, str(LABEL_COUNT[label])), ",")
            CH_DATA = {ch: [] for ch in CHANNELS}
            print("Produced csv no: {} lable: {}".format(file_iter + 1, label))
            LABEL_COUNT[label] += 1
            file_iter += 1
            label_iter = random.randint(0, NB_LABELS-1)

# connect to serial port
ser = serial.Serial('/dev/cu.usbmodem14201', 115200)

# create a list for our average
history = []

# Timer for recording length of time between spikes.
timer = time.time()

saveDatafromSerial()

ser.close() # Only executes once the loop exits
