Kilobots Toolchain
==================

This repository contains four command line tools and a graphical
frontend.

* kbcompile: Compile your C code to a hex file.
* kbmerge: Merge controller and program hex files to a single Overhead Controller hex file.
* kbprogram: Upload hex file to Kilobot Overhead Controller.
* kbsend: Sends commands to Kilobot Overhead Controller.
* kbgui: PyGTK front-end for the previous three programs.

[!kbgui](https://raw.github.com/acornejo/kilobots-toolchain/docs/scr.png "kbgui Screenshot")

Dependencies
------------

### avr-gcc and avr-libc

To compile your C code to a hex file you need to install avr-gcc and
avr-libc. 

If you are on linux, then a prepackaged version of these prorgams should
be available for your distribution. In a debian-based distribution
(including ubuntu) the following commands will install the required
programs (run as root):

    apt-get install avr-libc gcc-avr

If you are on OS X, then the recommended installation method is through
the homebrew package manager. Specifically type the following in a
terminal:

    brew tap larsimmisch/avr
    brew install avr-libc
    brew install avr-gcc

If you are on Windows the [WinAVR][winavr] software contains binary
versions of the required files. 

### avrdude

To upload new programs to your Kilobot controller, you will need to
install avrdude first (kbprogram is nothing more than a wrapper for
avrdude).

If you are on linux, then a prepackaged version of avrdude should be
available for your distribution. In debian-based distributions
(including ubuntu) execute the following command as root:

    apt-get install avrdude

If you are on OS X, then the recommended installation method is through
the homebrew package manager. Specifically type the following in a
terminal:

    brew install avrdude --with-usb

If you are on Windows the project [WinAVR][winavr] contains an
executable version of avrdude (make sure to copy the avrdude executable
to a directory accessible in your path).


### libftdi and libusb

To send commands to your Kilobot Overhead Controller requires
communicating with the FTDI chip through usb. For this kbsend uses
libftdi and libusb.

If you are on Linux, then your distribution should provide a packaged
version of libftdi and libusb development files. In debian-based
distributions (including ubuntu) execute the following command (as
        root):

    apt-get install libftdi-dev libusb-dev

If you are on OS X, the recommended installation method is through the
homebrew package meneger. Execute the following commands in a terminal:

    brew install libftdi

If you are on windows, first you will need to download the [MSYS][msys]
system from MinGW. Open an MSYS console and execute the following:

    mingw-get install msys-wget msys-unzip

    wget http://sourceforge.net/projects/libusb-win32/files/libusb-win32-releases/1.2.4.0/libusb-win32-bin-1.2.4.0.zip
    unzip libusb-win32-bin-1.2.4.0
    cp libusb-win32-bin-1.2.4.0/bin/x86/libusb0_x86.dll /usr/bin
    cp libusb-win32-bin-1.2.4.0/lib/gcc/* /usr/lib
    cp libusb-win32-bin-1.2.4.0/include/* /usr/include

    wget http://picusb.googlecode.com/files/libftdi-0.18_mingw32.zip
    unzip libftdi-0.18_mingw32.zip
    cp libftdi-0.18_mingw32/dll/libftdi.dll /usr/bin
    cp libftdi-0.18_mingw32/lib/* /usr/lib
    cp libftdi-0.18_mingw32/include/* /usr/include

Compile and Install
-------------------

Once you have installed all the dependencies, you can compile and
install the tools by executing the following commands in a terminal (in
windows, you should use an MSYS terminal):

    make
    make install

[winavr]:http://sourceforge.net/projects/winavr
[avrdude_windows]:http://tomeko.net/other/avrdude/building_avrdude.php
[msys]:http://www.mingw.org/wiki/MSYS
