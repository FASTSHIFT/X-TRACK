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
  uint32_t fs;

  testBegin();

  ATS_PrintTestStatus("SD.begin()", b = SD.begin());
  if (!b) goto done;

  SD.remove("test.txt");

  f.open("test.txt", FILE_WRITE);
  ATS_PrintTestStatus("SD.open()", f.isOpen());
  if (!f.isOpen()) goto done;

  f.print("abc");
  f.print("de");
  f.close();

  f.open("test.txt", FILE_WRITE);
  ATS_PrintTestStatus("SD.open()", f.isOpen());
  if (!f.isOpen()) goto done;

  f.print("fgh");
  f.close();

  f.open("test.txt", O_READ);
  ATS_PrintTestStatus("SD.open()", f.isOpen());
  if (!f.isOpen()) goto done;
  fs =f.fileSize();
  ATS_PrintTestStatus("read()", f.read() == 'a');
  ATS_PrintTestStatus("peek()", f.peek() == 'b');
  ATS_PrintTestStatus("read()", f.read() == 'b');
  ATS_PrintTestStatus("read()", f.read() == 'c');
  ATS_PrintTestStatus("peek()", f.peek() == 'd');
  ATS_PrintTestStatus("peek()", f.peek() == 'd');
  ATS_PrintTestStatus("peek()", f.peek() == 'd');
  ATS_PrintTestStatus("peek()", f.peek() == 'd');
  ATS_PrintTestStatus("read()", f.read() == 'd');
  ATS_PrintTestStatus("available()", f.curPosition() != fs);
  ATS_PrintTestStatus("read()", f.read() == 'e');
  ATS_PrintTestStatus("available()", f.curPosition() != fs);
  ATS_PrintTestStatus("peek()", f.peek() == 'f');
  ATS_PrintTestStatus("read()", f.read() == 'f');
  ATS_PrintTestStatus("peek()", f.peek() == 'g');
  ATS_PrintTestStatus("available()", f.curPosition() != fs);
  ATS_PrintTestStatus("peek()", f.peek() == 'g');
  ATS_PrintTestStatus("read()", f.read() == 'g');
  ATS_PrintTestStatus("available()", f.curPosition() != fs);
  ATS_PrintTestStatus("available()", f.curPosition() != fs);
  ATS_PrintTestStatus("available()", f.curPosition() != fs);
  ATS_PrintTestStatus("peek()", f.peek() == 'h');
  ATS_PrintTestStatus("read()", f.read() == 'h');
  ATS_PrintTestStatus("available()", f.curPosition() == fs);
  ATS_PrintTestStatus("peek()", f.peek() == -1);
  ATS_PrintTestStatus("read()", f.read() == -1);
  ATS_PrintTestStatus("peek()", f.peek() == -1);
  ATS_PrintTestStatus("read()", f.read() == -1);

  f.close();

  SD.remove("test2.txt");

  f.open("test2.txt", FILE_WRITE);
  ATS_PrintTestStatus("SD.open()", f.isOpen());
  if (!f.isOpen()) goto done;

  f.print("ABC");
  f.close();

  f.open("test.txt", O_READ);
  ATS_PrintTestStatus("SD.open()", f.isOpen());
  if (!f.isOpen()) goto done;

  ATS_PrintTestStatus("peek()", f.peek() == 'a');

  f.close();

  f.open("test2.txt", O_READ);
  ATS_PrintTestStatus("SD.open()", f.isOpen());
  if (!f.isOpen()) goto done;

  ATS_PrintTestStatus("peek()", f.peek() == 'A');
  ATS_PrintTestStatus("read()", f.read() == 'A');

  f.close();

done:
  testEnd();

}

void loop() {}


