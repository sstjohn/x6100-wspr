#!/usr/bin/env python3
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
import stat
from subprocess import Popen
import sys
from threading import Thread
from time import sleep
from traceback import print_exc

from apscheduler.schedulers.blocking import BlockingScheduler
from apscheduler.triggers.cron import CronTrigger
import dbus
import Hamlib
import requests

CALL=""
GRID=""

BANDS = {
  "160": {
     "frequency": 1836600,
  },
  "80": {
     "frequency": 3568600,
  },
  "60": {
     "frequency": 5287200,
  },
  "40": {
     "frequency": 7038600,
  },
  "30": {
     "frequency": 10138700,
  },
  "20": {
     "frequency": 14095600,
  },
  "17": {
     "frequency": 18104600,
  },
  "15": {
     "frequency": 21094600,
  },
  "12": {
     "frequency": 24924600,
  },
  "10": {
     "frequency": 28124600,
  },
  "6": {
     "frequency": 50293000,
  }
}

ALL_BAND_DEFAULTS = {'enabled': True}

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
        if "ALL_BAND_DEFAULTS" in user_conf:
            ALL_BAND_DEFAULTS.update(user_conf["ALL_BAND_DEFAULTS"])
    else:
        user_conf = {}
    for i in ("CALL", "GRID"):
        if not i in globals() or not globals()[i]:
            if i in user_conf:
                globals()[i] = user_conf[i]
            else:
              print(f"You should probably edit spotter-loop.py to set {i} first.")
    if not os.path.exists("/dev/tnt0"):
        if os.system("insmod /lib/modules/`uname -r`/extra/tty0tty.ko"):
            print("Load tty0tty.ko first.")
            return False
    
    return True


@contextmanager
def redirect_qt_app():
    if os.path.exists("/dev/ttyS2.old"):
        yield None
        return

    os.rename("/dev/ttyS2", "/dev/ttyS2.old")
    try:
        tnt0_stat = os.lstat("/dev/tnt0")
        newdev = os.makedev(os.major(tnt0_stat.st_rdev), os.minor(tnt0_stat.st_rdev))
        os.mknod("/dev/ttyS2", tnt0_stat.st_mode, newdev)
        
        if os.path.exists("/etc/init.d/S99userappstart"):
            os.system("/etc/init.d/S99userappstart stop")
        elif os.path.exists("/etc/init.d/S99-1-monit"):
            os.system("monit stop x6100_ui_v100")
        else:
            print("Don't know how to restart QT app.")
            exit(1)

        sleep(1)    
        ui_proc = Popen(["/usr/app_qt/x6100_ui_v100"], env=dict(os.environ, **{
                "QINJ_TEXT": "WSPR",
                "LD_PRELOAD": "libqinj.so.1.0.0"
        }))
        sleep(5)

        yield None

    finally:
        ui_proc.terminate()
        try:
            ui_proc.wait(5)
        except TimeoutError:
            ui_proc.kill()

        os.rename("/dev/ttyS2.old", "/dev/ttyS2")
        if os.path.exists("/etc/init.d/S99userappstart"):
            os.system("/etc/init.d/S99userappstart start")
        else:
            os.system("monit start x6100_ui_v100")


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
    band_params = dict(ALL_BAND_DEFAULTS, **BANDS[chosen_band])
    if not band_params['enabled']:
        chosen_band = choice(list(filter(lambda b: BANDS[b]["enabled"], BANDS)))

    print(f"hopping to {chosen_band}m band ({BANDS[chosen_band]['frequency']}c)")

    rig.set_mode(4, 700)
    rig.set_freq(Hamlib.RIG_VFO_A, BANDS[chosen_band]["frequency"])
    rig.set_level("AF", 0.0)
    rig.set_level("AGC", 2)
    rig.set_level("ATT", 1 if band_params.get("attenuator") else 0)
    rig.set_level("PREAMP", 10 if band_params.get("preamp") else 0)
    rig.set_level("RF", (1 + band_params.get("gain", 50)) / 100)

def decode_thread_main(recordings_queue):
    while True:
        next_recording, frequency = recordings_queue.get()
        if not next_recording:
            return
        os.system(f"time wsprd {WSPRD_ARGS} -f {frequency} {next_recording}")
        os.system(f"rm {next_recording}")
        add_spots_to_ui(next_recording)
        upload_spots(next_recording)

def add_spots_to_ui(recording):
    if os.path.getsize(f"{DATA_DIR}/wspr_spots.txt") == 0:
        return

    try:
        injection_proxy = dbus.SystemBus().get_object('lol.ssj.xwspr', '/')
    except dbus.exceptions.DBusException:
        return

    with open(f"{DATA_DIR}/wspr_spots.txt", "r") as spotfile:
        for line in spotfile.readlines():
            parts = line.split(" ")
            parts = list(filter(None, parts))
            injection_proxy.wsprReceived(
                f"{parts[0]} {parts[1]}", 
                parts[3], *parts[5:9]
            )

def upload_spots(recording=None, spotfile="wspr_spots.txt"):
    if os.path.getsize(spotfile) == 0:
        print("no spots")
        return True
    files = {'allmept': open(f"{DATA_DIR}/{spotfile}", 'r')}
    params = {'call': CALL, 'grid': GRID, 'version': 'x6w-0.9.6'}
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
                scheduler.add_job(do_rx, CronTrigger(minute='*/2', second=0), args=[recordings_queue, rig])
                scheduler.add_job(retry_failed_spot_uploads, CronTrigger(minute='*/30'))
                hop_bands(rig)
                scheduler.start()
            finally:
                try: recordings_queue.put((None, None), block=False)
                except Exception: pass


if __name__ == "__main__":
    sys.exit(main())
