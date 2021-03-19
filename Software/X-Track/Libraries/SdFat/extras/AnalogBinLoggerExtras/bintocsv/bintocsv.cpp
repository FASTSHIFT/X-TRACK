#include <stdio.h>
#include "AnalogBinLogger.h"
FILE *source;
FILE *destination;
int count = 0;

int main(int argc, char** argv) {
  metadata_t meta;
  adcdata_t adc;
  // Make sure no padding/size problems.
  if (sizeof(meta) != 512 || sizeof(adc) != 512) {
    printf("block size error\n");
    return 0;
  }
  if (argc != 3) {
    printf("missing arguments:\n");
    printf("%s binFile csvFile\n", argv[0]);
    return 0;
  }
  source = fopen(argv[1], "rb");
  if (!source) {
    printf("open failed for %s\n", argv[1]);
    return 0;
  }
  if (fread(&meta, sizeof(meta), 1, source) != 1) {
    printf("read meta data failed\n");
    return 0;
  }
  if ( meta.pinCount == 0 
    || meta.pinCount > (sizeof(meta.pinNumber)/sizeof(meta.pinNumber[0]))
    || meta.adcFrequency < 50000 || meta.adcFrequency > 4000000) {
    printf("Invalid meta data\n");
    return 0;
  }
  destination = fopen(argv[2], "w");
  if (!destination) {
    printf("open failed for %s\n", argv[2]);
    return 0;
  }
  int pinCount = meta.pinCount;
  printf("pinCount: %d\n", pinCount);
  printf("Sample pins:");
  for (unsigned i = 0; i < meta.pinCount; i++) {
    printf(" %d", meta.pinNumber[i]);
  }
  printf("\n");
  printf("ADC clock rate: %g kHz\n", 0.001*meta.adcFrequency);
  float sampleInterval = (float)meta.sampleInterval/(float)meta.cpuFrequency;
  printf("Sample rate: %g per sec\n", 1.0/sampleInterval);
  printf("Sample interval: %.4f usec\n", 1.0e6*sampleInterval);

  fprintf(destination, "Interval,%.4f,usec\n", 1.0e6*sampleInterval);
  // Write header with pin numbers
  for (int i = 0; i < ((int)meta.pinCount - 1); i++) {
    fprintf(destination, "pin%d,", meta.pinNumber[i]);
  }
  fprintf(destination, "pin%d\n", meta.pinNumber[meta.pinCount - 1]);
  unsigned maxCount = meta.recordEightBits ? DATA_DIM8 : DATA_DIM16;
  while (!feof(source)) {
    if (fread(&adc, sizeof(adc), 1, source) != 1) break;
    if (adc.count > maxCount) {
      printf("****Invalid data block****\n");
      return 0;
    }
    if (adc.overrun) {
      fprintf(destination, "Overruns,%d\n", adc.overrun);
    }
    for (int i = 0; i < adc.count; i++) {
      unsigned value = meta.recordEightBits ? adc.data.u8[i] : adc.data.u16[i];
      if ((i + 1)%pinCount) {
        fprintf(destination, "%d,", value);
      } else {
        fprintf(destination, "%d\n", value);
      }
    }
    count += adc.count;
  }
  printf("%d ADC values read\n", count);
  fclose(source);
  fclose(destination);
  return 0;
}