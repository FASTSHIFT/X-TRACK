// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

template <typename T, typename U>
static void check(const char* input, U expected) {
  DynamicJsonDocument doc(4096);

  DeserializationError error = deserializeMsgPack(doc, input);

  REQUIRE(error == DeserializationError::Ok);
  REQUIRE(doc.is<T>());
  REQUIRE(doc.as<T>() == expected);
}

#if ARDUINOJSON_USE_LONG_LONG == 0
static void checkNotSupported(const char* input) {
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeMsgPack(doc, input);
  REQUIRE(error == DeserializationError::Ok);
  REQUIRE(doc.isNull());
}
#endif

static void checkIsNull(const char* input) {
  DynamicJsonDocument doc(4096);

  DeserializationError error = deserializeMsgPack(doc, input);

  REQUIRE(error == DeserializationError::Ok);
  REQUIRE(doc.as<JsonVariant>().isNull());
}

TEST_CASE("deserialize MsgPack value") {
  SECTION("nil") {
    checkIsNull("\xc0");
  }

  SECTION("bool") {
    check<bool>("\xc2", false);
    check<bool>("\xc3", true);
  }

  SECTION("positive fixint") {
    check<int>("\x00", 0);
    check<int>("\x7F", 127);
  }

  SECTION("negative fixint") {
    check<int>("\xe0", -32);
    check<int>("\xff", -1);
  }

  SECTION("uint 8") {
    check<int>("\xcc\x00", 0);
    check<int>("\xcc\xff", 255);
  }

  SECTION("uint 16") {
    check<int>("\xcd\x00\x00", 0);
    check<int>("\xcd\xFF\xFF", 65535);
    check<int>("\xcd\x30\x39", 12345);
  }

  SECTION("uint 32") {
    check<uint32_t>("\xCE\x00\x00\x00\x00", 0x00000000U);
    check<uint32_t>("\xCE\xFF\xFF\xFF\xFF", 0xFFFFFFFFU);
    check<uint32_t>("\xCE\x12\x34\x56\x78", 0x12345678U);
  }

  SECTION("uint 64") {
#if ARDUINOJSON_USE_LONG_LONG
    check<uint64_t>("\xCF\x00\x00\x00\x00\x00\x00\x00\x00", 0U);
    check<uint64_t>("\xCF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
                    0xFFFFFFFFFFFFFFFFU);
    check<uint64_t>("\xCF\x12\x34\x56\x78\x9A\xBC\xDE\xF0",
                    0x123456789ABCDEF0U);
#else
    checkNotSupported("\xCF\x00\x00\x00\x00\x00\x00\x00\x00");
    checkNotSupported("\xCF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
    checkNotSupported("\xCF\x12\x34\x56\x78\x9A\xBC\xDE\xF0");
#endif
  }

  SECTION("int 8") {
    check<int>("\xd0\x00", 0);
    check<int>("\xd0\xff", -1);
  }

  SECTION("int 16") {
    check<int>("\xD1\x00\x00", 0);
    check<int>("\xD1\xFF\xFF", -1);
    check<int>("\xD1\xCF\xC7", -12345);
  }

  SECTION("int 32") {
    check<int>("\xD2\x00\x00\x00\x00", 0);
    check<int>("\xD2\xFF\xFF\xFF\xFF", -1);
    check<int>("\xD2\xB6\x69\xFD\x2E", -1234567890);
  }

  SECTION("int 64") {
#if ARDUINOJSON_USE_LONG_LONG
    check<int64_t>("\xD3\x00\x00\x00\x00\x00\x00\x00\x00", int64_t(0U));
    check<int64_t>("\xD3\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
                   int64_t(0xFFFFFFFFFFFFFFFFU));
    check<int64_t>("\xD3\x12\x34\x56\x78\x9A\xBC\xDE\xF0",
                   int64_t(0x123456789ABCDEF0));
#else
    checkNotSupported("\xD3\x00\x00\x00\x00\x00\x00\x00\x00");
    checkNotSupported("\xD3\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
    checkNotSupported("\xD3\x12\x34\x56\x78\x9A\xBC\xDE\xF0");
#endif
  }

  SECTION("float 32") {
    check<double>("\xCA\x00\x00\x00\x00", 0.0f);
    check<double>("\xCA\x40\x48\xF5\xC3", 3.14f);
  }

  SECTION("float 64") {
    check<double>("\xCB\x00\x00\x00\x00\x00\x00\x00\x00", 0.0);
    check<double>("\xCB\x40\x09\x21\xCA\xC0\x83\x12\x6F", 3.1415);
  }

  SECTION("fixstr") {
    check<const char*>("\xA0", std::string(""));
    check<const char*>("\xABhello world", std::string("hello world"));
    check<const char*>("\xBFhello world hello world hello !",
                       std::string("hello world hello world hello !"));
  }

  SECTION("str 8") {
    check<const char*>("\xd9\x05hello", std::string("hello"));
  }

  SECTION("str 16") {
    check<const char*>("\xda\x00\x05hello", std::string("hello"));
  }

  SECTION("str 32") {
    check<const char*>("\xdb\x00\x00\x00\x05hello", std::string("hello"));
  }
}
