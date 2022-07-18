#!/bin/sh

X6100_SDCARD_DEV=`echo $X6100_SDCARD_DEV | tr -d '"'`
echo -n X6100_SDCARD_DEV is $X6100_SDCARD_DEV
if [ "x$X6100_SDCARD_DEV" = "x" ]; then
  echo ' not given'
  exit 0
fi
echo


if [ ! -b $X6100_SDCARD_DEV ] || \
   [ $(cat /sys/block/`basename $X6100_SDCARD_DEV`/size) -eq 0 ]; then
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

mount_point_root=`mktemp --dir`
sudo mount ${X6100_SDCARD_DEV}${PART_PREFIX}2 $mount_point_root
sudo tar xCf $mount_point_root $1/rootfs.tar
mount_point_boot=`mktemp --dir`
sudo mount ${X6100_SDCARD_DEV}${PART_PREFIX}1 $mount_point_boot
sudo cp $mount_point_root/boot/zImage $mount_point_boot
sudo umount $mount_point_root
sudo umount $mount_point_boot
rmdir $mount_point_root
rmdir $mount_point_boot
echo "SD card updated"
