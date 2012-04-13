libzpu
======

A portable software emulator for the ZPU processor with no dependencies.

ZPU is an open source 32bit CPU with a GCC toolchain.
libzpu allows code compiled with zpu-elf-gcc to run inside a software virtual machine embedded inside another program.

All access to main memory and IO memory is via user provided callbacks.

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


Possible uses
-------------

Learning ZPU instructions/architecture.

Embedding sandboxed C code into your application.

Emulating a "real computer" with an 8-bit micro, by using an MMC card as virtual RAM.


Caveats
-------

libzpu is not a cycle accurate simulation of the hardware
libzpu does not implement every opcode
libzpu does not emulate any peripherals

