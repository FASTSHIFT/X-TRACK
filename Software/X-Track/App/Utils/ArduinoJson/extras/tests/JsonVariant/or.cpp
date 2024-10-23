// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

TEST_CASE("JsonVariant::operator|()") {
  DynamicJsonDocument doc(4096);
  JsonVariant variant = doc["value"].to<JsonVariant>();

  SECTION("undefined") {
    SECTION("undefined | const char*") {
      std::string result = variant | "default";
      REQUIRE(result == "default");
    }

    SECTION("undefined | int") {
      int result = variant | 42;
      REQUIRE(result == 42);
    }

    SECTION("undefined | bool") {
      bool result = variant | true;
      REQUIRE(result == true);
    }

    SECTION("undefined | ElementProxy") {
      doc["array"][0] = 42;

      JsonVariantConst result = variant | doc["array"][0];
      REQUIRE(result == 42);
    }

    SECTION("undefined | MemberProxy") {
      doc["other"] = 42;

      JsonVariantConst result = variant | doc["other"];
      REQUIRE(result == 42);
    }

    SECTION("ElementProxy | ElementProxy") {
      doc["array"][0] = 42;

      JsonVariantConst result = doc["array"][1] | doc["array"][0];
      REQUIRE(result == 42);
    }
  }

  SECTION("null") {
    variant.set(static_cast<const char*>(0));

    SECTION("null | const char*") {
      std::string result = variant | "default";
      REQUIRE(result == "default");
    }

    SECTION("null | int") {
      int result = variant | 42;
      REQUIRE(result == 42);
    }

    SECTION("null | bool") {
      bool result = variant | true;
      REQUIRE(result == true);
    }

    SECTION("null | ElementProxy") {
      doc["array"][0] = 42;

      JsonVariantConst result = variant | doc["array"][0];
      REQUIRE(result == 42);
    }

    SECTION("null | MemberProxy") {
      doc["other"] = 42;

      JsonVariantConst result = variant | doc["other"];
      REQUIRE(result == 42);
    }
  }

  SECTION("int | const char*") {
    variant.set(42);
    std::string result = variant | "default";
    REQUIRE(result == "default");
  }

  SECTION("int | uint8_t (out of range)") {
    variant.set(666);
    uint8_t result = variant | static_cast<uint8_t>(42);
    REQUIRE(result == 42);
  }

  SECTION("int | ElementProxy") {
    variant.set(42);
    doc["array"][0] = 666;
    JsonVariantConst result = variant | doc["array"][0];
    REQUIRE(result == 42);
  }

  SECTION("int | MemberProxy") {
    variant.set(42);
    doc["other"] = 666;
    JsonVariantConst result = variant | doc["other"];
    REQUIRE(result == 42);
  }

  SECTION("int | int") {
    variant.set(0);
    int result = variant | 666;
    REQUIRE(result == 0);
  }

  SECTION("double | int") {
    // NOTE: changed the behavior to fix #981
    variant.set(666.0);
    int result = variant | 42;
    REQUIRE(result == 42);
  }

  SECTION("bool | bool") {
    variant.set(false);
    bool result = variant | true;
    REQUIRE(result == false);
  }

  SECTION("int | bool") {
    variant.set(0);
    bool result = variant | true;
    REQUIRE(result == true);
  }

  SECTION("const char* | const char*") {
    variant.set("not default");
    std::string result = variant | "default";
    REQUIRE(result == "not default");
  }

  SECTION("const char* | char*") {
    char dflt[] = "default";
    variant.set("not default");
    std::string result = variant | dflt;
    REQUIRE(result == "not default");
  }

  SECTION("int | char*") {
    char dflt[] = "default";
    variant.set(42);
    std::string result = variant | dflt;
    REQUIRE(result == "default");
  }

  SECTION("const char* | int") {
    variant.set("not default");
    int result = variant | 42;
    REQUIRE(result == 42);
  }
}
