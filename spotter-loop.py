#!/usr/local/bin/python3.10
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

from contextlib import contextmanager
from datetime import datetime
from glob import glob
import json
from math import ceil
from queue import Queue
import os
from random import choice
import sys
from threading import Thread
from time import sleep
from traceback import print_exc

from apscheduler.schedulers.blocking import BlockingScheduler
import Hamlib
import requests

CALL=""
GRID=""

BANDS = {
  "160": {
     "frequency": 1836600,
     "enabled": True
  },
  "80": {
     "frequency": 3568600,
     "enabled": True
  },
  "60": {
     "frequency": 5287200,
     "enabled": True
  },
  "40": {
     "frequency": 7038600,
     "enabled": True
  },
  "30": {
     "frequency": 10138700,
     "enabled": True
  },
  "20": {
     "frequency": 14095600,
     "enabled": True
  },
  "17": {
     "frequency": 18104600,
     "enabled": True
  },
  "15": {
     "frequency": 21094600,
     "enabled": True
  },
  "12": {
     "frequency": 24924600,
     "enabled": True
  },
  "10": {
     "frequency": 28124600,
     "enabled": True
  },
  "6": {
     "frequency": 50293000,
     "enabled": True
  }
}

HOPPING_SCHEDULE = ["160", "80", "60", "40", "30", "20", "17", "15", "12", "10"] * 3

DATA_DIR="/root"
WSPRD_ARGS=f"-w -a {DATA_DIR}"

def check_setup(retry=False):
    try:
        resolv = open('/etc/resolv.conf', 'r').readlines()
    except FileNotFoundError:
        resolv = []
    if not 'nameserver' in ''.join(resolv):
        if os.path.exists("/run/NetworkManager/resolv.conf") and not ''.join(resolv) and not retry:
            os.system("cp /run/NetworkManager/resolv.conf /etc")
            return check_setup(True)
        else:
            print("Set up /etc/resolv.conf for DNS resolution to spot to wsprnet.org.")
    if os.system("command -v wsprd 2>&1 >/dev/null"):
        os.environ['PATH'] += os.pathsep + "/usr/local/bin"
    if os.system("command -v wsprd 2>&1 >/dev/null"):
        print("Put wsprd directory on PATH first.")
        return False
    if os.path.exists(f"{DATA_DIR}/.spotter.conf"):
        user_conf = json.load(open(f"{DATA_DIR}/.spotter.conf", "r"))
    else:
        user_conf = {}
    for i in ("CALL", "GRID"):
        if not i in globals() or not globals()[i]:
            if i in user_conf:
                globals()[i] = user_conf[i]
            else:
              print(f"Edit spotter-loop.py to set {i} first.")
              return False
    if not os.path.exists("/dev/tnt0"):
        if os.system("insmod /usr/local/lib/modules/`uname -r`/extra/tty0tty.ko"):
            print("Load tty0tty.ko first.")
            return False
    
    return True


@contextmanager
def redirect_qt_app():
    os.rename("/dev/ttyS2", "/dev/ttyS2.old")
    try:
        tnt0_stat = os.lstat("/dev/tnt0")
        newdev = os.makedev(os.major(tnt0_stat.st_rdev), os.minor(tnt0_stat.st_rdev))
        os.mknod("/dev/ttyS2", tnt0_stat.st_mode, newdev)
        
        os.system("/etc/init.d/S99userappstart restart")
        sleep(10)

        yield None

    finally:
        os.rename("/dev/ttyS2.old", "/dev/ttyS2")
        os.system("/etc/init.d/S99userappstart restart")


@contextmanager
def get_rig():
    Hamlib.rig_set_debug(Hamlib.RIG_DEBUG_NONE)
    rig = Hamlib.Rig(3087)
    rig.set_conf("rig_pathname", "/dev/tnt1")
    rig.open()
    try:
        yield rig

    finally:
        rig.close()


def hop_bands(rig):
    schedule_index = ceil(datetime.utcnow().minute / 2.0) % 30
    chosen_band = HOPPING_SCHEDULE[schedule_index]
    if not BANDS[chosen_band]['enabled']:
        chosen_band = choice(list(filter(lambda b: BANDS[b]["enabled"], BANDS)))

    print(f"hopping to {chosen_band}m band ({BANDS[chosen_band]['frequency']}c)")

    rig.set_mode(4, 700)
    rig.set_freq(Hamlib.RIG_VFO_A, BANDS[chosen_band]["frequency"])


def decode_thread_main(recordings_queue):
    while True:
        next_recording, frequency = recordings_queue.get()
        if not next_recording:
            return
        os.system(f"time wsprd {WSPRD_ARGS} -f {frequency} {next_recording}")
        os.system(f"rm {next_recording}")
        upload_spots(next_recording)


def upload_spots(recording=None, spotfile="wspr_spots.txt"):
    if os.path.getsize(spotfile) == 0:
        print("no spots")
        return True
    files = {'allmept': open(f"{DATA_DIR}/{spotfile}", 'r')}
    params = {'call': CALL, 'grid': GRID, 'version': 'x6w-0.2.1'}
    response = None
    try:
        response = requests.post('http://wsprnet.org/post', files=files, params=params)
        response.raise_for_status()
        if "Log rejected" in response.text:
            raise Exception("log rejected")
        return True

    except Exception as e:
        print(f"failed to upload spotfile {spotfile}: {e}")
        if spotfile == "wspr_spots.txt": 
            timestamp = os.path.splitext(os.path.basename(recording))[0]
            os.system(f"mv {DATA_DIR}/wspr_spots.txt {DATA_DIR}/wspr_spots-{timestamp}.rej")
        return False

    finally:
        if response:
            print(response.text)            


def retry_failed_spot_uploads():
    rejects = glob(f"{DATA_DIR}/*.rej")
    if not rejects:
        return
    print("retrying failed spot uploads")
    for reject in rejects:
        if upload_spots(spotfile=os.path.basename(reject)):
            os.system(f"rm {reject}")


def do_rx(recordings_queue, rig):
    fname = "/tmp/" + datetime.utcnow().strftime("%y%m%d_%H%M") + ".wav"
    res = os.system(f"arecord -D mixcapture -d 114 -f S16_LE -r 12000 {fname}")
    if 0 == res:
        recordings_queue.put([fname, rig.get_freq() / 1e6])
    hop_bands(rig)


def main():
    if not check_setup():
        return 1
    with redirect_qt_app():
        with get_rig() as rig:
            recordings_queue = Queue()
            decode_thread = Thread(target=decode_thread_main, args=[recordings_queue])
            decode_thread.start()
            try:
                scheduler = BlockingScheduler()
                scheduler.add_job(do_rx, 'cron', minute='*/2', second=0, args=[recordings_queue, rig])
                scheduler.add_job(retry_failed_spot_uploads, 'cron', minute='*/30')
                hop_bands(rig)
                scheduler.start()
            finally:
                try: recordings_queue.put((None, None), block=False)
                except Exception: pass


if __name__ == "__main__":
    sys.exit(main())
