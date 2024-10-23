// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#include <ArduinoJson.h>
#include <catch.hpp>

void check(std::string originalJson) {
  DynamicJsonDocument doc(16384);

  std::string prettyJson;
  deserializeJson(doc, originalJson);
  serializeJsonPretty(doc, prettyJson);

  std::string finalJson;
  deserializeJson(doc, originalJson);
  serializeJson(doc, finalJson);

  REQUIRE(originalJson == finalJson);
}

TEST_CASE("Round Trip: parse -> prettyPrint -> parse -> print") {
  SECTION("OpenWeatherMap") {
    check(
        "{\"coord\":{\"lon\":145.77,\"lat\":-16.92},\"sys\":{\"type\":1,\"id\":"
        "8166,\"message\":0.1222,\"country\":\"AU\",\"sunrise\":1414784325,"
        "\"sunset\":1414830137},\"weather\":[{\"id\":801,\"main\":\"Clouds\","
        "\"description\":\"few clouds\",\"icon\":\"02n\"}],\"base\":\"cmc "
        "stations\",\"main\":{\"temp\":296.15,\"pressure\":1014,\"humidity\":"
        "83,\"temp_min\":296.15,\"temp_max\":296.15},\"wind\":{\"speed\":2.22,"
        "\"deg\":114.501},\"clouds\":{\"all\":20},\"dt\":1414846800,\"id\":"
        "2172797,\"name\":\"Cairns\",\"cod\":200}");
  }

  SECTION("YahooQueryLanguage") {
    check(
        "{\"query\":{\"count\":40,\"created\":\"2014-11-01T14:16:49Z\","
        "\"lang\":\"fr-FR\",\"results\":{\"item\":[{\"title\":\"Burkina army "
        "backs Zida as interim leader\"},{\"title\":\"British jets intercept "
        "Russian bombers\"},{\"title\":\"Doubts chip away at nation's most "
        "trusted agencies\"},{\"title\":\"Cruise ship stuck off Norway, no "
        "damage\"},{\"title\":\"U.S. military launches 10 air strikes in "
        "Syria, Iraq\"},{\"title\":\"Blackout hits Bangladesh as line from "
        "India fails\"},{\"title\":\"Burkina Faso president in Ivory Coast "
        "after ouster\"},{\"title\":\"Kurds in Turkey rally to back city "
        "besieged by IS\"},{\"title\":\"A majority of Scots would vote for "
        "independence now:poll\"},{\"title\":\"Tunisia elections possible "
        "model for region\"},{\"title\":\"Islamic State kills 85 more members "
        "of Iraqi tribe\"},{\"title\":\"Iraqi officials:IS extremists line "
        "up, kill 50\"},{\"title\":\"Burkina Faso army backs presidential "
        "guard official to lead transition\"},{\"title\":\"Kurdish peshmerga "
        "arrive with weapons in Syria's Kobani\"},{\"title\":\"Driver sought "
        "in crash that killed 3 on Halloween\"},{\"title\":\"Ex-Marine arrives "
        "in US after release from Mexico jail\"},{\"title\":\"UN panel "
        "scrambling to finish climate report\"},{\"title\":\"Investigators, "
        "Branson go to spacecraft crash site\"},{\"title\":\"Soldiers vie for "
        "power after Burkina Faso president quits\"},{\"title\":\"For a man "
        "without a party, turnout is big test\"},{\"title\":\"'We just had a "
        "hunch':US marshals nab Eric Frein\"},{\"title\":\"Boko Haram leader "
        "threatens to kill German hostage\"},{\"title\":\"Nurse free to move "
        "about as restrictions eased\"},{\"title\":\"Former Burkina president "
        "Compaore arrives in Ivory Coast:sources\"},{\"title\":\"Libyan port "
        "rebel leader refuses to hand over oil ports to rival "
        "group\"},{\"title\":\"Iraqi peshmerga fighters prepare for Syria "
        "battle\"},{\"title\":\"1 Dem Senate candidate welcoming Obama's "
        "help\"},{\"title\":\"Bikers cancel party after police recover "
        "bar\"},{\"title\":\"New question in Texas:Can Davis survive "
        "defeat?\"},{\"title\":\"Ukraine rebels to hold election, despite "
        "criticism\"},{\"title\":\"Iraqi officials say Islamic State group "
        "lines up, kills 50 tribesmen, women in Anbar "
        "province\"},{\"title\":\"James rebounds, leads Cavaliers past "
        "Bulls\"},{\"title\":\"UK warns travelers they could be terror "
        "targets\"},{\"title\":\"Hello Kitty celebrates 40th "
        "birthday\"},{\"title\":\"A look at people killed during space "
        "missions\"},{\"title\":\"Nigeria's purported Boko Haram leader says "
        "has 'married off' girls:AFP\"},{\"title\":\"Mexico orders immediate "
        "release of Marine veteran\"},{\"title\":\"As election closes in, "
        "Obama on center stage\"},{\"title\":\"Body of Zambian president "
        "arrives home\"},{\"title\":\"South Africa arrests 2 Vietnamese for "
        "poaching\"}]}}}");
  }
}
