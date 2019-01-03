import websocket
from struct import *

try:
    import thread
except ImportError:
    import _thread as thread
import time

# Fo counting number of frames
counter = 0

# What to do when we receive a frame.
def on_message(ws, message):
	global counter
	# We receive the data in binary, so we must unpack it. Specifying the array elementa [0] drops the tuple so it formats better.
	for i in range(0, len(message), 2):
		print("{}, {}".format(counter, (unpack("h", message[i:i+2])[0])))
		counter += 1

def on_error(ws, error):
    print(error)

# What to do when we loose connection. (Prints total frames received.)
def on_close(ws):
    print("### closed ###")
    print(f"Counter: {counter}")

# Once we successfully connect, keep the connection open until we close the program.
def on_open(ws):
	def run(*args):
		print(",ch1")	
		while 1:
			time.sleep(1)
		ws.close()
		print("thread terminating...")
	
	thread.start_new_thread(run, ())


if __name__ == "__main__":
    websocket.enableTrace(True)
    ws = websocket.WebSocketApp("ws://192.168.4.1:80",
                              on_message = on_message,
                              on_error = on_error,
                              on_close = on_close)
    ws.on_open = on_open
    ws.run_forever()
