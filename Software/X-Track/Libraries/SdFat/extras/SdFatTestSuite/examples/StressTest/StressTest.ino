// This stress test will create and write files until the SD is full.
#include <SPI.h> 
#include <SdFat.h>

// SD chip select pin.
const uint8_t SD_CS_PIN = SS;

// Set write buffer size.
#ifdef __arm__
#ifndef CORE_TEENSY
// Due
const size_t BUF_SIZE = 32768;
#else  // CORE_TEENSY
// Teensy 3.0
const size_t BUF_SIZE = 8192;
#endif  // CORE_TEENSY
#elif defined(RAMEND) && RAMEND > 5000
// AVR with more than 4 KB RAM
const size_t BUF_SIZE = 4096;
#else  // __arm__
// other
const size_t BUF_SIZE = 512;
#endif  // __arm__

const size_t FILE_SIZE_KB = 10240;
const uint16_t BUFS_PER_FILE = (1024L*FILE_SIZE_KB/BUF_SIZE);

SdFat sd;

SdFile file;

uint8_t buf[BUF_SIZE];
char name[13];
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.print("BUF_SIZE ");
  Serial.println(BUF_SIZE);
  Serial.println("Type any character to start");
  while (Serial.read() < 0) {}
  
  if (!sd.begin(SD_CS_PIN))sd.errorHalt("sd.begin");
  
  // Fill buf with known value.
  for (size_t i = 0; i < BUF_SIZE; i++) buf[i] = i;
  
  // Wait to begin.
  do {delay(10);} while (Serial.read() >= 0);
  Serial.println("Type any character to stop after next file");
}
//------------------------------------------------------------------------------
void loop() {
  // Free KB on SD.
  uint32_t freeKB = sd.vol()->freeClusterCount()*sd.vol()->blocksPerCluster()/2;

  Serial.print("Free KB: ");
  Serial.println(freeKB);
  if (freeKB < 2*FILE_SIZE_KB) {
    Serial.println(" Done!");
    while(1);
  }
  sprintf(name, "%lu.DAT", freeKB);
  if (!file.open(name, O_WRITE | O_CREAT | O_TRUNC)) {
    sd.errorHalt("Open error!");
  }
  for (uint16_t i = 0; i < BUFS_PER_FILE; i++) {
    if (file.write(buf, BUF_SIZE) != BUF_SIZE) {
      sd.errorHalt("Write error!");
    }
  }
  file.close();
  if (Serial.available()) {
    Serial.println("Stopped!");
    while(1);
  }
}