#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <string>
#include <rapidjson/document.h>

#include <preset/preset.h>
#include <preset/preset.hpp>

#include "Editor/Value.h"

TEST_CASE ("JSON array") {
    SECTION ("Basic array parsing") {

        std::string json = R"_({"a":[1, 2, 4, 8]})_";
        REQUIRE(ossia::presets::read_json(json).size() == 4);
    }

    SECTION ("Contents of array") {
        std::string json2 = R"_({"a":[[3, -6], [9, 2, 11]]})_";
        ossia::presets::Preset p = ossia::presets::read_json(json2);
        REQUIRE(p.size() == 5);
        REQUIRE(p.find("/a.0.0") != p.end());
        REQUIRE(p.find("/a.0.1") != p.end());
        REQUIRE(p.find("/a.1.0") != p.end());
        REQUIRE(p.find("/a.1.1") != p.end());
        REQUIRE(p.find("/a.1.2") != p.end());
        OSSIA::Int* i1 = (OSSIA::Int*) p.find("/a.0.0")->second;
        OSSIA::Int* i2 = (OSSIA::Int*) p.find("/a.0.1")->second;
        OSSIA::Int* i3 = (OSSIA::Int*) p.find("/a.1.0")->second;
        OSSIA::Int* i4 = (OSSIA::Int*) p.find("/a.1.1")->second;
        OSSIA::Int* i5 = (OSSIA::Int*) p.find("/a.1.2")->second;
        REQUIRE(i1->value == 3);
        REQUIRE(i2->value == -6);
        REQUIRE(i3->value == 9);
        REQUIRE(i4->value == 2);
        REQUIRE(i5->value == 11);
    }

    SECTION ("Empty array") {
        std::string json3 = R"_({"empty":[]})_";
        REQUIRE(ossia::presets::read_json(json3).size() == 0);
    }
}

TEST_CASE ("Parsing nested objects") {
    std::string json = R"_({"a":{"c":{"d":13,"e":5}}, "b":{"f1":true, "f2":{"f3":false}}})_";
    ossia::presets::Preset p = ossia::presets::read_json(json);
    REQUIRE(p.size() == 4);
    REQUIRE(p.find("/a/c/d") != p.end());
    REQUIRE(p.find("/a/c/e") != p.end());
    REQUIRE(p.find("/b/f1") != p.end());
    REQUIRE(p.find("/b/f2/f3") != p.end());
    OSSIA::Int* acd = (OSSIA::Int*) p.find("/a/c/d")->second;
    OSSIA::Int* ace = (OSSIA::Int*) p.find("/a/c/e")->second;
    OSSIA::Bool* bf1 = (OSSIA::Bool*) p.find("/b/f1")->second;
    OSSIA::Bool* bf2f3 = (OSSIA::Bool*) p.find("/b/f2/f3")->second;
    REQUIRE(acd->value == 13);
    REQUIRE(ace->value == 5);
    REQUIRE(bf1->value == true);
    REQUIRE(bf2f3->value == false);
}

TEST_CASE ("Empty object") {

    SECTION("Basic empty object") {
        std::string empty = R"_({"empty":{}})_";
        REQUIRE(ossia::presets::read_json(empty).size() == 0);
    }

    SECTION ("Complex empty object") {
        std::string empty = R"_({"a":{"b":[{"c":[],"d":{}},{}]},"e":{"f":[],"g":[{},{},{}]}})_";
        REQUIRE(ossia::presets::read_json(empty).size() == 0);
    }
}

TEST_CASE ("Parsing types") {
    std::string json = R"_({"a":1,"b":2.34234,"c":false, "d":"hello world","e":true})_";
    ossia::presets::Preset p = ossia::presets::read_json(json);
    REQUIRE(p.find("/a") != p.end());
    REQUIRE(p.find("/b") != p.end());
    REQUIRE(p.find("/c") != p.end());
    REQUIRE(p.find("/d") != p.end());
    REQUIRE(p.find("/e") != p.end());
    REQUIRE(p.find("/a")->second->getType() == OSSIA::Value::Type::INT);
    REQUIRE(p.find("/b")->second->getType() == OSSIA::Value::Type::FLOAT);
    REQUIRE(p.find("/c")->second->getType() == OSSIA::Value::Type::BOOL);
    REQUIRE(p.find("/d")->second->getType() == OSSIA::Value::Type::STRING);
    REQUIRE(p.find("/e")->second->getType() == OSSIA::Value::Type::BOOL);
}

TEST_CASE ("Building JSON array") {

    SECTION("Basic array") {
        ossia::presets::Preset p;

        OSSIA::Int i1 (5);
        OSSIA::Int i2 (4);
        OSSIA::Int i3 (3);
        OSSIA::Int i4 (2);
        OSSIA::Int i5 (1);

        p.insert(ossia::presets::PresetPair("/a.0", &i1));
        p.insert(ossia::presets::PresetPair("/a.1", &i2));
        p.insert(ossia::presets::PresetPair("/a.2", &i3));
        p.insert(ossia::presets::PresetPair("/a.3", &i4));
        p.insert(ossia::presets::PresetPair("/a.4", &i5));

        std::string json = ossia::presets::write_json(p);

        rapidjson::Document d;
        d.Parse(json.c_str());

        REQUIRE(d.IsObject());
        REQUIRE(d.MemberCount() == 1);
        REQUIRE(d.HasMember("a"));
        REQUIRE(d["a"].IsArray());
        rapidjson::Value arr (rapidjson::kArrayType);
        arr = d["a"];
        REQUIRE(arr.Size() == 5);
        for (int i = 0; i < 5; ++i) {
            REQUIRE(arr[i] == 5 - i);
        }
    }

    SECTION ("Nested array") {
        ossia::presets::Preset p;

        OSSIA::Int i1 (1);
        OSSIA::Int i2 (2);
        OSSIA::Int i3 (3);
        OSSIA::Int i4 (4);
        OSSIA::Int i5 (5);
        OSSIA::Int i6 (6);

        p.insert(ossia::presets::PresetPair ("/a.0.0.0", &i1));
        p.insert(ossia::presets::PresetPair ("/a.0.1.0", &i2));
        p.insert(ossia::presets::PresetPair ("/a.0.1.1", &i3));
        p.insert(ossia::presets::PresetPair ("/a.0.1.2", &i4));
        p.insert(ossia::presets::PresetPair ("/a.1.0", &i5));
        p.insert(ossia::presets::PresetPair ("/a.1.1", &i6));

        // a = [[[1], [2, 3, 4]], [5, 6]]

        std::string json = ossia::presets::write_json(p);

        rapidjson::Document d;
        d.Parse(json.c_str());

        REQUIRE(d.IsObject());
        REQUIRE(d.MemberCount() == 1);
        REQUIRE(d.HasMember("a"));
        REQUIRE(d["a"].IsArray());
        REQUIRE(d["a"].Size() == 2);
        REQUIRE(d["a"][0].IsArray());
        REQUIRE(d["a"][0].Size() == 2);
        REQUIRE(d["a"][0][0].IsArray());
        REQUIRE(d["a"][0][0].Size() == 1);
        REQUIRE(d["a"][0][1].IsArray());
        REQUIRE(d["a"][0][1].Size() == 3);
        REQUIRE(d["a"][1].IsArray());
        REQUIRE(d["a"][1].Size() == 2);

        REQUIRE(d["a"][0][0][0].GetInt() == i1.value);
        REQUIRE(d["a"][0][1][0].GetInt() == i2.value);
        REQUIRE(d["a"][0][1][1].GetInt() == i3.value);
        REQUIRE(d["a"][0][1][2].GetInt() == i4.value);
        REQUIRE(d["a"][1][0].GetInt() == i5.value);
        REQUIRE(d["a"][1][1].GetInt() == i6.value);
    }
}

TEST_CASE ("Building object"){
    ossia::presets::Preset p;

    OSSIA::Int i1 (1);
    OSSIA::Int i2 (2);
    OSSIA::Int i3 (3);
    OSSIA::Int i4 (4);

    p.insert(ossia::presets::PresetPair ("/a", &i1));
    p.insert(ossia::presets::PresetPair ("/b/c", &i2));
    p.insert(ossia::presets::PresetPair ("/b/d", &i3));
    p.insert(ossia::presets::PresetPair ("/b/e/f", &i4));

    std::string json = ossia::presets::write_json(p);

    rapidjson::Document d;
    d.Parse(json.c_str());

    REQUIRE(d.IsObject());
    REQUIRE(d.MemberCount() == 2);
    REQUIRE(d.HasMember("a"));
    REQUIRE(d["a"].IsInt());
    REQUIRE(d["a"].GetInt() == i1.value);
    REQUIRE(d.HasMember("b"));
    REQUIRE(d["b"].IsObject());
    REQUIRE(d["b"].MemberCount() == 3);
    REQUIRE(d["b"].HasMember("c"));
    REQUIRE(d["b"]["c"].IsInt());
    REQUIRE(d["b"]["c"].GetInt() == i2.value);
    REQUIRE(d["b"].HasMember("d"));
    REQUIRE(d["b"]["d"].IsInt());
    REQUIRE(d["b"]["d"].GetInt() == i3.value);
    REQUIRE(d["b"].HasMember("e"));
    REQUIRE(d["b"]["e"].IsObject());
    REQUIRE(d["b"]["e"].MemberCount() == 1);
    REQUIRE(d["b"]["e"].HasMember("f"));
    REQUIRE(d["b"]["e"]["f"].GetInt() == i4.value);
}

TEST_CASE ("Nested arrays and objects") {
    ossia::presets::Preset p;

    OSSIA::Int i1 (1);
    OSSIA::Int i2 (2);
    OSSIA::Int i3 (3);
    OSSIA::Int i4 (4);

    p.insert(ossia::presets::PresetPair ("/a.0/b/c.0", &i1));
    p.insert(ossia::presets::PresetPair ("/a.0/b/c.1", &i2));
    p.insert(ossia::presets::PresetPair ("/a.1/b.0/c", &i3));
    p.insert(ossia::presets::PresetPair ("/a.1/b.0/d", &i4));

    std::string json = ossia::presets::write_json(p);

    rapidjson::Document d;
    d.Parse(json.c_str());

    REQUIRE(d.IsObject());
    REQUIRE(d.MemberCount() == 1);
    REQUIRE(d.HasMember("a"));
    REQUIRE(d["a"].IsArray());
    REQUIRE(d["a"].Size() == 2);
    REQUIRE(d["a"][0].IsObject());
    REQUIRE(d["a"][0].HasMember("b"));
    REQUIRE(d["a"][0]["b"].IsObject());
    REQUIRE(d["a"][0]["b"].HasMember("c"));
    REQUIRE(d["a"][0]["b"]["c"].IsArray());
    REQUIRE(d["a"][0]["b"]["c"].Size() == 2);
    REQUIRE(d["a"][0]["b"]["c"][0].IsInt());
    REQUIRE(d["a"][0]["b"]["c"][0].GetInt() == i1.value);
    REQUIRE(d["a"][0]["b"]["c"][1].IsInt());
    REQUIRE(d["a"][0]["b"]["c"][1].GetInt() == i2.value);
    REQUIRE(d["a"][1].IsObject());
    REQUIRE(d["a"][1].HasMember("b"));
    REQUIRE(d["a"][1]["b"].IsArray());
    REQUIRE(d["a"][1]["b"].Size() == 1);
    REQUIRE(d["a"][1]["b"][0].IsObject());
    REQUIRE(d["a"][1]["b"][0].HasMember("c"));
    REQUIRE(d["a"][1]["b"][0]["c"].IsInt());
    REQUIRE(d["a"][1]["b"][0]["c"].GetInt() == i3.value);
    REQUIRE(d["a"][1]["b"][0].HasMember("d"));
    REQUIRE(d["a"][1]["b"][0]["d"].IsInt());
    REQUIRE(d["a"][1]["b"][0]["d"].GetInt() == i4.value);
}

TEST_CASE ("Types conversion") {
    ossia::presets::Preset p;

    OSSIA::Bool btrue (true);
    OSSIA::Bool bfalse (false);
    OSSIA::Char c ('2');
    OSSIA::Int i (15551);
    OSSIA::Float f (3.566);
    OSSIA::String s ("bonjour");

    p.insert(ossia::presets::PresetPair ("/true", &btrue));
    p.insert(ossia::presets::PresetPair ("/false", &bfalse));
    p.insert(ossia::presets::PresetPair ("/char", &c));
    p.insert(ossia::presets::PresetPair ("/int", &i));
    p.insert(ossia::presets::PresetPair ("/float", &f));
    p.insert(ossia::presets::PresetPair ("/string", &s));

    std::string json = ossia::presets::write_json(p);

    rapidjson::Document d;
    d.Parse(json.c_str());

    REQUIRE(d.IsObject());

    SECTION ("Bool") {
        REQUIRE(d.HasMember("true"));
        REQUIRE(d["true"].GetType() == rapidjson::kTrueType);
        REQUIRE(d.HasMember("false"));
        REQUIRE(d["false"].GetType() == rapidjson::kFalseType);
    }

    SECTION ("Char") {
        REQUIRE(d.HasMember("char"));
        REQUIRE(d["char"].IsString());
        REQUIRE(std::strcmp(d["char"].GetString(), "2") == 0);
    }

    SECTION ("String") {
        REQUIRE(d.HasMember("string"));
        REQUIRE(d["string"].IsString());
        REQUIRE(std::strcmp(d["string"].GetString(), "bonjour") == 0);
    }

    SECTION ("Int") {
        REQUIRE(d.HasMember("int"));
        REQUIRE(d["int"].IsInt());
        REQUIRE(d["int"].GetInt() == 15551);
    }

    SECTION ("Float") {
        REQUIRE(d.HasMember("float"));
        REQUIRE(d["float"].IsDouble());
        REQUIRE(d["float"].GetDouble() == Approx(3.566f).epsilon(0.001));
    }
}

TEST_CASE ("Exceptions") {
    SECTION ("Invalid JSON") {
        std::string invalidjson = R"_({{"a":true})_";
        REQUIRE_THROWS(ossia::presets::read_json(invalidjson));
    }
}
