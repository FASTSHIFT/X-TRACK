// modified from ArduinoTestSuite 0022 by William Greiman
// Tests writing to and reading from a file, in particular the
// the Stream implementation (e.g. read() and peek()).
#include <SPI.h>
#include <SdFat.h>
#include <SdFatTestSuite.h>
SdFat SD;
#define ATS_PrintTestStatus(msg, b) testVerify_P(b, PSTR(msg))

void setup() {
  boolean b;
  SdFile f;

  testBegin();

  ATS_PrintTestStatus("SD.begin()", b = SD.begin());
  if (!b) goto done;

  SD.remove("test.txt");

  f.open("test.txt", FILE_WRITE);
  ATS_PrintTestStatus("SD.open()", f.isOpen());
  if (!f.isOpen()) goto done;

  ATS_PrintTestStatus("initial position", f.curPosition() == 0);
  ATS_PrintTestStatus("initial size", f.fileSize() == 0);

  f.print("0123456789");

  ATS_PrintTestStatus("position after writing", f.curPosition() == 10);
  ATS_PrintTestStatus("size after writing", f.fileSize() == 10);

  f.seekSet(0);

  ATS_PrintTestStatus("size after seek", f.fileSize() == 10);
  ATS_PrintTestStatus("position after seek", f.curPosition() == 0);

  f.seekSet(7);

  ATS_PrintTestStatus("position after seek", f.curPosition() == 7);
  ATS_PrintTestStatus("reading after seek", f.read() == '7');
  ATS_PrintTestStatus("position after reading after seeking", f.curPosition() == 8);
  ATS_PrintTestStatus("reading after reading after seeking", f.read() == '8');

  f.seekSet(3);

  ATS_PrintTestStatus("position after seeking", f.curPosition() == 3);
  ATS_PrintTestStatus("peeking after seeking", f.peek() == '3');
  ATS_PrintTestStatus("position after peeking after seeking", f.curPosition() == 3);
  ATS_PrintTestStatus("peeking after peeking after seeking", f.peek() == '3');
  ATS_PrintTestStatus("position after peeking after seeking", f.curPosition() == 3);
  ATS_PrintTestStatus("peeking after peeking after seeking", f.read() == '3');
  ATS_PrintTestStatus("position after peeking after seeking", f.curPosition() == 4);

  f.seekSet(1);

  ATS_PrintTestStatus("position after seeking", f.curPosition() == 1);
  ATS_PrintTestStatus("peeking after seeking", f.peek() == '1');

  f.seekSet(4);

  ATS_PrintTestStatus("position after seeking", f.curPosition() == 4);
  ATS_PrintTestStatus("peeking after seeking", f.peek() == '4');

  f.seekSet(7);

  ATS_PrintTestStatus("position()", f.curPosition() == 7);
  ATS_PrintTestStatus("read()", f.read() == '7');

  f.seekSet(0);
  f.peek();
  f.print("AB");

  ATS_PrintTestStatus("position()", f.curPosition() == 2);
  ATS_PrintTestStatus("size()", f.fileSize() == 10);
  ATS_PrintTestStatus("read()", f.read() == '2');

  f.seekSet(0);

  ATS_PrintTestStatus("read()", f.read() == 'A');
  ATS_PrintTestStatus("read()", f.read() == 'B');
  ATS_PrintTestStatus("read()", f.read() == '2');

  f.close();

  f.open("test.txt", O_READ);
  ATS_PrintTestStatus("SD.open()", f.isOpen());
  if (!f.isOpen()) goto done;

  ATS_PrintTestStatus("position()", f.curPosition() == 0);
  ATS_PrintTestStatus("size()", f.fileSize() == 10);
  ATS_PrintTestStatus("peek()", f.peek() == 'A');
  ATS_PrintTestStatus("read()", f.read() == 'A');

  f.seekSet(4);

  ATS_PrintTestStatus("position()", f.curPosition() == 4);
  ATS_PrintTestStatus("size()", f.fileSize() == 10);
  ATS_PrintTestStatus("peek()", f.peek() == '4');
  ATS_PrintTestStatus("read()", f.read() == '4');

  f.close();

done:
  testEnd();
}

void loop() {}