# AVRDUDE for Windows

[AVRDUDE](https://www.nongnu.org/avrdude/) is a utility to download/upload/manipulate
the ROM and EEPROM contents of AVR microcontrollers using the in-system programming
technique (ISP).

This port can be built natively under Windows with Visual C++ compiler (no more
[crosscompiling](https://github.com/arduino/avrdude-build-script#windows-cross-compile-from-linux-requirements) with MinGW or Cygwin).

This port has no external dependencies. The FTDI CDM drivers are required when using
FTDI based programmers (_avrftdi_ or _ftdi_syncbb_). No dependencies on _libusb_ or _libftdi_.

## Download

Latest release can be downloaded from the [release page](https://github.com/manison/avrdude/releases/).

## System Requirements

* Windows XP or newer

## Build Prerequisities

* [Visual Studio 2019 Community](https://visualstudio.microsoft.com/) or higher with C++ desktop development workload

## Build Instructions

* Open solution file _avrdude.sln_
* Hit _Build Solution_

## References

* [Project homepage](https://www.nongnu.org/avrdude/)
* [Documentation](https://www.nongnu.org/avrdude/user-manual/avrdude.html)
* [Project page at Savannah](http://savannah.nongnu.org/projects/avrdude)
