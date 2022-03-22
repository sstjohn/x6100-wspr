#!/usr/local/bin/python3.9
# -*- coding: utf-8 -*-
#
# Copyright (c) 2022 Saul St John
# 
# This program is free software: you can redistribute it and/or modify  
# it under the terms of the GNU General Public License as published by  
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License 
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

import os
from datetime import datetime
from glob import glob
from queue import Queue
import sys
from threading import Thread
from traceback import print_exc

from apscheduler.schedulers.blocking import BlockingScheduler
import requests

#FREQUENCY="3.5686"   #80m
#FREQUENCY="5.2872"   #60m
#FREQUENCY="5.3647"   #60m
#FREQUENCY="7.0386"   #40m
FREQUENCY="10.1387"  #30m
#FREQUENCY="14.0956"  #20m
#FREQUENCY="18.1046"  #17m
#FREQUENCY="21.0946"  #15m
#FREQUENCY="24.9246"  #12m
#FREQUENCY="28.1246"  #10m
#FREQUENCY="50.293"   #6m

CALL="WI9YZA"
GRID="EN53ib"

WSPRD_ARGS="-w"

def check_setup():
	resolv = open('/etc/resolv.conf', 'r').readlines()
	if not 'nameserver' in ''.join(resolv):
		print("Set up /etc/resolv.conf for DNS resolution first.")
		return False
	if os.system("command -v wsprd 2>&1 >/dev/null"):
		print("Put wsprd directory on PATH first.")
		return False
	return True

def decode_thread_main(recordings_queue):
	while True:
		next_recording = recordings_queue.get()
		if not next_recording:
			return
		os.system(f"time wsprd {WSPRD_ARGS} -f {FREQUENCY} {next_recording}")
		os.system(f"rm {next_recording}")
		upload_spots(next_recording)

def upload_spots(recording=None, spotfile="wspr_spots.txt"):
	files = {'allmept': open(spotfile, 'r')}
	params = {'call': CALL, 'grid': GRID}
	response = None
	try:
		response = requests.post('http://wsprnet.org/post', files=files, params=params)
		response.raise_for_status()
		return True
	except Exception as e:
		print(f"spotfile upload failed: {e}")
		if spotfile == "wspr_spots.txt": 
			timestamp = os.path.splitext(os.path.basename(recording))[0]
			os.system(f"mv wspr_spots.txt wspr_spots-{timestamp}.rej")
		return False
	finally:
		if response:
			print(response.text)			

def retry_failed_spot_uploads():
	rejects = glob("*.rej")
	if not rejects:
		return
	print("retrying failed spot uploads")
	for reject in rejects:
		if upload_spots(spotfile=reject):
			os.system(f"rm {reject}")

def do_rx(recordings_queue):
	fname = "/tmp/" + datetime.utcnow().strftime("%y%m%d_%H%M") + ".wav"
	res = os.system(f"arecord -D mixcapture -d 114 -f S16_LE -r 12000 {fname}")
	if 0 == res:
		recordings_queue.put(fname)

def main():
	if not check_setup():
		return 1
	recordings_queue = Queue()
	decode_thread = Thread(target=decode_thread_main, args=[recordings_queue])
	decode_thread.start()
	try:
		scheduler = BlockingScheduler()
		scheduler.add_job(do_rx, 'cron', minute='*/2', second=0, args=[recordings_queue])
		scheduler.add_job(retry_failed_spot_uploads, 'cron', minute='*/30')
		scheduler.start()
	finally:
		try: recordings_queue.put(None)
		except Exception: pass

if __name__ == "__main__":
	sys.exit(main())
