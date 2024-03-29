#!/usr/bin/env bash

ZIP_FILENAME=Xiegu_X6100_Firmware_Upgrade_20220418.zip
ZIP_SITE=https://radioddity.s3.amazonaws.com

IMG_FILE=Firmware/X6100-sdcard-20220418.img
OVERLAY_FILES=`cat $BR2_EXTERNAL_X6100_WSPR_PATH/board/rootfs_overlay_files.txt | xargs`

if [ ! -e $BR2_DL_DIR/$ZIP_FILENAME ]; then
  wget -O $BR2_DL_DIR/$ZIP_FILENAME $ZIP_SITE/$ZIP_FILENAME
fi

if [ "x$X6100_USE_STOCK_KERNEL" = "xYES" ]; then
  OVERLAY_FILES="$OVERLAY_FILES ./lib/modules/ ./boot/zImage"
  if [ "x$X6100_OVERLAY_MODULES" != "xYES" ]; then
    rm -rf $TARGET_DIR/lib/modules/5.8.9/kernel
  fi
fi

unzip -p $BR2_DL_DIR/$ZIP_FILENAME $IMG_FILE | dd bs=1024 skip=529608 | tar xC $TARGET_DIR $OVERLAY_FILES
cp -r $TARGET_DIR/usr/share/emmc_sources/etc $TARGET_DIR

cp $BR2_EXTERNAL_X6100_WSPR_PATH/../spotter-loop.py $TARGET_DIR/root/

cat >>$TARGET_DIR/etc/ssh/sshd_config <<__EOF__
PermitRootLogin yes
PasswordAuthentication yes
PermitEmptyPasswords yes
__EOF__

if [ "x$X6100_DEFAULT_PRELOAD" = "xYES" ]; then
  patchelf --add-needed libqinj.so.1.0.0 $TARGET_DIR/usr/app_qt/x6100_ui_v100
fi

if [ "x$X6100_RELEASE_BUILD" = "xYES" ]; then
	rm $TARGET_DIR/root/spotter-loop.conf
	find $TARGET_DIR/etc/NetworkManager/system-connections -type f ! -name '*.template' -delete
	rm $TARGET_DIR/etc/ssh/ssh_host_*
fi

