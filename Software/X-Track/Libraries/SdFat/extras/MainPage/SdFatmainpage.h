/**
 * Copyright (c) 20011-2018 Bill Greiman
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
\mainpage Arduino %SdFat Library
<CENTER>Copyright &copy 2012-2018 by William Greiman
</CENTER>

\section Intro Introduction
The Arduino %SdFat Library is a minimal implementation of FAT16 and FAT32
file systems on SD flash memory cards. Standard SD and high capacity SDHC
cards are supported.

Experimental support for FAT12 can be enabled by setting FAT12_SUPPORT
nonzero in SdFatConfig.h.

The %SdFat library supports Long %File Names or short 8.3 names.
Edit the SdFatConfig.h file to select short or long file names.

The main classes in %SdFat are SdFat, SdFatEX, SdFatSoftSpi, SdFatSoftSpiEX,
SdBaseFile, SdFile, File, StdioStream, \ref fstream, \ref ifstream,
and \ref ofstream.

The SdFat, SdFatEX, SdFatSoftSpi and SdFatSoftSpiEX classes maintain a
FAT volume, a current working directory, and simplify initialization
of other classes. The SdFat and SdFatEX classes uses a fast custom hardware SPI
implementation.  The SdFatSoftSpi and SdFatSoftSpiEX classes uses software SPI.

the SdFatEX and SdFatSoftSpiEX use extended multi-block I/O for enhanced
performance.  These classes must have exclusive use of the SPI bus.

The SdBaseFile class provides basic file access functions such as open(),
binary read(), binary write(), close(), remove(), and sync(). SdBaseFile
is the smallest file class.

The SdFile class has all the SdBaseFile class functions plus the Arduino
Print class functions.

The File class has all the SdBaseFile functions plus the functions in
the Arduino SD.h File class. This provides compatibility with the
Arduino SD.h library.

The StdioStream class implements functions similar to Linux/Unix standard
buffered input/output.

The \ref fstream class implements C++ iostreams for both reading and writing
text files.

The \ref ifstream class implements C++ iostreams for reading text files.

The \ref ofstream class implements C++ iostreams for writing text files.

The classes \ref ifstream, \ref ofstream, \ref istream, and \ref ostream
follow the C++ \ref iostream standard when possible.

There are many tutorials and much documentation about using C++ iostreams
on the web.

http://www.cplusplus.com/  is a good C++ site for learning iostreams.

The classes \ref ibufstream and \ref obufstream format and parse character
 strings in memory buffers.

the classes ArduinoInStream and ArduinoOutStream provide iostream functions
for Serial, LiquidCrystal, and other devices.

A number of example are provided in the %SdFat/examples folder.  These were
developed to test %SdFat and illustrate its use.

\section Install Installation

You must manually install SdFat by copying the SdFat folder from the download
package to the Arduino libraries folder in your sketch folder.

See the Manual installation section of this guide.

http://arduino.cc/en/Guide/Libraries

\section SDconfig SdFat Configuration

Several configuration options may be changed by editing the SdFatConfig.h
file in the %SdFat folder.

Set USE_LONG_FILE_NAMES nonzero to enable Long %File Names.  By default,
Long %File Names are enabled. For the leanest fastest library disable
Long %File Names.  Long %File names require extra flash but no extra RAM.
Opening Long %File Names can be slower than opening Short %File Names.
Data read and write performance is not changed by the type of %File Name.

If the symbol ENABLE_EXTENDED_TRANSFER_CLASS is nonzero, the class SdFatEX
will be defined. If the symbol ENABLE_SOFTWARE_SPI_CLASS is also nonzero,
the class SdFatSoftSpiEX will be defined.
These classes used extended multi-block SD I/O for better performance.
the SPI bus may not be shared with other devices in this mode.

Set USE_STANDARD_SPI_LIBRARY and  ENABLE_SOFTWARE_SPI_CLASS to
enable various SPI options. set USE_STANDARD_SPI_LIBRARY to use the standard
Arduino SPI library. set ENABLE_SOFTWARE_SPI_CLASS to enable the SdFatSoftSpi
class which uses software SPI.

To enable SD card CRC checking set USE_SD_CRC nonzero.

Set FAT12_SUPPORT nonzero to enable use of FAT12 volumes.
FAT12 has not been well tested and requires additional flash.

\section SDPath Paths and Working Directories

Relative paths in SdFat are resolved in a manner similar to Windows.

Each instance of SdFat has a current directory.  In SdFat this directory
is called the volume working directory, vwd.  Initially this directory is
the root directory for the volume.

The volume working directory is changed by calling SdFat::chdir(path).

The call sd.chdir("/2014") will change the volume working directory
for sd to "/2014", assuming "/2014" exists.

Relative paths for SdFat member functions are resolved by starting at
the volume working directory.

For example, the call sd.mkdir("April") will create the directory
"/2014/April" assuming the volume working directory is "/2014".

SdFat has a current working directory, cwd, that is used to resolve paths
for file.open() calls.

For a single SD card the current working directory is always the volume
working directory for that card.

For multiple SD cards the current working directory is set to the volume
working directory of a card by calling the SdFat::chvol() member function.
The chvol() call is like the Windows \<drive letter>: command.

The call sd2.chvol() will set the current working directory to the volume
working directory for sd2.

If the volume working directory for sd2 is "/music" the call

file.open("BigBand.wav", O_READ);

will then open "/music/BigBand.wav" on sd2.

The following functions are used to change or get current directories.
See the html documentation for more information.
@code
bool SdFat::chdir(bool set_cwd = false);
bool SdFat::chdir(const char* path, bool set_cwd = false);
void SdFat::chvol();
SdBaseFile* SdFat::vwd();
static SdBaseFile* SdBaseFile::cwd();
@endcode

\section SDcard SD\SDHC Cards

Arduinos access SD cards using the cards SPI protocol.  PCs, Macs, and
most consumer devices use the 4-bit parallel SD protocol.  A card that
functions well on A PC or Mac may not work well on the Arduino.

Most cards have good SPI read performance but cards vary widely in SPI
write performance.  Write performance is limited by how efficiently the
card manages internal erase/remapping operations.  The Arduino cannot
optimize writes to reduce erase operations because of its limit RAM.

SanDisk cards generally have good write performance.  They seem to have
more internal RAM buffering than other cards and therefore can limit
the number of flash erase operations that the Arduino forces due to its
limited RAM.

\section Hardware Hardware Configuration

%SdFat was developed using an
<A HREF = "http://www.adafruit.com/"> Adafruit Industries</A>
Data Logging Shield.

The hardware interface to the SD card should not use a resistor based level
shifter.  %SdFat sets the SPI bus frequency to 8 MHz which results in signal
rise times that are too slow for the edge detectors in many newer SD card
controllers when resistor voltage dividers are used.

The 5 to 3.3 V level shifter for 5 V Arduinos should be IC based like the
74HC4050N based circuit shown in the file SdLevel.png.  The Adafruit Wave Shield
uses a 74AHC125N.  Gravitech sells SD and MicroSD Card Adapters based on the
74LCX245.

If you are using a resistor based level shifter and are having problems try
setting the SPI bus frequency to 4 MHz.  This can be done by using
card.init(SPI_HALF_SPEED) to initialize the SD card.

A feature to use software SPI is available.  Software SPI is slower
than hardware SPI but allows any digital pins to be used.  See
SdFatConfig.h for software SPI definitions.

\section comment Bugs and Comments

If you wish to report bugs or have comments, send email to
fat16lib@sbcglobal.net.  If possible, include a simple program that illustrates
the bug or problem.

\section Trouble Troubleshooting

The two example programs QuickStart, and SdInfo are useful for troubleshooting.

A message like this from SdInfo with errorCode 0X1 indicates the SD card
is not seen by SdFat.  This is often caused by a wiring error and reformatting
the card will not solve the problem.
<PRE>
cardBegin failed
SD errorCode: 0X1
SD errorData: 0XFF
</PRE>
Here is a similar message from QuickStart:
<PRE>
SD initialization failed.
Do not reformat the card!
Is the card correctly inserted?
Is chipSelect set to the correct value?
Does another SPI device need to be disabled?
Is there a wiring/soldering problem?

errorCode: 0x1, errorData: 0xff
</PRE>
Here is a message from QuickStart that indicates a formatting problem:
<PRE>
Card successfully initialized.
Can't find a valid FAT16/FAT32 partition.
Try reformatting the card.  For best results use
the SdFormatter program in SdFat/examples or download
and use SDFormatter from www.sdcard.org/downloads.
</PRE>

The best source of recent information and help is the Arduino forum.

http://arduino.cc/forum/

Also search the Adafruit forum.

http://forums.adafruit.com/

If you are using a Teensy try.

http://forum.pjrc.com/forum.php

\section SdFatClass SdFat Usage

SdFat supports Long File Names.  Long names in SdFat are limited to 7-bit
ASCII characters in the range 0X20 - 0XFE The following are reserved characters:
    <ul>
    <li>< (less than)
    <li>> (greater than)
    <li>: (colon)
    <li>" (double quote)
    <li>/ (forward slash)
    <li>\ (backslash)
    <li>| (vertical bar or pipe)
    <li>? (question mark)
    <li>* (asterisk)
    </ul>
%SdFat uses a slightly restricted form of short names.
Short names are limited to 8 characters followed by an optional period (.)
and extension of up to 3 characters.  The characters may be any combination
of letters and digits.  The following special characters are also allowed:

$ % ' - _ @ ~ ` ! ( ) { } ^ # &

Short names are always converted to upper case and their original case
value is lost.  Files that have a base-name where all characters have the
same case and an extension where all characters have the same case will
display properly.  Examples this type name are UPPER.low, lower.TXT,
UPPER.TXT, and lower.txt.

An application which writes to a file using print(), println() or
write() must close the file or call sync() at the appropriate time to
force data and directory information to be written to the SD Card.

Applications must use care calling sync()
since 2048 bytes of I/O is required to update file and
directory information.  This includes writing the current data block, reading
the block that contains the directory entry for update, writing the directory
block back and reading back the current data block.

It is possible to open a file with two or more instances of a file object.
A file may be corrupted if data is written to the file by more than one
instance of a file object.

\section HowTo How to format SD Cards as FAT Volumes

The best way to restore an SD card's format on a PC or Mac is to use
SDFormatter which can be downloaded from:

http://www.sdcard.org/downloads

A formatter program, SdFormatter.ino, is included in the
%SdFat/examples/SdFormatter directory.  This program attempts to
emulate SD Association's SDFormatter.

SDFormatter aligns flash erase boundaries with file
system structures which reduces write latency and file system overhead.

The PC/Mac SDFormatter does not have an option for FAT type so it may format
very small cards as FAT12.  Use the SdFat formatter to force FAT16
formatting of small cards.

Do not format the SD card with an OS utility, OS utilities do not format SD
cards in conformance with the SD standard.

You should use a freshly formatted SD card for best performance.  FAT
file systems become slower if many files have been created and deleted.
This is because the directory entry for a deleted file is marked as deleted,
but is not deleted.  When a new file is created, these entries must be scanned
before creating the file.  Also files can become
fragmented which causes reads and writes to be slower.

\section ExampleFilder Examples

A number of examples are provided in the SdFat/examples folder.
See the html documentation for a list.

To access these examples from the Arduino development environment
go to:  %File -> Examples -> %SdFat -> \<program Name\>

Compile, upload to your Arduino and click on Serial Monitor to run
the example.

Here is a list:

AnalogBinLogger - Fast AVR ADC logger - see the AnalogBinLoggerExtras folder.

bench - A read/write benchmark.

dataLogger - A simple modifiable data logger.

DirectoryFunctions - Demo of chdir(), ls(), mkdir(), and  rmdir().

fgets - Demo of the fgets read line/string function.

formating - Print a table with various formatting options.

getline - Example of getline from section 27.7.1.3 of the C++ standard.

LongFileName - Example use of openNext, printName, and open by index.

LowLatencyLogger - A data logger for higher data rates. ADC version.

LowLatencyLoggerADXL345 - A data logger for higher data rates. ADXL345 SPI.

LowLatencyLoggerMPU6050 - A data logger for higher data rates. MPU6050 I2C.

OpenNext - Open all files in the root dir and print their filename.

PrintBenchmark - A simple benchmark for printing to a text file.

QuickStart - A program to quickly test your SD card and SD shield/module.

RawWrite - A test of raw write functions for contiguous files.

ReadCsv - Function to read a CSV text file one field at a time.

ReadCsvStream - Read a comma-separated value file using iostream extractors.

ReadCsvArray - Read a two dimensional array from a CSV file.

ReadWrite - Compatibility test of Arduino SD ReadWrite example.

rename - A demo of SdFat::rename(old, new) and SdFile::rename(dirFile, newPath).

SdFormatter - This program will format an SD or SDHC card.

SoftwareSpi - Simple demonstration of the SdFatSoftSpi template class.

SdInfo - Initialize an SD card and analyze its structure for trouble shooting.

StdioBench - Demo and test of stdio style stream.

Timestamp - Sets file create, modify, and access timestamps.

TwoCards - Example using two SD cards.

VolumeFreeSpace - Demonstrate the freeClusterCount() call.

wipe - Example to wipe all data from an already formatted SD.
 */
