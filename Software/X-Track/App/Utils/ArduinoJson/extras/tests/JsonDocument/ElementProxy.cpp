// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

using namespace ARDUINOJSON_NAMESPACE;

TEST_CASE("ElementProxy::add()") {
  DynamicJsonDocument doc(4096);
  doc.addElement();
  ElementProxy<JsonDocument&> ep = doc[0];

  SECTION("add(int)") {
    ep.add(42);

    REQUIRE(doc.as<std::string>() == "[[42]]");
  }

  SECTION("add(const char*)") {
    ep.add("world");

    REQUIRE(doc.as<std::string>() == "[[\"world\"]]");
  }

  SECTION("set(char[])") {
    char s[] = "world";
    ep.add(s);
    strcpy(s, "!!!!!");

    REQUIRE(doc.as<std::string>() == "[[\"world\"]]");
  }
}

TEST_CASE("ElementProxy::clear()") {
  DynamicJsonDocument doc(4096);
  doc.addElement();
  ElementProxy<JsonDocument&> ep = doc[0];

  SECTION("size goes back to zero") {
    ep.add(42);
    ep.clear();

    REQUIRE(ep.size() == 0);
  }

  SECTION("isNull() return true") {
    ep.add("hello");
    ep.clear();

    REQUIRE(ep.isNull() == true);
  }
}

TEST_CASE("ElementProxy::operator==()") {
  DynamicJsonDocument doc(4096);

  SECTION("1 vs 1") {
    doc.add(1);
    doc.add(1);

    REQUIRE(doc[0] <= doc[1]);
    REQUIRE(doc[0] == doc[1]);
    REQUIRE(doc[0] >= doc[1]);
    REQUIRE_FALSE(doc[0] != doc[1]);
    REQUIRE_FALSE(doc[0] < doc[1]);
    REQUIRE_FALSE(doc[0] > doc[1]);
  }

  SECTION("1 vs 2") {
    doc.add(1);
    doc.add(2);

    REQUIRE(doc[0] != doc[1]);
    REQUIRE(doc[0] < doc[1]);
    REQUIRE(doc[0] <= doc[1]);
    REQUIRE_FALSE(doc[0] == doc[1]);
    REQUIRE_FALSE(doc[0] > doc[1]);
    REQUIRE_FALSE(doc[0] >= doc[1]);
  }

  SECTION("'abc' vs 'bcd'") {
    doc.add("abc");
    doc.add("bcd");

    REQUIRE(doc[0] != doc[1]);
    REQUIRE(doc[0] < doc[1]);
    REQUIRE(doc[0] <= doc[1]);
    REQUIRE_FALSE(doc[0] == doc[1]);
    REQUIRE_FALSE(doc[0] > doc[1]);
    REQUIRE_FALSE(doc[0] >= doc[1]);
  }
}

TEST_CASE("ElementProxy::remove()") {
  DynamicJsonDocument doc(4096);
  doc.addElement();
  ElementProxy<JsonDocument&> ep = doc[0];

  SECTION("remove(int)") {
    ep.add(1);
    ep.add(2);
    ep.add(3);

    ep.remove(1);

    REQUIRE(ep.as<std::string>() == "[1,3]");
  }

  SECTION("remove(const char *)") {
    ep["a"] = 1;
    ep["b"] = 2;

    ep.remove("a");

    REQUIRE(ep.as<std::string>() == "{\"b\":2}");
  }

  SECTION("remove(std::string)") {
    ep["a"] = 1;
    ep["b"] = 2;

    ep.remove(std::string("b"));

    REQUIRE(ep.as<std::string>() == "{\"a\":1}");
  }

#ifdef HAS_VARIABLE_LENGTH_ARRAY
  SECTION("remove(vla)") {
    ep["a"] = 1;
    ep["b"] = 2;

    int i = 4;
    char vla[i];
    strcpy(vla, "b");
    ep.remove(vla);

    REQUIRE(ep.as<std::string>() == "{\"a\":1}");
  }
#endif
}

TEST_CASE("ElementProxy::set()") {
  DynamicJsonDocument doc(4096);
  ElementProxy<JsonDocument&> ep = doc[0];

  SECTION("set(int)") {
    ep.set(42);

    REQUIRE(doc.as<std::string>() == "[42]");
  }

  SECTION("set(const char*)") {
    ep.set("world");

    REQUIRE(doc.as<std::string>() == "[\"world\"]");
  }

  SECTION("set(char[])") {
    char s[] = "world";
    ep.set(s);
    strcpy(s, "!!!!!");

    REQUIRE(doc.as<std::string>() == "[\"world\"]");
  }
}

TEST_CASE("ElementProxy::size()") {
  DynamicJsonDocument doc(4096);
  doc.addElement();
  ElementProxy<JsonDocument&> ep = doc[0];

  SECTION("returns 0") {
    REQUIRE(ep.size() == 0);
  }

  SECTION("as an array, returns 2") {
    ep.add(1);
    ep.add(2);
    REQUIRE(ep.size() == 2);
  }

  SECTION("as an object, returns 2") {
    ep["a"] = 1;
    ep["b"] = 2;
    REQUIRE(ep.size() == 2);
  }
}

TEST_CASE("ElementProxy::operator[]") {
  DynamicJsonDocument doc(4096);
  ElementProxy<JsonDocument&> ep = doc[1];

  SECTION("set member") {
    ep["world"] = 42;

    REQUIRE(doc.as<std::string>() == "[null,{\"world\":42}]");
  }

  SECTION("set element") {
    ep[2] = 42;

    REQUIRE(doc.as<std::string>() == "[null,[null,null,42]]");
  }
}
