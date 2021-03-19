#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
const uint8_t SD_CS_PIN = SS;
SdFat sd;
SdFile file;
char name[260];

// Serial output stream
ArduinoOutStream cout(Serial);

// Serial in buffer.
char cinBuf[10];

// Serial input stream
ArduinoInStream cin(Serial, cinBuf, sizeof(cinBuf));
//------------------------------------------------------------------------------
const char* testName[] = {
  "low.low",
  "low.Mix",
  "low.UP",
  "Mix.low",
  "Mix.Mix",
  "Mix.UP",  
  "UP.low",
  "UP.Mix",
  "UP.UP",
  ".dot",
  ".dot.dot",
  "A b c . txt",
  " Leading space and no extension",
  "Trailing dots and space  . . .",
  "Long extension.extension",
  "Space after dot. txt",
  "Dot.dot.test.txt",
  "Dot.dot.test.seq.txt",
  "LOW.LOW",
  "MIX.MIX",  
  "Invalid character *.test"
};
//------------------------------------------------------------------------------
bool checkName(char first, size_t len) {
  size_t i;
  if (len < 5 || len > sizeof(name)) {
    return false;
  }
  if ( name[0] != first) {
    return false;
  }
  for (i = 1; i < (len - 4); i++) {
    if (name[i] != (char)('0' + (i + 1) %10)) {
      return false;
    }
  }
  const char* p = ".txt";
  while (*p) {
    if (name[i++] != *p++) {
      return false;
    }
  }
  return name[i] == 0;
}
//------------------------------------------------------------------------------
void makeName(char first, size_t len) {
  size_t i;
  if (len > sizeof(name)) {
    len = 255;
  }
  if (len < 5) {
    len = 5;
  }
  name[0] = first;
  for (i = 1; i < (len - 4); i++) {
    name[i] = '0' + (i + 1) %10;
  }
  const char* p = ".txt";
  while (*p) name[i++] = *p++;
  name[i] = 0;
}
//------------------------------------------------------------------------------
// test open, remove, getName, and ls.
void basicTest() {
  size_t i;
  size_t n = sd.vol()->fatType() == 32 ? 255 : 99;
  uint16_t maxIndex = 0;
  
  makeName('Z', 256);
  if (!file.open(name, O_RDWR | O_CREAT)) {
    cout << F("255 limit OK") << endl;
  } else {
    sd.errorHalt(F("255 limit"));
  }
  for (i = 5; i <= n; i++) {
    makeName('A', i);

    if (!file.open(name, O_RDWR | O_CREAT)) {
      sd.errorHalt(F("open A"));
    }
    file.println(name);
    cout << setw(3) << i << setw(5) << file.dirIndex() << F(" open A") << endl;
    
    if (file.fileSize() != (i + 2)) {
      sd.errorHalt(F("file size A"));
    }    
    if (file.dirIndex() >= maxIndex) {
      maxIndex = file.dirIndex();
    } else {
      sd.errorHalt(F("dirIndex"));
    }
    file.close();
    if (!file.open(sd.vwd(), maxIndex, O_READ)) {
      sd.errorHalt(F("open by index"));
    }
    memset(name, 0, sizeof(name));
    if (!file.getName(name, sizeof(name))) {
      sd.errorHalt(F("getName"));
    }
    if (!checkName('A', i)) {
      cout << name << endl;
      sd.errorHalt(F("checkName"));
    }
    file.close();    
  }
  for (i = n; i >= 5; i -= 2) {
    makeName('A', i);
    cout << setw(3) << i << F( " rm A") << endl;
    if (!sd.remove(name)) {
      sd.errorHalt(F("remove A"));
    }
  }
  for (i = n; i >= 5; i -= 2) {
    makeName('B', i);
    if (!file.open(name, O_RDWR | O_CREAT)) {
      sd.errorHalt(F("open B"));
    }
    file.println(name);
    
    cout << setw(3) << i << setw(5) << file.dirIndex() << F(" open B") << endl;
  
    if (file.fileSize() != (i + 2)) {
      sd.errorHalt(F("file size B"));
    }
    if (file.dirIndex() > maxIndex) {
      sd.errorHalt(F("maxIndex"));
    }
    file.close();
  }
  cout << endl << F("----- ls ------") << endl;
  sd.ls();
  for (i = 5; i <= n; i++) {
    char fc = i & 1 ? 'B' : 'A';
    makeName(fc, i);
    cout << setw(3) << i << F(" rm ") << fc << endl;
    if (!sd.remove(name)) {
      sd.errorHalt(F("remove A/B"));
    }
  }
  if (file.openNext(sd.vwd())) {
    sd.errorHalt(F("remove all"));
  }
  cout << endl << F("basicTest done") << endl;
}
//------------------------------------------------------------------------------
void nameTest() {
  cout << endl;
  uint8_t n = sizeof(testName)/sizeof(char*);
  for (uint8_t i = 0; i < n; i++) {
    cout << F("Name: \"") << testName[i] << '"' << endl;
    if(!file.open(testName[i], O_CREAT | O_RDWR)) {
      cout <<F("Open failed") << endl;
    } else {
      file.println(testName[i]);
      if (!file.getName(name, sizeof(name))) {
        sd.errorHalt(F("getFilemame"));
      }
      cout << F("LFN:  \"") << name << '"' << endl;      
      cout << F("SFN:  \"");
      file.printSFN(&Serial);
      cout << '"' << endl;
      cout <<F("Index: ") << setw(2) << file.dirIndex() << endl;  
      file.close();
    }
    cout << endl;
  }
  cout << F("----- ls ------") << endl;
  sd.ls();
  cout << endl << F("nameTest done") << endl;

}
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  while(!Serial);  // Wait for USB Serial.
  
  cout << endl << F("FreeRam: ") << FreeRam() << endl;
  cout << F("Type any character to start.") << endl;
  cin.readline();
  if (!sd.begin(SD_CS_PIN)) {
    sd.initErrorHalt();
  }
  if (file.openNext(sd.vwd())) {
    file.close();
    cout << F("Type 'W' to wipe the card: ");
    cin.readline();   
    char c = cin.get();
    cout << c << endl;
    if (c != 'W') {
      sd.errorHalt(F("Invalid"));
    }
    if (!sd.wipe(&Serial) || !sd.begin(SD_CS_PIN)) {
      sd.errorHalt(F("wipe failed"));
    }
  }
  basicTest();
  nameTest();
}
//------------------------------------------------------------------------------
void loop() {}