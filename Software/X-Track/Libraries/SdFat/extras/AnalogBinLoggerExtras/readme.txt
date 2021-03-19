AnalogBinLogger.ino logs analog data to a binary SD file at high rates.

Samples are logged at regular intervals by using timer1.  Timer/Counter1
Compare Match B is used to trigger the ADC for the first pin in a sample.
The ADC is triggered for remaining sample pins in the ADC conversion complete
interrupt routine.

Data is captured in the ADC interrupt routine and saved in 512 byte buffers.

Buffered data is written to the SD in a function called from loop().  The
entire data set is written to a large contiguous file as a single multi-block
write.  This reduces write latency problems.

Many inexpensive SD cards work well at lower rates.  I used a $6.00
SanDisk 4 GB class 4 card for testing.

SanDisk class 4 cards work well at fairly high rates.  I used the 4 GB SanDisk
card to log a single pin at 40,000 samples per second.

You may need to increase the time between samples if your card has higher
latency.  Using a Mega Arduino can help since it has more buffering.

The bintocsv folder contains a PC program for converting binary files to
CSV files.  Build it from the included source files.  bintocvs is a command line program.

bintocsv binFile csvFile 

AnalogBinLogger requires a recent version of the SdFat library.  The SdFat
folder contains a beta version I used for development.

The latest stable version is here:
http://code.google.com/p/sdfatlib/downloads/list

You also need to install the included BufferedWriter library.  It provides
fast text formatting.

Example data for a 2 kHz sine wave logged at 40,000 samples per second is
shown in DATA.PNG and FFT.PNG shows a FFT of the data.  See ExcelFFT.pdf
in the ADCdocs folder for details on calculating a FFT.

The accuracy of the ADC samples depends on the ADC clock rate.  See the
ADC_ENOB.PNG file for a plot of accuracy vs ADC clock frequency.

See files in the ADCdocs folder for more information on ADC accuracy.

To modify this program you will need a good knowledge of the Arduino
ADC, timer1 and C++ programming.  This is not for the newbie.

I have an LED and resistor connected to pin 3 to signal fatal errors and
data overruns. Fatal errors are indicated by a blinking led.  Overrun errors
are indicated by a solid lit led.  The count of samples dropped is written
to the SD and data logging continues.

You can disable the error led feature by setting the error pin number negative:

To use AnalogBinLogger, install these items. 

Place the BufferWriter and SdFat folders in your sketchbook libraries folder.

Place the AnalogIsrLogger folder in your sketchbook folder.

You must edit the configuration constants at the beginning of the program 
to set the sample pins, sample rate, and other configuration values.

Initially the program is setup to log the first five analog pins at 5000
samples per second.  Change these values to suit your needs.

See RateTable.txt for maximum allowed sample rates vs pin count and ADC clock
frequency.

The program has four commands:

c - convert file to CSV
d - dump data to Serial
e - overrun error details
r - record ADC data

All commands can be terminated by entering a character from the serial monitor.

The c command converts the current binary file to a text file.  Entering a
character on the serial monitor terminates the command.

The d command converts the binary file to text and displays it on the serial
monitor.  Entering a character on the serial monitor terminates the command.

The e command displays details about overruns in the current binary file.
Data overruns happen when data samples are lost due to long write latency
of the SD.

The r command will record ADC data to a binary file.  It will terminate
when a character is entered on the serial monitor or the the maximum file
block count has been reached.

A number of program options can be set by changing constants at the beginning
of the program.  