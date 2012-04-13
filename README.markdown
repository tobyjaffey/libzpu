libzpu
======

A portable software emulator for the ZPU processor with no dependencies.

ZPU is an open source 32bit CPU with a GCC toolchain.
libzpu allows code compiled with zpu-elf-gcc to run inside a software virtual machine embedded inside another program.

All access to main memory and IO memory is via user provided callbacks meaning that RAM can be emulated 

GCC Toolchain
--------------

Get a prebuilt Linux version with:

        wget http://www.alvie.com/zpuino/downloads/zpu-toolchain-linux-1.0.tar.bz2
        mkdir /usr/local/zpu-toolchain
        cd /usr/local/zpu-toolchain
        tar jxfv ~/zpu-toolchain-linux-1.0.tar.bz2


To build from source:

        git clone git://repo.or.cz/zpugcc.git
        cd zpugcc/toolchain
        bash ./build.sh
        cp -r install /usr/local/zpu-toolchain
        export PATH=$PATH:/usr/local/zpu-toolchain/bin


