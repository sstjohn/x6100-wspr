#!/usr/bin/env bash

# This script sets up a buildroot similar enough to the X6100 to cross-compile 
# for it. It produces a workalike rootfs with bonus functionality and 
# optionally updates an sdcard with it.

if [ ! -e buildroot-2020.02.9.tar.gz ]; then
  wget https://buildroot.org/downloads/buildroot-2020.02.9.tar.gz
fi
if [ ! -e buildroot-2020.02.9/output/host/bin/arm-buildroot-linux-gnueabihf-gcc ]; then
  tar xf buildroot-2020.02.9.tar.gz
  make -C buildroot-2020.02.9 BR2_EXTERNAL=../br2_external O=../br2_build x6100-wspr_defconfig
  make -C br2_build -j$((`nproc` + 1))
fi
