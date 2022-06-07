#!/usr/bin/env bash

if [ "x$X6100_SDCARD_DEV" = "x" ]; then
  echo set X6100_SDCARD_DEV first
  exit 1
fi

if [ ! -b ${X6100_SDCARD_DEV} ]; then
  echo "X6100_SDCARD_DEV ($X6100_SDCARD_DEV) does not exist"
  exit 2
fi

echo WARNING! All data in $X6100_SDCARD_DEV will be lost.
if [ "x$X6100_SDCARD_CLEAN_REALLY" = "xYES" ]; then
  echo "X6100_SDCARD_CLEAN_REALLY is set, you were already warned."
else
  echo -n "Type 'YES' to continue: "
  read yn
  if [ $yn != "YES" ]; then
    echo aborting
    exit 3
  fi
fi

umount ${X6100_SDCARD_DEV}*
fdisk ${X6100_SDCARD_DEV} <<__EOF__
o
n
p
1
32768
131071
n
p
2
131072

w
__EOF__

if [ $? -ne 0 ]; then
  echo "fdisk did not succeed"
  #exit 4
fi

sync

mkfs.vfat ${X6100_SDCARD_DEV}${PART_PREFIX}1
if [ $? -ne 0 ]; then
  echo "mkfs.vfat did not succeed on first partition"
  exit 5
fi

if [ "x$X6100_SDCARD_CLEAN_REALLY" = "xYES" ]; then
  echo y | mkfs.ext4 ${X6100_SDCARD_DEV}${PART_PREFIX}2
  if [ $? -ne 0 ]; then
    echo "mkfs.ext4 did not succeed on second partition"
    exit 6
  fi
else
  mkfs.ext4 ${X6100_SDCARD_DEV}${PART_PREFIX}2
  if [ $? -ne 0 ]; then
    echo "mkfs.ext4 did not succeed on second partition"
    exit 6
  fi
fi

dd if=${TARGET_DIR}/usr/share/emmc_sources/u-boot-sunxi-with-spl.bin of=${X6100_SDCARD_DEV} bs=1024 seek=8

mount_point=`mktemp --dir`
mount ${X6100_SDCARD_DEV}${PART_PREFIX}1 $mount_point
#cp -f ${TARGET_DIR}/usr/share/emmc_sources/zImage ${mount_point}
cp -f ${TARGET_DIR}/usr/share/emmc_sources/sun8i-r16-x6100.dtb ${mount_point}
${HOST_DIR}/bin/mkimage -C none -A arm -T script -d ${BR2_EXTERNAL_X6100_WSPR_PATH}/board/boot.cmd ${mount_point}/boot.scr
umount ${mount_point}
rmdir ${mount_point}

exit 0

