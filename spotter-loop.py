#!/usr/local/bin/python3.9

import os
from datetime import datetime
from queue import Queue
from threading import Thread

from apscheduler.schedulers.blocking import BlockingScheduler
import requests

FREQUENCY="3.5686"

CALL="WI9YZA"
GRID="EN53ib"

def decode_thread_main(recordings_queue):
	while True:
		next_recording = recordings_queue.get()
		if not next_recording:
			return
		os.system(f"wsprd -f {FREQUENCY} {next_recording}")
		os.system(f"rm {next_recording}")
		upload_spots()

def upload_spots():
	files = {'allmept': open('wspr_spots.txt', 'r')}
	params = {'call': CALL, 'grid': GRID}
	response = requests.post('http://wsprnet.org/post', files=files, params=params)
	response.raise_for_status()
	print(response.text)			

def do_rx(recordings_queue):
	fname = datetime.utcnow().strftime("%y%m%d_%H%M") + ".wav"
	res = os.system(f"arecord -D mixcapture -d 114 -f S16_LE -r 12000 {fname}")
	if 0 == res:
		recordings_queue.put(fname)

def main():
	recordings_queue = Queue()
	decode_thread = Thread(target=decode_thread_main, args=[recordings_queue])
	decode_thread.start()
	try:
		scheduler = BlockingScheduler()
		scheduler.add_job(do_rx, 'cron', minute='*/2', second=0, args=[recordings_queue])
		scheduler.start()
	finally:
		try: recordings_queue.put(None)
		except Exception: pass

if __name__ == "__main__":
	main()
