// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#pragma once

#include <stdint.h>
#include <string.h>  // for strlen

#include <ArduinoJson/Json/EscapeSequence.hpp>
#include <ArduinoJson/Numbers/FloatParts.hpp>
#include <ArduinoJson/Numbers/Integer.hpp>
#include <ArduinoJson/Polyfills/assert.hpp>
#include <ArduinoJson/Polyfills/attributes.hpp>
#include <ArduinoJson/Polyfills/type_traits.hpp>
#include <ArduinoJson/Serialization/CountingDecorator.hpp>

namespace ARDUINOJSON_NAMESPACE {

template <typename TWriter>
class TextFormatter {
 public:
  explicit TextFormatter(TWriter writer) : _writer(writer) {}

  // Returns the number of bytes sent to the TWriter implementation.
  size_t bytesWritten() const {
    return _writer.count();
  }

  void writeBoolean(bool value) {
    if (value)
      writeRaw("true");
    else
      writeRaw("false");
  }

  void writeString(const char *value) {
    ARDUINOJSON_ASSERT(value != NULL);
    writeRaw('\"');
    while (*value) writeChar(*value++);
    writeRaw('\"');
  }

  void writeChar(char c) {
    char specialChar = EscapeSequence::escapeChar(c);
    if (specialChar) {
      writeRaw('\\');
      writeRaw(specialChar);
    } else {
      writeRaw(c);
    }
  }

  template <typename T>
  void writeFloat(T value) {
    if (isnan(value))
      return writeRaw(ARDUINOJSON_ENABLE_NAN ? "NaN" : "null");

#if ARDUINOJSON_ENABLE_INFINITY
    if (value < 0.0) {
      writeRaw('-');
      value = -value;
    }

    if (isinf(value))
      return writeRaw("Infinity");
#else
    if (isinf(value))
      return writeRaw("null");

    if (value < 0.0) {
      writeRaw('-');
      value = -value;
    }
#endif

    FloatParts<T> parts(value);

    writeInteger(parts.integral);
    if (parts.decimalPlaces)
      writeDecimals(parts.decimal, parts.decimalPlaces);

    if (parts.exponent) {
      writeRaw('e');
      writeInteger(parts.exponent);
    }
  }

  template <typename T>
  typename enable_if<is_signed<T>::value>::type writeInteger(T value) {
    typedef typename make_unsigned<T>::type unsigned_type;
    unsigned_type unsigned_value;
    if (value < 0) {
      writeRaw('-');
      unsigned_value = unsigned_type(unsigned_type(~value) + 1);
    } else {
      unsigned_value = unsigned_type(value);
    }
    writeInteger(unsigned_value);
  }

  template <typename T>
  typename enable_if<is_unsigned<T>::value>::type writeInteger(T value) {
    char buffer[22];
    char *end = buffer + sizeof(buffer);
    char *begin = end;

    // write the string in reverse order
    do {
      *--begin = char(value % 10 + '0');
      value = T(value / 10);
    } while (value);

    // and dump it in the right order
    writeRaw(begin, end);
  }

  void writeDecimals(uint32_t value, int8_t width) {
    // buffer should be big enough for all digits and the dot
    char buffer[16];
    char *end = buffer + sizeof(buffer);
    char *begin = end;

    // write the string in reverse order
    while (width--) {
      *--begin = char(value % 10 + '0');
      value /= 10;
    }
    *--begin = '.';

    // and dump it in the right order
    writeRaw(begin, end);
  }

  void writeRaw(const char *s) {
    _writer.write(reinterpret_cast<const uint8_t *>(s), strlen(s));
  }

  void writeRaw(const char *s, size_t n) {
    _writer.write(reinterpret_cast<const uint8_t *>(s), n);
  }

  void writeRaw(const char *begin, const char *end) {
    _writer.write(reinterpret_cast<const uint8_t *>(begin),
                  static_cast<size_t>(end - begin));
  }

  template <size_t N>
  void writeRaw(const char (&s)[N]) {
    _writer.write(reinterpret_cast<const uint8_t *>(s), N - 1);
  }
  void writeRaw(char c) {
    _writer.write(static_cast<uint8_t>(c));
  }

 protected:
  CountingDecorator<TWriter> _writer;

 private:
  TextFormatter &operator=(const TextFormatter &);  // cannot be assigned
};
}  // namespace ARDUINOJSON_NAMESPACE
