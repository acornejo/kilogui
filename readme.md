Kilobots Toolchain
==================

This repository contains a few tools to get started with the kilobots.

* kilogui: Graphical frontend to program and send commands to the Kilobots.
* kilolib: Library for basic access to kilobot hardware
* examples: Example programs for the kilobots

Setting up the basic toolchain
------------------------------

To compile programs for the kilobots, you will need to the avr-gcc
compiler. To upload your programs to the overhead controller (and to
reprogram the bootloader for the kilobots), you will need
avrdude.

### Dependencies: avr-gcc and avr-libc (for compiling)

Linux: A prepackaged version of these programs should be available for
your distribution. In a debian-based distribution (including ubuntu) the
following commands will install the required programs (run as root):

    apt-get install avr-libc gcc-avr

OS X: The recommended installation method is through the [homebrew][brew]
package manager. Specifically type the following in a terminal:

    brew tap larsimmisch/avr
    brew install avr-libc
    brew install avr-gcc

Windows: The [WinAVR][winavr] software contains compiled versions of
both avr-gcc and avr-libc. (make sure to add the directory containing
the avr-gcc executables to your path).

### Dependencies: avrdude (for programming)

Linux: A prepackaged version of avrdude should be available for your
distribution. In debian-based distributions (including ubuntu) execute
the following command as root:

    apt-get install avrdude

OS X: The recommended installation method is through the [homebrew][brew]
package manager. Specifically type the following in a terminal:

    brew install avrdude --with-usb

Windows: The [WinAVR][winavr] software ontains an
executable version of avrdude (make sure to add the directory containing
the avrdude executable to your path).

kilogui
-------
Graphical front-end to program and control the kilobots via the overhead controller.

* Binaries for Windows 32 bit [here][kilogui-binary].
* Binaries for OS X 10.8.2 [here][kilogui-binaryosx]

![kilogui-win32](https://raw.github.com/acornejo/kilobots-toolchain/docs/gui-win32.png "kilogui win32 screenshot")
![kilogui-osx](https://raw.github.com/acornejo/kilobots-toolchain/docs/gui-osx.png "kilogui osx screenshot")
![kilogui-linux](https://raw.github.com/acornejo/kilobots-toolchain/docs/gui-linux.png "kilogui linux screenshot")

[brew]:http://brew.sh
[winavr]:http://sourceforge.net/projects/winavr [avrdude_windows]:http://tomeko.net/other/avrdude/building_avrdude.php
[msys]:http://www.mingw.org/wiki/MSYS
[kbsend-binary]:https://github.com/acornejo/kilobots-toolchain/raw/binaries/kbsend-i586-win32.zip 
[kilogui-binary]:https://github.com/acornejo/kilobots-toolchain/raw/binaries/kilogui-i586-win32.zip 
[kilogui-binaryosx]:https://github.com/acornejo/kilobots-toolchain/raw/binaries/kilogui.dmg 
