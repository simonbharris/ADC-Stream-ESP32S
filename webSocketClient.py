import argparse, time, os
import websocket
import pandas as pd
import random
from struct import *

try:
    import thread
except ImportError:
    import _thread as thread
import time

# globals ########################
g_iter = 0
file_iter = 0
timer = 0
label_iter = 0
sample_data = []
setNo = 0

# Number of frames before saving (~110ms per frame)
INTERVAL = 12

CHANNELS = ['ch1', 'ch2', "ch3", "ch4", "ch5", "ch6", "ch7", "ch8"]
CH_DATA = {ch: [] for ch in CHANNELS}
NB_CHANNELS = len(CH_DATA.keys())

#        |   Fireball      |    Shield    |      Bolt
LABELS = ["5-Finger-Spread", "Tucked-Fist", "2-finger-Point"]
LABEL_COUNT = {label:0 for label in LABELS}
NB_LABELS = len(LABELS)

INPUT_MULTIPLIER = 1 #//7.8125

# Collects data from incoming message.
def collectData(message):
    global CH_DATA

    # For each channel, pull one 2-byte integer out of the message, Keep reading the message until the end.
    # i tracks the start position for the set of channels.
    # x tracks which channel we are reading from.
    for i in range(0, len(message), NB_CHANNELS * 2):
        for x in range(NB_CHANNELS):                            # 'i'th set, 'x'th channel * LSB Microvolt value
            CH_DATA['ch{}'.format(x + 1)].append(round(unpack("h", message[i+(x*2):i+(x*2)+2])[0] * INPUT_MULTIPLIER, 2))

# What to do when we receive a frame.
def on_message(ws, message):
    global g_iter
    global label_iter
    global LABEL_COUNT
    global CH_DATA
    global file_iter
    global timer

    label = LABELS[label_iter]
    # A new sample set starts at every INERVAL number of frames.
    if g_iter % INTERVAL == 0:
        print("\u001b[3{}m".format(label_iter+1))
        print("Frame start: {} - {}  ".format(label, LABEL_COUNT[label]))
        timer = time.time()
	# We receive the data in binary, so we must unpack it. Specifying the array elementa [0] drops the tuple so it formats better.
    collectData(message)
    g_iter += 1

    # At every INTERVAL number of frames, save to file.
    if g_iter % INTERVAL == 0:
        df = pd.DataFrame(CH_DATA)
        df.to_csv("data/{}/{}{}.txt".format(label, setNo, str(LABEL_COUNT[label])), ",")
        CH_DATA = {ch: [] for ch in CHANNELS}
        print("Produced csv no: {} lable: {}".format(file_iter + 1, label))
        LABEL_COUNT[label] += 1
        file_iter += 1
        label_iter = random.randint(0, NB_LABELS-1)
        print(round(time.time() - timer, 5))

def on_error(ws, error):
    print(error)

# What to do when we loose connection. (Prints total frames received.)
def on_close(ws):
    print("### closed ###")
    print(f"Num.Frames: {g_iter}")

# Once we successfully connect, keep the connection open until we close the program.
def on_open(ws):
	def run(*args):
		print(",ch1, ch2")
	
	thread.start_new_thread(run, ())


if __name__ == "__main__":
    websocket.enableTrace(True)
    # ESP32 server is hosted on 192.168.4.1:80 when connected to it's access point.
    parser = argparse.ArgumentParser()
    parser.add_argument("--setNo",
                        default="0", 
                        help="append setNo to filenames")
    args = parser.parse_args()

    setNo = args.setNo
    ws = websocket.WebSocketApp("ws://192.168.4.1:80",
                              on_message = on_message,
                              on_error = on_error,
                              on_close = on_close)
    ws.on_open = on_open
    ws.run_forever()
