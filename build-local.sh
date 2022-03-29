#!/usr/bin/env sh

# This script sets up a buildroot and toolchain similar enough to the X6100
# to cross-compile for it, and then builds python, screen and wsprd. It has 
# only been tested on Debian 11 x64. It requies an as-yet indeterminate set 
# of packages, including qemu-system-arm and binfmt. The build itself is slow 
# because cmake requires a bunch of libraries to set up WSJT-X that aren't 
# ultimately used by wsprd. They don't make it into the final tarball, though.

MAKE_JOBS=5  # argument to make -j
MINIMIZE=1   # deletes unneeded libraries and include files

WORK_ROOT=`pwd`

set -e

if [ ! -e buildroot-2020.02.9.tar.gz ]; then
  wget https://buildroot.org/downloads/buildroot-2020.02.9.tar.gz
fi
if [ ! -e buildroot-2020.02.9/output/host/bin/arm-buildroot-linux-gnueabihf-gcc ]; then
  tar xf buildroot-2020.02.9.tar.gz
  cp buildroot.config buildroot-2020.02.9/.config
  make -C buildroot-2020.02.9 oldconfig
  make -C buildroot-2020.02.9 -j$MAKE_JOBS
fi

if [ ! -d target ]; then
  mkdir target
fi
if [ ! -e target/lib/ld-linux/armhf.so.3 ]; then
  tar xf buildroot-2020.02.9/output/images/rootfs.tar -C target
  mkdir -p target/usr/local/lib
fi

if [ ! -e Python-3.10.4.tar.xz ]; then
  wget https://www.python.org/ftp/python/3.10.4/Python-3.10.4.tar.xz
fi
if [ ! -d Python-3.10.4 ]; then
  tar xf Python-3.10.4.tar.xz
fi
if [ ! -e host/bin/python3.10 ]; then
  mkdir -p Python-3.10.4/host_build
  (cd Python-3.10.4/host_build; ../configure --prefix=$WORK_ROOT/host --enable-optimizations)
  make -C Python-3.10.4/host_build -j$MAKE_JOBS
  make -C Python-3.10.4/host_build altinstall
fi
if [ ! -e target/usr/local/bin/python3.10 ]; then
  mkdir -p Python-3.10.4/target_build
  (
  	cd Python-3.10.4/target_build
       	PATH=$PATH:$WORK_ROOT/buildroot-2020.02.9/output/host/bin:$WORK_ROOT/host/bin 
	../configure --target=arm-buildroot-linux-gnueabihf --host=arm-buildroot-linux-gnueabihf --build=x86_64-pc-linux-gnu --prefix=$WORK_ROOT/target/usr/local --enable-optimizations --disable-ipv6 ac_cv_file__dev_ptmx=no ac_cv_file__dev_ptc=no
  	make -j$MAKE_JOBS
  	make altinstall
  )
fi

if [ ! -e target/usr/local/bin/pip3 ]; then
  QEMU_LD_PREFIX=$WORK_ROOT/target $WORK_ROOT/target/usr/local/bin/python3.10 -mensurepip
fi
if [ ! -d target/usr/local/lib/python3.10/site-packages/apscheduler ] || [ ! -d target/usr/local/lib/python3.10/site-packages/requests ]; then
  QEMU_LD_PREFIX=$WORK_ROOT/target $WORK_ROOT/target/usr/local/bin/pip3 install apscheduler requests
fi

if [ ! -e fftw-3.3.10.tar.gz ]; then
  wget https://www.fftw.org/fftw-3.3.10.tar.gz
fi
if [ ! -d fftw-3.3.10 ]; then
  tar xf fftw-3.3.10.tar.gz
fi
if [ ! -e target/usr/local/lib/libfftw3f.a ]; then
  ( 
    cd fftw-3.3.10;
    PATH=$PATH:$WORK_ROOT/buildroot-2020.02.9/output/host/bin
    ./configure --target=arm-buildroot-linux-gnueabihf --host=arm-buildroot-linux-gnueabihf --prefix=$WORK_ROOT/target/usr/local --enable-float --enable-threads
    make -j$MAKE_JOBS
    make install
  )
fi

if [ ! -e libusb-1.0.25.tar.bz2 ]; then
  wget https://github.com/libusb/libusb/releases/download/v1.0.25/libusb-1.0.25.tar.bz2
fi
if [ ! -d libusb-1.0.25 ]; then
  tar xf libusb-1.0.25.tar.bz2
fi
if [ ! -e target/usr/local/lib/libusb-1.0.a ]; then
  (
    cd libusb-1.0.25
    PATH=$PATH:$WORK_ROOT/buildroot-2020.02.9/output/host/bin
    ./configure --target=arm-buildroot-linux-gnueabihf --host=arm-buildroot-linux-gnueabihf --build=x86_64-pc-linux-gnu --prefix=$WORK_ROOT/target/usr/local
    make -j$MAKE_JOBS
    make install
  )
fi

if [ ! -d hamlib-for-wsjtx ]; then
  git clone git://git.code.sf.net/u/bsomervi/hamlib hamlib-for-wsjtx --branch integration --single-branch --depth 1
fi
if [ ! -e hamlib-for-wsjtx/build/lib/libhamlib.a ]; then
  (
    cd hamlib-for-wsjtx
    PATH=$PATH:$WORK_ROOT/buildroot-2020.02.9/output/host/bin
    ./bootstrap
    ./configure --target=arm-buildroot-linux-gnueabihf --host=arm-buildroot-linux-gnueabihf --build=x86_64-pc-linux-gnu --prefix=$WORK_ROOT/hamlib-for-wsjtx/build --disable-shared --enable-static --without-cxx-binding --disable-winradio CFLAGS=-"g -O2 -fdata-sections -ffunction-sections" LDFLAGS=-Wl,--gc-sections 
    make -j$MAKE_JOBS
    make install-strip
  )
fi

if [ ! -e boost_1_78_0.tar.bz2 ]; then
  wget https://boostorg.jfrog.io/artifactory/main/release/1.78.0/source/boost_1_78_0.tar.bz2
fi
if [ ! -d boost_1_78_0 ]; then
  tar xf boost_1_78_0.tar.bz2
fi
if [ ! -e boost_1_78_0/b2 ]; then
  (cd boost_1_78_0; ./bootstrap.sh)  
  sed -i -e 's/using gcc ;/using gcc : arm : arm-buildroot-linux-gnueabihf-g++ ;/' boost_1_78_0/project-config.jam
fi
if [ ! -e target/usr/local/lib/libboost_system.a ]; then
  (
    cd boost_1_78_0
    PATH=$PATH:$WORK_ROOT/buildroot-2020.02.9/output/host/bin
    ./b2 install --without-python --prefix=$WORK_ROOT/target/usr/local
  )
fi

if [ ! -e wsjtx-2.5.4.tgz ]; then
  wget https://physics.princeton.edu/pulsar/k1jt/wsjtx-2.5.4.tgz
fi
if [ ! -d wsjtx ]; then
  tar xf wsjtx-2.5.4.tgz
  tar xf wsjtx-2.5.4/src/wsjtx.tgz
  rm -rf wsjtx-2.5.4
fi
if [ ! -e target/usr/local/bin/wsprd ]; then
  (
    cd wsjtx
    mkdir build
    cd build
    cmake -DCMAKE_TOOLCHAIN_FILE=$WORK_ROOT/buildroot-2020.02.9/output/host/share/buildroot/toolchainfile.cmake -DBoost_INCLUDE_DIR=$WORK_ROOT/target/usr/local/include -DBoost_DIR=$WORK_ROOT/target/usr/local/lib/cmake/Boost-1.78.0/ -Dboost_headers_DIR=$WORK_ROOT/target/usr/local/lib/cmake/boost_headers-1.78.0/ -Dboost_log_DIR=$WORK_ROOT/target/usr/local/lib/cmake/boost_log-1.78.0/ -Dboost_log_setup_DIR=$WORK_ROOT/target/usr/local/lib/cmake/boost_log_setup-1.78.0/ -Dboost_atomic_DIR=$WORK_ROOT/target/usr/local/lib/cmake/boost_atomic-1.78.0/ -Dboost_chrono_DIR=$WORK_ROOT/target/usr/local/lib/cmake/boost_chrono-1.78.0/ -Dboost_filesystem_DIR=$WORK_ROOT/target/usr/local/lib/cmake/boost_filesystem-1.78.0/ -Dboost_regex_DIR=$WORK_ROOT/target/usr/local/lib/cmake/boost_regex-1.78.0/ -Dboost_thread_DIR=$WORK_ROOT/target/usr/local/lib/cmake/boost_thread-1.78.0/ -DFFTW3F_THREADS_LIBRARY=$WORK_ROOT/target/usr/local/lib/libfftw3f_threads.a -DFFTW3F_LIBRARY=$WORK_ROOT/target/usr/local/lib/libfftw3f.a -DFFTW3_INCLUDE_DIR=$WORK_ROOT/target/usr/local/include -DUsb_INCLUDE_DIR=$WORK_ROOT/target/usr/local/include -DUsb_LIBRARY=$WORK_ROOT/target/usr/local/lib/libusb-1.0.a -DHamlib_INCLUDE_DIR=$WORK_ROOT/hamlib-for-wsjtx/build/include -DHamlib_LIBRARY=$WORK_ROOT/hamlib-for-wsjtx/build/lib/libhamlib.a ..
    cmake --build . --target wsprd
    $WORK_ROOT/buildroot-2020.02.9/output/host/bin/patchelf --set-rpath /usr/local/lib wsprd
    cp wsprd $WORK_ROOT/target/usr/local/bin
  )
fi

if [ ! -d screen-4.9.0 ]; then
  if [ ! -e screen-4.9.0.tar.gz ]; then
    wget http://ftp.gnu.org/gnu/screen/screen-4.9.0.tar.gz
  fi
  tar xf screen-4.9.0.tar.gz
fi
if [ ! -e target/usr/local/bin/screen ]; then
  (
    cd screen-4.9.0
    ./autogen.sh
    PATH=$PATH:$WORK_ROOT/buildroot-2020.02.9/output/host/bin
    ./configure --target=arm-buildroot-linux-gnueabihf --host=arm-buildroot-linux-gnueabihf --build=x86_64-pc-linux-gnu --prefix=$WORK_ROOT/target/usr/local
    make
    make install
  )
fi

if [ $MINIMIZE -eq  1 ]; then
  rm -rf target/usr/local/include
  rm -rf target/usr/local/lib/lib*
  rm -rf target/usr/local/share/info
  rm -rf target/usr/local/share/man
  rm -rf target/usr/local/lib/cmake
  rm -rf target/usr/local/lib/pkgconfig
  find target/usr/local/lib/python3.10 | grep -E "(__pycache__|\.pyc|\.pyo$)" | xargs rm -rf
fi

cp target/usr/lib/libgfortran* target/usr/local/lib
fakeroot sh -c "chown -R root.root target/usr/local; tar -C target -czf x6100-local.tgz usr/local"

