#!/bin/sh

echo -n X6100_SDCARD_DEV is $X6100_SDCARD_DEV
if [ "x$X6100_SDCARD_DEV" = "x" ]; then
  echo ' not given'
  exit 0
fi
echo

if [ ! -b $X6100_SDCARD_DEV ]; then
  echo 'SD card not present'
  exit 0
fi

if [ "x$X6100_SDCARD_CLEAN" = "xYES" ]; then
  echo "Formatting SD card"
  sudo -E ${BR2_EXTERNAL_X6100_WSPR_PATH}/board/prepare_sdcard.sh
fi

if [ ! -b ${X6100_SDCARD_DEV}${PART_PREFIX}2 ]; then
  echo "SD card not partitioned correctly"
  exit 0
fi

mount_point=`mktemp --dir`
sudo mount ${X6100_SDCARD_DEV}${PART_PREFIX}2 $mount_point
sudo tar xCf $mount_point $1/rootfs.tar
sudo umount $mount_point
rmdir $mount_point
echo "SD card updated"
