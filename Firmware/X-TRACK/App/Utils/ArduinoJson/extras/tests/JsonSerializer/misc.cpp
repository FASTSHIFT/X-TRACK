#include <ArduinoJson.h>
#include <catch.hpp>
#include <limits>

TEST_CASE("serializeJson(MemberProxy)") {
  DynamicJsonDocument doc(4096);
  deserializeJson(doc, "{\"hello\":42}");
  JsonObject obj = doc.as<JsonObject>();
  std::string result;

  serializeJson(obj["hello"], result);

  REQUIRE(result == "42");
}

TEST_CASE("serializeJson(ElementProxy)") {
  DynamicJsonDocument doc(4096);
  deserializeJson(doc, "[42]");
  JsonArray arr = doc.as<JsonArray>();
  std::string result;

  serializeJson(arr[0], result);

  REQUIRE(result == "42");
}

TEST_CASE("serializeJson(JsonVariantSubscript)") {
  DynamicJsonDocument doc(4096);
  deserializeJson(doc, "[42]");
  JsonVariant var = doc.as<JsonVariant>();
  std::string result;

  serializeJson(var[0], result);

  REQUIRE(result == "42");
}
