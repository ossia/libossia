// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/context.hpp>
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_model_property.hpp>
#include <ossia-qt/device/qml_property.hpp>
#include <ossia-qt/qml_plugin.hpp>
#include <ossia-qt/js_utilities.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/domain/domain.hpp>
#include <fmt/format.h>
#include <QQmlEngine>
#include <QQmlComponent>

std::vector<ossia::value> testOssiaValues()
{
  std::vector<ossia::value> vals{
    ossia::value{},
    ossia::value{ossia::impulse{}},
    ossia::value{123},
    ossia::value{456.},
    ossia::value{"foo"},
    ossia::value{true},
    ossia::value{false},
    ossia::value{ossia::make_vec(1, 2)},
    ossia::value{ossia::make_vec(1, 2, 3)},
    ossia::value{ossia::make_vec(1, 2, 3, 4)},
    ossia::value{std::vector<ossia::value>{3, "foo", 'c'}}
  };
  return vals;
}

std::vector<QJSValue> testJSValues()
{
  std::vector<QJSValue> vals{
    QJSValue{},
    QJSValue{1234},
    QJSValue{true},
    QJSValue{false},
    QJSValue{123.5456},
    QJSValue{"foo bar baz"}
  };
  {
    QJSValue vec2;
    vec2.setProperty(0, 1.23);
    vec2.setProperty(1, 3.23);
    vals.push_back(vec2);
    QJSValue vec3;
    vec3.setProperty(0, 1.23);
    vec3.setProperty(1, 6.23);
    vec3.setProperty(2, 4.23);
    vals.push_back(vec3);
    QJSValue vec4;
    vec4.setProperty(0, 1.23);
    vec4.setProperty(1, 6.23);
    vec4.setProperty(2, 7.23);
    vec4.setProperty(3, 9.23);
    vals.push_back(vec4);

    {
      for(int N = 1; N < 6; N++)
      {
        QJSValue tuple;
        for(int i = 0; i < N; i++)
        {
          tuple.setProperty(i, vals[i]);
        }
        vals.push_back(tuple);
      }
    }
  }
  {
    QJSValue vec2;
    vec2.setProperty(0, "1.23");
    vec2.setProperty(1, "3.23");
    vals.push_back(vec2);
    QJSValue vec3;
    vec3.setProperty(0, "1.23");
    vec3.setProperty(1, "6.23");
    vec3.setProperty(2, "4.23");
    vals.push_back(vec3);
    QJSValue vec4;
    vec4.setProperty(0, "1.23");
    vec4.setProperty(1, "6.23");
    vec4.setProperty(2, "7.23");
    vec4.setProperty(3, "9.23");
    vals.push_back(vec4);
  }

  QJSValue obj;
  obj.setProperty("foo", 1123);
  obj.setProperty("bar", vals[4]);
  vals.push_back(obj);
  return vals;
}

TEST_CASE ("test_value_conversion", "test_value_conversion")
{
  // TODO test undefined oscquery

  {
    ossia::value ov{ossia::impulse{}};
    QJSValue jv;
    REQUIRE(ossia::qt::value_from_js(ov, jv) == ov);
  }
  {
    ossia::value ov{1234};
    QJSValue jv = 5674;
    REQUIRE(ossia::qt::value_from_js(ov, jv) == ossia::value(5674));
  }
  {
    ossia::value ov{1234};
    QJSValue jv = 5674.;
    REQUIRE(ossia::qt::value_from_js(ov, jv) == ossia::value(5674));
  }
  {
    ossia::value ov{1234};
    QJSValue jv = "5674";
    REQUIRE(ossia::qt::value_from_js(ov, jv) == ossia::value(5674));
  }
  {
    ossia::value ov{1234.};
    QJSValue jv = 5674;
    REQUIRE(ossia::qt::value_from_js(ov, jv) == ossia::value(5674.));
  }
  {
    ossia::value ov{"1234"};
    QJSValue jv = 5674;
    REQUIRE(ossia::qt::value_from_js(ov, jv) == ossia::value("5674"));
  }

  for(auto& j : testJSValues())
  {
    auto val = ossia::qt::value_from_js(j);
  }

  for(auto& o : testOssiaValues())
  {
    auto val = ossia::qt::value_to_js_string(o);
  }


  int argc{}; char** argv{};
  QCoreApplication app{argc, argv};
  QJSEngine e;
  for(auto& v : testOssiaValues())
  {
    for(auto& j : testJSValues())
    {
      auto val = ossia::qt::value_from_js(v, j);
      ossia::qt::value_to_js_value(v, e);
      ossia::qt::value_to_js_value(val, e);
    }
  }
}
