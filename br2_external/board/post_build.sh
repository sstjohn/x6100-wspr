#!/usr/bin/env bash

ZIP_FILENAME=Xiegu_X6100_Firmware_Upgrade_20220418.zip
ZIP_SITE=https://radioddity.s3.amazonaws.com

IMG_FILE=Firmware/X6100-sdcard-20220418.img
OVERLAY_FILES=`cat $BR2_EXTERNAL_X6100_WSPR_PATH/board/rootfs_overlay_files.txt | xargs`

if [ ! -e $BR2_DL_DIR/$ZIP_FILENAME ]; then
  wget -O $BR2_DL_DIR/$ZIP_FILENAME $ZIP_SITE/$ZIP_FILENAME
fi

rm -rf $TARGET_DIR/lib/modules/5.8.9/kernel
rm $TARGET_DIR/lib/modules/5.8.9/modules.*
unzip -p $BR2_DL_DIR/$ZIP_FILENAME $IMG_FILE | dd bs=1024 skip=529608 | tar xC $TARGET_DIR $OVERLAY_FILES
cp -r $TARGET_DIR/usr/share/emmc_sources/etc $TARGET_DIR

cp $BR2_EXTERNAL_X6100_WSPR_PATH/../spotter-loop.py $TARGET_DIR/root/

cat >>$TARGET_DIR/etc/monitrc <<__EOF__
set httpd port 2812 and
    allow localhost
    allow admin:monit
__EOF__
