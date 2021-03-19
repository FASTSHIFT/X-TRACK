#include <SPI.h>
#include <SdFat.h>
#include <SdFatTestSuite.h>
//------------------------------------------------------------------------------
void ostreamBool() {
  char buf[40];
  obufstream ob(buf, sizeof(buf));
  bool f = false;
  bool t = true;
  ob << t << ',' << f << ',' << setw(2) << t << ',' << left << setw(2) << f;
  testVerifyStr(buf, "1,0, 1,0 ");

  ob.init(buf, sizeof(buf));
  ob << boolalpha << t << ',' << f << ',' << setw(5) << t;
  ob << ',' << right << setw(6) << f;
  testVerifyStr(buf, "true,false,true , false");
}
//------------------------------------------------------------------------------
void ostreamChar() {
  char buf[40];
  obufstream ob(buf, sizeof(buf));
  char c = 'c';
  signed char sc = 's';
  unsigned char uc = 'u';
  ob <<'l' << c << sc << uc;
  ob.put('p');
  testVerifyStr(buf, "lcsup");
  
  ob.init(buf, sizeof(buf));
  ob << 's' << setw(2) << 'r' << 'n' << left << setw(2) << 'l';
  ob << 'm' << right << setw(2) << 'e';
  testVerifyStr(buf, "s rnl m e");
  
  ob.init(buf, sizeof(buf));
  ob << setfill('f') << setw(5) << 'r';
  testVerifyStr(buf, "ffffr");
}
//------------------------------------------------------------------------------
void ostreamFloat() {
  char buf[50];
  obufstream ob(buf, sizeof(buf));
  float f = 9.87654;
  double d = -123.4567;
  ob << f <<',';
  ob << internal << setw(10) << d << ',';
  ob << setfill('0') << setw(10) << d;
  testVerifyStr(buf, "9.88,-   123.46,-000123.46");
  
  ob.init(buf, sizeof(buf));
  ob << setw(10) << left << d << ',' << showpos << -d << ',';
  ob << setprecision(0) << d;
  testVerifyStr(buf, "-123.46000,+123.46,-123");
  
  ob.init(buf, sizeof(buf));
  ob << showpoint << d << noshowpoint << ',' << d << ',';
  ob << setprecision(4)  << f << ',' << setprecision(2) << noshowpos << f;
  testVerifyStr(buf, "-123.,-123,+9.8765,9.88");
}
//------------------------------------------------------------------------------
void ostreamNumber() {
  char buf[50];
  obufstream ob(buf, sizeof(buf));
  
  short s = 1;
  short signed ss = 2;
  short unsigned su = 3;
  int i = 4;
  int signed is = 5;
  int unsigned iu = 6;
  long l = 7;
  long signed ls = 8;
  long unsigned lu = 9;


  ob << s << ss << su << i << is << iu << l <<ls << lu;
  s  = -1;
  ss = -2;
  i  = -3;
  is = -4;
  l  = -5;
  ls = -6;
  ob << s << ss  << i << is << l <<ls;
  testVerifyStr(buf, "123456789-1-2-3-4-5-6");

  ob.init(buf, sizeof(buf));
  s = ss = 0X8000;
  i = is = sizeof(int) == 2 ? 0X8000 : 0X80000000;
  l = ls = 0X80000000;
  su = iu = lu = 0;
  ob << su << iu << lu << s << ss;
  testVerifyStr(buf, "000-32768-32768");
  ob.init(buf, sizeof(buf));
  ob << i << is;
  if (sizeof(int) == 2) {
    testVerifyStr(buf, "-32768-32768");
  } else {
    testVerifyStr(buf, "-2147483648-2147483648");
  }
  ob.init(buf, sizeof(buf));
  ob << l << ls;
  testVerifyStr(buf, "-2147483648-2147483648");

  ob.init(buf, sizeof(buf));
  s = ss = i = is = 0X7FFF;
  l = ls = 0X7FFFFFFF;
  su = iu = 0XFFFF;
  lu = 0XFFFFFFFF;
  ob << showpos << s << ss << i << is;
  testVerifyStr(buf, "+32767+32767+32767+32767");

  ob.init(buf, sizeof(buf));
  ob << l << ls;
  testVerifyStr(buf, "+2147483647+2147483647");
  
  ob.init(buf, sizeof(buf));
  ob << su << iu << lu;
  testVerifyStr(buf, "+65535+65535+4294967295");
  
  i = 0XABC;
  ob.init(buf, sizeof(buf));
  ob << noshowpos << hex << i << ',' << dec << i << ',' << oct << i;
  testVerifyStr(buf, "abc,2748,5274");
  
  ob.init(buf, sizeof(buf));
  ob << showbase << hex << i << ',' << dec << i << ',' << oct << i;
  testVerifyStr(buf, "0xabc,2748,05274");
  
  ob.init(buf, sizeof(buf));
  ob << showpos << uppercase << hex << i << ',' << dec << i << ',' << oct << i;
  testVerifyStr(buf, "0XABC,+2748,05274");
  
  is = -123;
  ob.init(buf, sizeof(buf));
  ob << internal << setfill('0');
  ob << setw(6) << hex << i << ',' << dec << setw(6) << i << ',';
  ob << setw(5) << is;
  testVerifyStr(buf, "0X0ABC,+02748,-0123");
  
  ob.init(buf, sizeof(buf));
  ob << setfill(' ') << left << setw(5) << is << ',' << right << setw(6) << is;
  testVerifyStr(buf, "-123 ,  -123");
}
//------------------------------------------------------------------------------
void ostreamStr() {
  char buf[40];
  obufstream ob(buf, sizeof(buf));
  char c[] = "c";
  const char* cc = "CC";
  signed char* sc = (signed char*)"sc";
  const signed char* csc = (const signed char*)"CSC";
  unsigned char *uc = (unsigned char *)"uc";
  const unsigned char *cuc = (const unsigned char *)"CUC";
  ob << "lit" << c << cc << sc << csc << uc << cuc  << F("F");
  testVerifyStr(buf, "litcCCscCSCucCUCF");
  
  ob.init(buf, sizeof(buf));
  ob << setfill('*') << "s" << setw(8) << "right";
  ob << " " << left << setw(6) << "left" << "e";
  testVerifyStr(buf, "s***right left**e");
}
//------------------------------------------------------------------------------
void setup() {
  testBegin();
  ostreamBool();
  ostreamChar();
  ostreamFloat();
  ostreamNumber();
  ostreamStr();
  testEnd();
}
//------------------------------------------------------------------------------
void loop() {}
