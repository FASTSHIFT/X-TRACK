#include <SPI.h>
#include <SdFat.h>
#include <SdFatTestSuite.h>
SdFat sd;
const char *testName = "SDFAT.TST";
//------------------------------------------------------------------------------
void fstreamOpen() {
  ios::openmode nocreate[] = {ios::in, ios::in | ios::out};
  ios::openmode create[] =
    {ios::out, ios::out | ios::app, ios::app, ios::out | ios::trunc,
     ios::in | ios::out | ios::trunc, ios::in | ios::out | ios::app,
     ios::in | ios::app};
  ios::openmode illegal[] =
    {0, ios::trunc, ios::app | ios::trunc, ios::in | ios::app | ios::trunc,
     ios::in | ios::trunc, ios::out | ios::app | ios::trunc,
     ios::in | ios::out | ios::app | ios::trunc};

  sd.remove(testName);
  fstream file(testName);
  testVerifyMsg(!file.is_open()&& !sd.exists(testName), "fstream constructor");

  for (uint8_t i = 0 ; i < sizeof(nocreate)/sizeof(nocreate[1]); i++) {
    file.close();
    sd.remove(testName);
    file.open(testName, nocreate[i]);
    testVerifyMsg(!sd.exists(testName) && !file.is_open(), "fstream nocreate !exists");
  }
  for (uint8_t i = 0 ; i < sizeof(create)/sizeof(create[1]); i++) {
    file.close();
    sd.remove(testName);
    file.open(testName, create[i]);
    testVerifyMsg(sd.exists(testName) && file.is_open(), "fstream create openmode");
  }
  for (uint8_t i = 0 ; i < sizeof(illegal)/sizeof(illegal[1]); i++) {
    file.close();
    file.open(testName, illegal[i]);
    testVerifyMsg(sd.exists(testName) && !file.is_open(), "fstream illegal openmode");
  }
  for (uint8_t i = 0 ; i < sizeof(nocreate)/sizeof(nocreate[1]); i++) {
    file.close();
    file.open(testName, nocreate[i]);
    testVerifyMsg(sd.exists(testName) && file.is_open(), "fstream nocreate exists");
  }
}
//------------------------------------------------------------------------------
void testPosition() {
  sd.remove(testName);
  ofstream ofs(testName);
  testVerifyBool(ofs.good() && ofs.tellp() == 0);
  ofs.seekp(0, ios::end);
  testVerifyBool(ofs.good() && ofs.tellp() == 0);
  ofs << "abcde";
  testVerifyBool(ofs.good() && ofs.tellp() == 5);
  ofs.seekp(4);
  testVerifyBool(ofs.good() && ofs.tellp() == 4);
  ofs.seekp(-1, ios::cur);
  testVerifyBool(ofs.good() && ofs.tellp() == 3);
  ofs.close();
  ifstream ifs(testName, ios::ate);
  testVerifyBool(ifs.good() && ifs.tellg() == 5);
  ifs.seekg(0);
  testVerifyBool(ifs.get() == 'a' && ifs.get() == 'b');
  testVerifyBool(ifs.tellg() == 2 && ifs.good());
  ifs.seekg(3, ios::cur);
  testVerifyBool(ifs.tellg() == 5 && ifs.good());
  ifs.seekg(4, ios::beg);
  testVerifyBool(ifs.good() && ifs.tellg() == 4);
  ifs.close();
  ofs.open(testName, ios::app);
  testVerifyBool(ofs.good() && ofs.tellp() == 0);
  ofs << 'f';
  testVerifyBool(ofs.good() && ofs.tellp() == 6);
  ofs.close();
  ofs.open(testName, ios::trunc);
  ofs.seekp(0, ios::end);
  testVerifyBool(ofs.good() && ofs.tellp() == 0);
  ofs << "ABCDEF";
  ofs.close();
  fstream fs(testName);
  testVerifyBool(fs.good() && fs.tellp() == 0 && fs.tellg() == 0);
  fs.seekg(2);
  testVerifyBool(fs.good() && fs.get() == 'C');
}
//------------------------------------------------------------------------------
void setup() {

  testBegin();
  if (!sd.begin()) sd.initErrorHalt();
  fstreamOpen();
  testPosition();
  testEnd();
}
//------------------------------------------------------------------------------
void loop() {}
