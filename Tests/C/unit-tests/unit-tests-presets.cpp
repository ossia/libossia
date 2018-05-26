// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <iostream>
#include <string>
#include <rapidjson/document.h>
#include <ossia-c/preset/preset.h>
#include <ossia/preset/preset.hpp>

namespace ossia
{
auto find(ossia::presets::preset& p, ossia::string_view t)
{
  for(auto it = p.begin(); it != p.end(); ++it)
  {
    if(it->first == t)
      return it;
  }
  return p.end();
}
auto find(ossia::presets::preset& p, const char* t)
{
  return find(p, ossia::string_view{t});
}
}
TEST_CASE ("JSON array") {
    SECTION ("Basic array parsing") {

        std::string json = R"_({"a":[1, 2, 4, 8]})_";
        REQUIRE(ossia::presets::read_json(json).size() == 4);
    }

    SECTION ("Contents of array") {
        std::string json2 = R"_({"a":[[3, -6], [9, 2, 11]]})_";
        ossia::presets::preset p = ossia::presets::read_json(json2);
        REQUIRE(p.size() == 5);
        REQUIRE(ossia::find(p, "/a.0.0") != p.end());
        REQUIRE(ossia::find(p, "/a.0.1") != p.end());
        REQUIRE(ossia::find(p, "/a.1.0") != p.end());
        REQUIRE(ossia::find(p, "/a.1.1") != p.end());
        REQUIRE(ossia::find(p, "/a.1.2") != p.end());
        auto i1 = ossia::find(p, "/a.0.0")->second.get<int32_t>();
        auto i2 = ossia::find(p, "/a.0.1")->second.get<int32_t>();
        auto i3 = ossia::find(p, "/a.1.0")->second.get<int32_t>();
        auto i4 = ossia::find(p, "/a.1.1")->second.get<int32_t>();
        auto i5 = ossia::find(p, "/a.1.2")->second.get<int32_t>();
        REQUIRE(i1 == 3);
        REQUIRE(i2 == -6);
        REQUIRE(i3 == 9);
        REQUIRE(i4 == 2);
        REQUIRE(i5 == 11);
    }

    SECTION ("Empty array") {
        std::string json3 = R"_({"empty":[]})_";
        REQUIRE(ossia::presets::read_json(json3).size() == 0);
    }
}

TEST_CASE ("Parsing nested objects") {
    std::string json = R"_({"a":{"c":{"d":13,"e":5}}, "b":{"f1":true, "f2":{"f3":false}}})_";
    ossia::presets::preset p = ossia::presets::read_json(json);
    REQUIRE(p.size() == 4);
    REQUIRE(ossia::find(p, "/a/c/d") != p.end());
    REQUIRE(ossia::find(p, "/a/c/e") != p.end());
    REQUIRE(ossia::find(p, "/b/f1") != p.end());
    REQUIRE(ossia::find(p, "/b/f2/f3") != p.end());
    auto acd = ossia::find(p, "/a/c/d")->second.get<int32_t>();
    auto ace = ossia::find(p, "/a/c/e")->second.get<int32_t>();
    auto bf1 = ossia::find(p, "/b/f1")->second.get<bool>();
    auto bf2f3 = ossia::find(p, "/b/f2/f3")->second.get<bool>();
    REQUIRE(acd == 13);
    REQUIRE(ace == 5);
    REQUIRE(bf1 == true);
    REQUIRE(bf2f3 == false);
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
    ossia::presets::preset p = ossia::presets::read_json(json);
    REQUIRE(ossia::find(p, "/a") != p.end());
    REQUIRE(ossia::find(p, "/b") != p.end());
    REQUIRE(ossia::find(p, "/c") != p.end());
    REQUIRE(ossia::find(p, "/d") != p.end());
    REQUIRE(ossia::find(p, "/e") != p.end());
    REQUIRE(ossia::find(p, "/a")->second.getType() == ossia::val_type::INT);
    REQUIRE(ossia::find(p, "/b")->second.getType() == ossia::val_type::FLOAT);
    REQUIRE(ossia::find(p, "/c")->second.getType() == ossia::val_type::BOOL);
    REQUIRE(ossia::find(p, "/d")->second.getType() == ossia::val_type::STRING);
    REQUIRE(ossia::find(p, "/e")->second.getType() == ossia::val_type::BOOL);
}

TEST_CASE ("Building JSON array") {

    SECTION("Basic array") {
        ossia::presets::preset p;

        int32_t i1 (5);
        int32_t i2 (4);
        int32_t i3 (3);
        int32_t i4 (2);
        int32_t i5 (1);

        p.push_back(std::make_pair("/a.0", i1));
        p.push_back(std::make_pair("/a.1", i2));
        p.push_back(std::make_pair("/a.2", i3));
        p.push_back(std::make_pair("/a.3", i4));
        p.push_back(std::make_pair("/a.4", i5));

        std::string json = ossia::presets::write_json("device", p);

        rapidjson::Document doc;
        doc.Parse(json.c_str());

        REQUIRE(doc.IsObject());
        REQUIRE(doc.MemberCount() == 1);
        REQUIRE(doc.HasMember("device") == 1);
        auto& d = doc["device"];

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
        ossia::presets::preset p;

        int32_t i1 (1);
        int32_t i2 (2);
        int32_t i3 (3);
        int32_t i4 (4);
        int32_t i5 (5);
        int32_t i6 (6);

        p.push_back(std::make_pair ("/a.0.0.0", i1));
        p.push_back(std::make_pair ("/a.0.1.0", i2));
        p.push_back(std::make_pair ("/a.0.1.1", i3));
        p.push_back(std::make_pair ("/a.0.1.2", i4));
        p.push_back(std::make_pair ("/a.1.0", i5));
        p.push_back(std::make_pair ("/a.1.1", i6));

        // a = [[[1], [2, 3, 4]], [5, 6]]

        std::string json = ossia::presets::write_json("device", p);

        rapidjson::Document doc;
        doc.Parse(json.c_str());

        REQUIRE(doc.IsObject());
        REQUIRE(doc.MemberCount() == 1);
        REQUIRE(doc.HasMember("device") == 1);

        auto& d = doc["device"];
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

        REQUIRE(d["a"][0][0][0].GetInt() == i1);
        REQUIRE(d["a"][0][1][0].GetInt() == i2);
        REQUIRE(d["a"][0][1][1].GetInt() == i3);
        REQUIRE(d["a"][0][1][2].GetInt() == i4);
        REQUIRE(d["a"][1][0].GetInt() == i5);
        REQUIRE(d["a"][1][1].GetInt() == i6);
    }
}

TEST_CASE ("Building object"){
    ossia::presets::preset p;

    int32_t i1 (1);
    int32_t i2 (2);
    int32_t i3 (3);
    int32_t i4 (4);

    p.push_back(std::make_pair ("/a", i1));
    p.push_back(std::make_pair ("/b/c", i2));
    p.push_back(std::make_pair ("/b/d", i3));
    p.push_back(std::make_pair ("/b/e/f", i4));

    std::string json = ossia::presets::write_json("device", p);

    rapidjson::Document doc;
    doc.Parse(json.c_str());

    REQUIRE(doc.IsObject());
    REQUIRE(doc.MemberCount() == 1);
    REQUIRE(doc.HasMember("device") == 1);
    auto& d = doc["device"];

    REQUIRE(d.IsObject());
    REQUIRE(d.MemberCount() == 2);
    REQUIRE(d.HasMember("a"));
    REQUIRE(d["a"].IsInt());
    REQUIRE(d["a"].GetInt() == i1);
    REQUIRE(d.HasMember("b"));
    REQUIRE(d["b"].IsObject());
    REQUIRE(d["b"].MemberCount() == 3);
    REQUIRE(d["b"].HasMember("c"));
    REQUIRE(d["b"]["c"].IsInt());
    REQUIRE(d["b"]["c"].GetInt() == i2);
    REQUIRE(d["b"].HasMember("d"));
    REQUIRE(d["b"]["d"].IsInt());
    REQUIRE(d["b"]["d"].GetInt() == i3);
    REQUIRE(d["b"].HasMember("e"));
    REQUIRE(d["b"]["e"].IsObject());
    REQUIRE(d["b"]["e"].MemberCount() == 1);
    REQUIRE(d["b"]["e"].HasMember("f"));
    REQUIRE(d["b"]["e"]["f"].GetInt() == i4);
}

void make_preset(const ossia::net::node_base& n, ossia::presets::preset& p)
{
  if(auto addr = n.get_parameter())
  {

  }
}


TEST_CASE ("Device") {
    ossia::net::generic_device dev{""};
    auto& r = dev.get_root_node();
    ossia::net::create_node(r, "/width").create_parameter(ossia::val_type::FLOAT);
    ossia::net::create_node(r, "/leText.0/text").create_parameter(ossia::val_type::STRING);
    ossia::net::create_node(r, "/leText.1/text").create_parameter(ossia::val_type::STRING);
    ossia::net::create_node(r, "/leText.2/text").create_parameter(ossia::val_type::STRING);
    ossia::net::create_node(r, "/leText.3/text").create_parameter(ossia::val_type::STRING);
    ossia::net::create_node(r, "/leText.0/color").create_parameter(ossia::val_type::INT);
    ossia::net::create_node(r, "/leText.1/color").create_parameter(ossia::val_type::INT);
    ossia::net::create_node(r, "/leText.2/color").create_parameter(ossia::val_type::INT);
    ossia::net::create_node(r, "/leText.3/color").create_parameter(ossia::val_type::INT);
    ossia::net::create_node(r, "/leText.0/font.pointSize").create_parameter(ossia::val_type::INT);
    ossia::net::create_node(r, "/leText.1/font.pointSize").create_parameter(ossia::val_type::INT);
    ossia::net::create_node(r, "/leText.2/font.pointSize").create_parameter(ossia::val_type::INT);
    ossia::net::create_node(r, "/leText.3/font.pointSize").create_parameter(ossia::val_type::INT);

    auto p = ossia::presets::make_preset(dev);
    REQUIRE(p.size() == 13);

    for(auto& preset : p)
      std::cerr << preset.first << " : " << preset.second << " \n";

    std::string json = ossia::presets::write_json("device", p);

    rapidjson::Document d;
    d.Parse(json.c_str());
}

TEST_CASE ("Instances") {
    ossia::presets::preset p;
    using namespace std::literals;

    p.push_back(std::make_pair ("/width", 123));
    p.push_back(std::make_pair ("/leText.0/text", "foo"s));
    p.push_back(std::make_pair ("/leText.0/color", "bar"s));
    p.push_back(std::make_pair ("/leText.0/font.pointSize", 456));
    p.push_back(std::make_pair ("/leText.1/text", "foo"s));
    p.push_back(std::make_pair ("/leText.1/color", "bar"s));
    p.push_back(std::make_pair ("/leText.1/font.pointSize", 456));
    p.push_back(std::make_pair ("/leText.2/text", "foo"s));
    p.push_back(std::make_pair ("/leText.2/color", "bar"s));
    p.push_back(std::make_pair ("/leText.2/font.pointSize", 456));
    p.push_back(std::make_pair ("/leText.3/text", "foo"s));
    p.push_back(std::make_pair ("/leText.3/color", "bar"s));
    p.push_back(std::make_pair ("/leText.3/font.pointSize", 456));

    std::string json = ossia::presets::write_json("device", p);

    rapidjson::Document d;
    d.Parse(json.c_str());
}

TEST_CASE ("Nested arrays and objects") {
    ossia::presets::preset p;

    int32_t i1 (1);
    int32_t i2 (2);
    int32_t i3 (3);
    int32_t i4 (4);

    p.push_back(std::make_pair ("/a.0/b/c.0", i1));
    p.push_back(std::make_pair ("/a.0/b/c.1", i2));
    p.push_back(std::make_pair ("/a.1/b.0/c", i3));
    p.push_back(std::make_pair ("/a.1/b.0/d", i4));

    std::string json = ossia::presets::write_json("device", p);

    rapidjson::Document doc;
    doc.Parse(json.c_str());

    REQUIRE(doc.IsObject());
    REQUIRE(doc.MemberCount() == 1);
    REQUIRE(doc.HasMember("device") == 1);
    auto& d = doc["device"];
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
    REQUIRE(d["a"][0]["b"]["c"][0].GetInt() == i1);
    REQUIRE(d["a"][0]["b"]["c"][1].IsInt());
    REQUIRE(d["a"][0]["b"]["c"][1].GetInt() == i2);
    REQUIRE(d["a"][1].IsObject());
    REQUIRE(d["a"][1].HasMember("b"));
    REQUIRE(d["a"][1]["b"].IsArray());
    REQUIRE(d["a"][1]["b"].Size() == 1);
    REQUIRE(d["a"][1]["b"][0].IsObject());
    REQUIRE(d["a"][1]["b"][0].HasMember("c"));
    REQUIRE(d["a"][1]["b"][0]["c"].IsInt());
    REQUIRE(d["a"][1]["b"][0]["c"].GetInt() == i3);
    REQUIRE(d["a"][1]["b"][0].HasMember("d"));
    REQUIRE(d["a"][1]["b"][0]["d"].IsInt());
    REQUIRE(d["a"][1]["b"][0]["d"].GetInt() == i4);
}

TEST_CASE ("Types conversion") {
    ossia::presets::preset p;

    bool btrue (true);
    bool bfalse (false);
    char c ('2');
    int32_t i (15551);
    float f (3.566);
    std::string s ("bonjour");

    ossia::value v = c;
    REQUIRE(v.getType() == ossia::val_type::CHAR);
    REQUIRE(*v.target<char>() == '2');
    p.push_back(std::make_pair ("/true", btrue));
    p.push_back(std::make_pair ("/false", bfalse));
    p.push_back(std::make_pair ("/char", c));
    p.push_back(std::make_pair ("/int", i));
    p.push_back(std::make_pair ("/float", f));
    p.push_back(std::make_pair ("/string", s));

    std::string json = ossia::presets::write_json("device", p);

    rapidjson::Document doc;
    doc.Parse(json.c_str());

    REQUIRE(doc.IsObject());
    REQUIRE(doc.MemberCount() == 1);
    REQUIRE(doc.HasMember("device") == 1);
    auto& d = doc["device"];

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
