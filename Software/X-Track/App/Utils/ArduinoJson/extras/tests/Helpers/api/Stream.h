// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#pragma once

// Reproduces Arduino's Stream class
class Stream  // : public Print
{
 public:
  virtual ~Stream() {}
  virtual int read() = 0;
  virtual size_t readBytes(char *buffer, size_t length) = 0;
};
