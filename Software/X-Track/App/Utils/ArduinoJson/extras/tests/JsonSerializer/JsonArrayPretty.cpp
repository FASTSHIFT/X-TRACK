// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

static void checkArray(JsonArray array, std::string expected) {
  std::string actual;
  size_t actualLen = serializeJsonPretty(array, actual);
  size_t measuredLen = measureJsonPretty(array);
  CHECK(actualLen == expected.size());
  CHECK(measuredLen == expected.size());
  REQUIRE(expected == actual);
}

TEST_CASE("serializeJsonPretty(JsonArray)") {
  DynamicJsonDocument doc(4096);
  JsonArray array = doc.to<JsonArray>();

  SECTION("Empty") {
    checkArray(array, "[]");
  }

  SECTION("OneElement") {
    array.add(1);

    checkArray(array,
               "[\r\n"
               "  1\r\n"
               "]");
  }

  SECTION("TwoElements") {
    array.add(1);
    array.add(2);

    checkArray(array,
               "[\r\n"
               "  1,\r\n"
               "  2\r\n"
               "]");
  }

  SECTION("EmptyNestedArrays") {
    array.createNestedArray();
    array.createNestedArray();

    checkArray(array,
               "[\r\n"
               "  [],\r\n"
               "  []\r\n"
               "]");
  }

  SECTION("NestedArrays") {
    JsonArray nested1 = array.createNestedArray();
    nested1.add(1);
    nested1.add(2);

    JsonObject nested2 = array.createNestedObject();
    nested2["key"] = 3;

    checkArray(array,
               "[\r\n"
               "  [\r\n"
               "    1,\r\n"
               "    2\r\n"
               "  ],\r\n"
               "  {\r\n"
               "    \"key\": 3\r\n"
               "  }\r\n"
               "]");
  }
}
