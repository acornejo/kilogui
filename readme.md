Kilobots Toolchain
==================

This repository contains four command line tools and a graphical
frontend.

* kilogui: Graphical frontend to program and send commands to the Kilobots.
* kbcompile: Compile your C code to a hex file.
* kbmerge: Merge controller and program hex files to a single Overhead Controller hex file.
* kbprogram: Upload hex file to Kilobot Overhead Controller.
* kbsend: Sends commands to Kilobot Overhead Controller.


kilogui
-------
Graphical front-end to program the kilobot overhead controller (handles
merging of control and program file), and sending commands to
the kilobots through the overhead controller.

* Binaries for Windows 32 bit [here][kilogui-binary].
* Binaries for OS X 10.8.2 [here][kilogui-binaryosx]

### Dependencies: avrdude (for programming)

Linux: A prepackaged version of avrdude should be available for your
distribution. In debian-based distributions (including ubuntu) execute
the following command as root:

    apt-get install avrdude

OS X: The recommended installation method is through the homebrew
package manager. Specifically type the following in a terminal:

    brew install avrdude --with-usb

Windows: The [WinAVR][winavr] software ontains an
executable version of avrdude (make sure to add the directory containing
the avrdude executable to your path).

![kilogui-win32](https://raw.github.com/acornejo/kilobots-toolchain/docs/gui-win32.png "kilogui win32 screenshot")
![kilogui-osx](https://raw.github.com/acornejo/kilobots-toolchain/docs/gui-osx.png "kilogui osx screenshot")
![kilogui-linux](https://raw.github.com/acornejo/kilobots-toolchain/docs/gui-linux.png "kilogui linux screenshot")

### Compiling

In a terminal console do the following:

    git clone https://github.com/acornejo/kilobots-toolchain.git
    cd kilobots-toolchain
    make 

kbcompile
----------

This is a small bash script wrapper that allows you to compile your C
code to a hex file. If you are on windows, you probably want to use
[WinAVR][winavr] instead.

Usage:

    kbcompile {program.hex} {source1.c} {source2.c}


### Dependencies: avr-gcc and avr-libc

Linux: A prepackaged version of these programs should be available for
your distribution. In a debian-based distribution (including ubuntu) the
following commands will install the required programs (run as root):

    apt-get install avr-libc gcc-avr

OS X: The recommended installation method is through the homebrew
package manager. Specifically type the following in a terminal:

    brew tap larsimmisch/avr
    brew install avr-libc
    brew install avr-gcc

Windows: The [WinAVR][winavr] software contains compiled versions of
both avr-gcc and avr-libc. (make sure to add the directory containing
the avr-gcc executalbes to your path).

kbprogram
---------

This is a small bash script wrapper that allows you to upload a hex
file to the Kilobot Overhead Controller. IMPORTANT: Remember that you
should only upload a hex file which has both a program and a controller,
see kbmerge.

Usage:

    kbprogram {file.hex}


### Dependencies: avrdude

Linux: A prepackaged version of avrdude should be available for your
distribution. In debian-based distributions (including ubuntu) execute
the following command as root:

    apt-get install avrdude

OS X: The recommended installation method is through the homebrew
package manager. Specifically type the following in a terminal:

    brew install avrdude --with-usb

Windows: The [WinAVR][winavr] software ontains an
executable version of avrdude (make sure to add the directory containing
the avrdude executable to your path).


kbsend
-------
The kbsend program that sends commands to your Kilobot Overhead
Controller requires communicating with the FTDI chip through usb. 


Usage:

    kbsend {command}

Where {command} is one of the following

    Bootload, Sleep, Wakeup, Pause, Voltage, Run, Charge, Stop,
    LedsToggle, BootloadMsg, Reset

### Dependencies: libftdi and libusb

Linux: A packaged version of libftdi and libusb should be available for
your distribution. In debian-based distributions (including ubuntu)
execute the following command (as root):

    apt-get install libftdi-dev libusb-dev

OS X: The recommended installation method is through the
homebrew package meneger. Execute the following commands in a terminal:

    brew install libftdi

Windows : There are ready made binaries [here][kbsend-binary]. If you
want to build your own version first you will need to download the
[MSYS][msys] system from MinGW. Open an MSYS console and execute the
following:

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

kbmerge
--------
The kbmerge program merges together the Kilobot controller file, and
your kilobot program. This step is required before using kbprogram to
upload the resulting hex file to the kilobots.

Usage:

    kbmerge {control.hex} {program.hex} {output.hex}

### Dependencies: None

[winavr]:http://sourceforge.net/projects/winavr
[avrdude_windows]:http://tomeko.net/other/avrdude/building_avrdude.php
[msys]:http://www.mingw.org/wiki/MSYS
[kbsend-binary]:https://github.com/acornejo/kilobots-toolchain/raw/binaries/kbsend-i586-win32.zip 
[kilogui-binary]:https://github.com/acornejo/kilobots-toolchain/raw/binaries/kilogui-i586-win32.zip 
[kilogui-binaryosx]:https://github.com/acornejo/kilobots-toolchain/raw/binaries/kilogui.dmg 
