Kilobots Toolchain
==================

This repository contains four tools:

* kprogram: Wrapper for avrdude for flashing Kilobot controller.
* kmerge: Merges a controller and a program file into a single Kilobot controller file.
* ksend: Sends commands to Kilobot controller (requires libftdi).
* kgui: PyGTK front-end for the previous three programs.

Dependencies
------------

### avrdude

To upload new programs to your Kilobot controller, you will need to
install avrdude first (kprogram is nothing more than a wrapper for
avrdude).

If you are on linux, then a prepackaged version of avrdude should be
available for your distribution. In debian-based distributions
(including ubuntu) execute the following command as root:

    apt-get install avrdude

If you are on OS X, then the recommended installation method is through
the homebrew package manager. Specifically type the following in a
terminal:

    brew install avrdude --with-usb

If you are on Windows the project
[WinAVR][winavr] contains an executable
version of avrdude (make sure to place the avrdude executable on your
path). To compile your own version follow the instructions
[here][avrdude_windows].


### libftdi and libusb

To send commands to your Kilobot controller requires communicating with
the FTDI chip through usb. For this ksend uses libftdi and libusb, which
are available for Windows, Linux and OS X.

If you are on Linux, then your distribution should provide a packaged
version of libftdi and libusb development files. In debian-based
distributions (including ubuntu) execute the following command (as
        root):

    apt-get install libftdi-dev libusb-dev

If you are on OS X, the recommended installation method is through the
homebrew package meneger. Execute the following commands in a terminal:

    brew install libftdi


If you are on windows, first you will need to download the
[MSYS][msys] system from MinGW. Open an MSYS
console and execute the following:

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
[avrdude_windwos]:http://tomeko.net/other/avrdude/building_avrdude.php
[msys]:http://www.mingw.org/wiki/MSYS
