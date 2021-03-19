#include <SPI.h>
#include <SdFat.h>
#include <SdFatTestSuite.h>

char buf[100];
ibufstream ib;
#define ibInit(s) ibInit_P(PSTR(s))

//----------------------------------------------------------
void ibInit_P(PGM_P p) {
  if (strlen_P(p) >= sizeof(buf)) {
    ib.init("");
    ib.setstate(ios::badbit);
  } else {
    ib.clear();
    strncpy_P(buf, p, sizeof(buf));
    ib.init(buf);
  }
}
//------------------------------------------------------------------------------
void istreamBool() {
  bool b;
  ibInit(" 0 1 2");
  testVerifyBool((ib >> b) && !b);
  testVerifyBool((ib >> b) && b);
  testVerifyBool(!(ib >> b) && !ib.good());

  ibInit(" true false err");
  testVerifyBool((ib >> boolalpha >> b) && b && ib.good());
  testVerifyBool((ib >> b) && !b && ib.good());
  testVerifyBool(!(ib >> b) && ib.fail());

  ibInit("1");
  testVerifyBool((ib >> noboolalpha >> b) && b && ib.eof());
}
//------------------------------------------------------------------------------
void istreamChar() {
  char c;
  signed char sc;
  unsigned char uc;

  ibInit("c s u g");
  testVerifyBool((ib >> c) && ib.good() && c == 'c');
  testVerifyBool((ib >> sc) && ib.good() && sc == 's');
  testVerifyBool((ib >> uc) && ib.good() && uc == 'u');
  testVerifyBool(ib.get() == ' ');
  testVerifyBool(ib.peek() == 'g' && ib.good());
  testVerifyBool(ib.get() == 'g' && ib.good());
  testVerifyBool(ib.get() == -1 && ib.eof());
}
//------------------------------------------------------------------------------
void istreamDouble() {
  double f;
  ibInit("0 .1 1. 2 3.4 .1e5 1e5 -1E6 +2.3e-3 -123.4567");
  testVerifyBool((ib >> f) && f == 0 && ib.good());
  testVerifyBool((ib >> f) && f == 0.1 && ib.good());
  testVerifyBool((ib >> f) && f == 1.0 && ib.good());
  testVerifyBool((ib >> f) && f == 2.0 && ib.good());
  testVerifyBool((ib >> f) && f == 3.4 && ib.good());
  testVerifyBool((ib >> f) && f == 10000.0 && ib.good());
  testVerifyBool((ib >> f) && f == 1e5 && ib.good());
  testVerifyBool((ib >> f) && f == -1E6 && ib.good());
  testVerifyBool((ib >> f) && f == 2.3e-3 && ib.good());
  testVerifyBool((ib >> f) && fabs(f + 123.4567) < 1e-5 && ib.eof());
  if (fabs(f + 123.4567) >= 1e-5) Serial.println(f, 8);
}
//------------------------------------------------------------------------------
void istreamFloat() {
  float f;
  ibInit("0 .1 1. 2 3.4 .1e5 1e5 -1E6 +2.3e-3 -123.4567");
  testVerifyBool((ib >> f) && f == 0 && ib.good());
  testVerifyBool((ib >> f) && f == 0.1f && ib.good());
  testVerifyBool((ib >> f) && f == 1.0 && ib.good());
  testVerifyBool((ib >> f) && f == 2.0 && ib.good());
  testVerifyBool((ib >> f) && f == 3.4f && ib.good());
  testVerifyBool((ib >> f) && f == 10000.0 && ib.good());
  testVerifyBool((ib >> f) && f == 1e5 && ib.good());
  testVerifyBool((ib >> f) && f == -1E6 && ib.good());
  testVerifyBool((ib >> f) && f == 2.3e-3f && ib.good());
  testVerifyBool((ib >> f) && fabs(f + 123.4567f) < 1e-5 && ib.eof());
  if (fabs(f + 123.4567) >= 1e-5) Serial.println(f, 8);
}
//------------------------------------------------------------------------------
void istreamGet() {
  char s[4];
  ibInit("ab c");
  testVerifyBool(ib.get() == 'a' && ib.good() && ib.gcount() == 1);
  testVerifyBool(ib.get() == 'b' && ib.good() && ib.gcount() == 1);
  testVerifyBool(ib.get() == ' ' && ib.good() && ib.gcount() == 1);
  testVerifyBool(ib.get() == 'c' && ib.good() && ib.gcount() == 1);
  testVerifyBool(ib.get() == -1 && ib.eof() && ib.gcount() == 0);

  ibInit("ab\ncdef");
  ib.get(s, sizeof(s));
  testVerifyBool(ib.good() && ib.gcount() == 2);
  testVerifyStr(s, "ab");
  testVerifyBool(ib.get() == '\n' && ib.good() && ib.gcount() == 1);
  ib.get(s, sizeof(s));
  testVerifyBool(ib.good() && ib.gcount() == 3);
  testVerifyStr(s, "cde");
  ib.get(s, sizeof(s));
  testVerifyBool(ib.eof() && ib.gcount() == 1);
  testVerifyStr(s, "f");

  ibInit(
    "short line\n"
    "\n"
    "17 character line\n"
    "too long for buffer\n"
    "line with no nl"
  );
  char buf[18];
  ib.getline(buf, sizeof(buf));
  testVerifyBool(ib.good() && ib.gcount() == 11);
  testVerifyStr(buf, "short line");
  ib.getline(buf, sizeof(buf));
  testVerifyBool(ib.good() && ib.gcount() == 1 && buf[0] == '\0');
  ib.getline(buf, sizeof(buf));
  testVerifyBool(ib.good() && ib.gcount() == 18);
  testVerifyStr(buf, "17 character line");
  ib.getline(buf, sizeof(buf));
  testVerifyBool(ib.fail() && !ib.eof() && ib.gcount() == 17);
  testVerifyStr(buf, "too long for buff");
  ib.clear();
  ib.getline(buf, sizeof(buf));
  testVerifyBool(ib.good() && !ib.eof() && ib.gcount() == 3);
  testVerifyStr(buf, "er");
  ib.getline(buf, sizeof(buf));
  testVerifyBool(!ib.fail() && ib.eof() && ib.gcount() == 15);
  testVerifyStr(buf, "line with no nl");
}
//------------------------------------------------------------------------------
void istreamNumber() {
  short s;
  signed short ss;
  unsigned short us;
  int i;
  signed int si;
  unsigned int ui;
  long l;
  signed long sl;
  unsigned long ul;

  ibInit("-32769");
  testVerifyBool(!(ib >> s) && ib.fail());
  ibInit("-32768 0 32767 32768");
  testVerifyBool((ib >> s) && s == -32768 && ib.good());
  testVerifyBool((ib >> s) && s == 0 && ib.good());
  testVerifyBool((ib >> s) && s == 32767 && ib.good());
  testVerifyBool(!(ib >> s) && ib.fail());

  ibInit("-32769");
  testVerifyBool(!(ib >> ss) && ib.fail());
  ibInit("-32768 0 32767 32768");
  testVerifyBool((ib >> ss) && ss == -32768 && ib.good());
  testVerifyBool((ib >> ss) && ss == 0 && ib.good());
  testVerifyBool((ib >> ss) && ss == 32767 && ib.good());
  testVerifyBool(!(ib >> ss) && ib.fail());

  ibInit("0 65535 65536");
  testVerifyBool((ib >> us) && us == 0 && ib.good());
  testVerifyBool((ib >> us) && us == 65535 && ib.good());
  testVerifyBool(!(ib >> us) && ib.fail());
  
if (sizeof(int) == 2) {
    ibInit("-32769");
    testVerifyBool(!(ib >> i) && ib.fail());
    ibInit("-32768 0 32767 32768");
    testVerifyBool((ib >> i) && i == -32768 && ib.good());
    testVerifyBool((ib >> i) && i == 0 && ib.good());
    testVerifyBool((ib >> i) && i == 32767 && ib.good());
    testVerifyBool(!(ib >> i) && ib.fail());

    ibInit("-32769");
    testVerifyBool(!(ib >> si) && ib.fail());
    ibInit("-32768 0 32767 32768");
    testVerifyBool((ib >> si) && si == -32768 && ib.good());
    testVerifyBool((ib >> si) && si == 0 && ib.good());
    testVerifyBool((ib >> si) && si == 32767 && ib.good());
    testVerifyBool(!(ib >> si) && ib.fail());

    ibInit("0 65535 65536");
    testVerifyBool((ib >> ui) && ui == 0 && ib.good());
    testVerifyBool((ib >> ui) && ui == 65535 && ib.good());
    testVerifyBool(!(ib >> ui) && ib.fail());
  } else {
    ibInit("-2147483649");
    testVerifyBool(!(ib >> i) && ib.fail());
    ibInit("-2147483648 0 2147483647 2147483648");
    testVerifyBool((ib >> i) && i == -2147483648 && ib.good());
    testVerifyBool((ib >> i) && i == 0 && ib.good());
    testVerifyBool((ib >> i) && i == 2147483647 && ib.good());
    testVerifyBool(!(ib >> i) && ib.fail());

    ibInit("-2147483649");
    testVerifyBool(!(ib >> si) && ib.fail());
    ibInit("-2147483648 0 2147483647 2147483648");
    testVerifyBool((ib >> si) && si == -2147483648 && ib.good());
    testVerifyBool((ib >> si) && si == 0 && ib.good());
    testVerifyBool((ib >> si) && si == 2147483647 && ib.good());
    testVerifyBool(!(ib >> si) && ib.fail());

    ibInit("0 4294967295 4294967296");
    testVerifyBool((ib >> ui) && ui == 0 && ib.good());
    testVerifyBool((ib >> ui) && ui == 4294967295 && ib.good());
    testVerifyBool(!(ib >> ui) && ib.fail());
  }
  ibInit("-2147483649");
  testVerifyBool(!(ib >> l) && ib.fail());
  ibInit("-2147483648 0 2147483647 2147483648");
  testVerifyBool((ib >> l) && l == -2147483648 && ib.good());
  testVerifyBool((ib >> l) && l == 0 && ib.good());
  testVerifyBool((ib >> l) && l == 2147483647 && ib.good());
  testVerifyBool(!(ib >> l) && ib.fail());

  ibInit("-2147483649");
  testVerifyBool(!(ib >> sl) && ib.fail());
  ibInit("-2147483648 0 2147483647 2147483648");
  testVerifyBool((ib >> sl) && sl == -2147483648 && ib.good());
  testVerifyBool((ib >> sl) && sl == 0 && ib.good());
  testVerifyBool((ib >> sl) && sl == 2147483647 && ib.good());
  testVerifyBool(!(ib >> sl) && ib.fail());

  ibInit("0 4294967295 4294967296");
  testVerifyBool((ib >> ul) && ul == 0 && ib.good());
  testVerifyBool((ib >> ul) && ul == 4294967295 && ib.good());
  testVerifyBool(!(ib >> ul) && ib.fail());

  // octal hex
  ibInit("123 abc 0xdef 0XABC 567");
  testVerifyBool((ib >> oct >> i) && i == 83);
  testVerifyBool((ib >> hex >> i) && i == 0xabc);
  testVerifyBool((ib >> i) && i == 0xdef);
  testVerifyBool((ib >> i) && i == 0xabc);
  testVerifyBool((ib >> dec >> i) && i ==567);
}
//------------------------------------------------------------------------------
void istreamStr() {
  char str[20];
  ibInit("abc def\r\n hij");
  testVerifyBool((ib >> str) && ib.good());
  testVerifyStr(str, "abc");
  testVerifyBool((ib >> str) && ib.good());
  testVerifyStr(str, "def");
  testVerifyBool((ib >> str) && ib.eof());
  testVerifyStr(str, "hij");
}
//------------------------------------------------------------------------------
void setup() {
  testBegin();
  istreamBool();
  istreamChar();
  istreamDouble();
  istreamFloat();
  istreamGet();
  istreamNumber();
  istreamStr();
  testEnd();
}
//------------------------------------------------------------------------------
void loop() {}