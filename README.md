# WSPR tools firmware for the Xiegu X6100

* Receive only (so far)
* Uploads spots to wsprnet.org when online
* Bonus userland improvements

## Installation Overview

1. Prepare a bootable SD card.
2. Unpack the root filesystem to the SD card.
2. Edit CALL and GRID toward the top of `/root/spotter-loop.py`.
3. (optional) Edit BANDS or HOPPING\_SCHEDULE to taste.

## Use

Boot the SD card on the X6100, run `/root/spotter-loop.py` on it.

## SD card preparation

A bootable SD card has Das U-Boot for sunxi 8KB in. The easiest way to 
prepare one for use is to flash a firmware upgrade image, destroy the second 
and third partitions, and create one with an ext4 filesystem in their place. 
See also `/usr/share/emmc_sources/*.sh` in any bootable image, and
`br2_external/board/prepare_sdcard.sh` in this tree.

## Root filesystem build

This will take over an hour and several dozen gigabytes of disk space.

1. `wget https://buildroot.org/downloads/buildroot-2020.02.9.tar.gz`
2. `git clone https://github.com/sstjohn/x6100-wspr.git --depth 1`
3. `tar xf buildroot-2020.02.9.tar.gz`
4. `make -C buildroot-2020.02.9 O=../br2_build BR2_EXTERNAL=../x6100-wspr/br2_external x6100-wspr_defconfig`
5. `cd br2_build; make -j$(nproc)`
6.  Produced image output to `br2_build/images/rootfs.tar`.

## Tips

* GNU Screen is included to allow the spotter loop to survive disconnects.
* Gamma-Ray is included for Qt debugging.
* Hamlib and tty0tty are included for loopback control. (See `redirect_qt_app` 
  in the spotter loop script).
* Set the `X6100_SDCARD_DEV` environment variable to the SD card device and
  the buildroot will update its second partition with the new rootfs in the 
  post image step. (See `br2_external/build/post_image.sh` for details.)
* The kernel built in the buildroot will boot, but will not drive the display
  as panel drivers are not available. The post-build step replaces the built
  kernel and modules with those from Xiegu's April 2022 firmware in order to 
  make the screen work.
* Demo GUI injection library is included:
  ```
  monit stop x6100_ui_v100 
  cd /usr/app_qt
  LD_PRELOAD=libqinj.so.1.0.0 ./x6100_ui_v100
  ```
