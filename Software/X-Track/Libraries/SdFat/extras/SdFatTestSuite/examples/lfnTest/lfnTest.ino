#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
const uint8_t SD_CS_PIN = SS;
SdFat sd;
SdFile file;
char name[260];

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
    Serial.println(F("255 limit OK"));
  } else {
    sd.errorHalt(F("255 limit"));
  }
  for (i = 5; i <= n; i++) {
    makeName('A', i);

    if (!file.open(name, O_RDWR | O_CREAT)) {
      sd.errorHalt(F("open A"));
    }
    file.println(name);
    Serial.print(i);
    Serial.write(' ');   
    Serial.print(file.dirIndex());
    Serial.write(' ');
    Serial.print(file.fileSize());
    Serial.println(F(" open A"));
    if (file.fileSize() != (i + 2)) {
      sd.errorHalt(F("file size A"));
    }    
    if (file.dirIndex() >= maxIndex) {
      maxIndex = file.dirIndex();
    } else {
      Serial.print(maxIndex); Serial.print(',');Serial.println(file.dirIndex());
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
      Serial.println(name);
      sd.errorHalt(F("checkName"));
    }
    file.close();    
  }
  for (i = n; i >= 5; i -= 2) {
    makeName('A', i);
    Serial.print(i);
    Serial.println(F( " rm A"));
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
    Serial.print(i);
    Serial.write(' ');   
    Serial.print(file.dirIndex());
    Serial.write(' ');
    Serial.print(file.fileSize());    
    Serial.println(F(" open B"));
    if (file.fileSize() != (i + 2)) {
      sd.errorHalt(F("file size B"));
    }
    if (file.dirIndex() > maxIndex) {
      sd.errorHalt(F("maxIndex"));
    }
    file.close();
  }
  Serial.println(F("----- ls ------"));
  sd.ls();
  for (i = 5; i <= n; i++) {
    char fc = i & 1 ? 'B' : 'A';
    makeName(fc, i);
    Serial.print(i);
    Serial.print(F(" rm "));
    Serial.println(fc);
    if (!sd.remove(name)) {
      sd.errorHalt(F("remove A/B"));
    }
  }
  if (file.openNext(sd.vwd())) {
    sd.errorHalt(F("remove all"));
  }
  Serial.println();
  Serial.println(F("basicTest done"));
}
//------------------------------------------------------------------------------
void nameTest() {
  Serial.println();
  uint8_t n = sizeof(testName)/sizeof(char*);
  for (uint8_t i = 0; i < n; i++) {
    Serial.print(F("Name: "));
    Serial.write('"');
    Serial.print(testName[i]);
    Serial.println('"');
    if(!file.open(testName[i], O_CREAT | O_RDWR)) {
      Serial.println(F("Open failed"));
    } else {
      file.println(testName[i]);
      if (!file.getName(name, sizeof(name))) {
        sd.errorHalt(F("getFilemame"));
      }
      file.println(name);
      Serial.print(F("LFN:  "));
      Serial.write('"');
      Serial.print(name);
      Serial.println('"');
      Serial.print(F("SFN:  "));
      Serial.write('"');
      file.printSFN(&Serial);
      Serial.println('"');
      Serial.print(F("Index: "));
      if (file.dirIndex() < 10) {
        Serial.write(' ');
      }
      Serial.println(file.dirIndex());      
      file.close();
    }
    Serial.println();
  }
  Serial.println(F("----- ls ------"));
  sd.ls();
  Serial.println();
  Serial.println(F("nameTest done"));
}
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.print(F("\r\nFreeRam: "));
  Serial.println(FreeRam());
  Serial.println(F("Type any character to start."));
  while (Serial.read() < 0) {}
  if (!sd.begin(SD_CS_PIN)) sd.initErrorHalt();
  if (file.openNext(sd.vwd())) {
    file.close();
    delay(100);
    while (Serial.read() >= 0) {}
    Serial.print(F("Type 'W' to wipe the card: "));
    int c;
    while ((c = Serial.read()) < 0) {}
    if (c != 'W') {
      sd.errorHalt(F("Invalid"));
    }
    Serial.println((char)c);
    if (!sd.wipe(&Serial) || !sd.begin(SD_CS_PIN)) {
      sd.errorHalt(F("wipe failed"));
    }
  }
  basicTest();
  nameTest();
}
//------------------------------------------------------------------------------
void loop() {}