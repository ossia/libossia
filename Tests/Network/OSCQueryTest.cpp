// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>
#include <QtTest>
#include <ossia/context.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <iostream>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include "TestUtils.hpp"

using namespace ossia;
using namespace ossia::net;
class OSCQueryTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void test_oscquery()
    {
        auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
        generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
        TestDeviceRef d{serv};
        auto& dev = d.device;

        auto& n = find_or_create_node(serv, "/main");
        auto a = n.create_parameter(ossia::val_type::FLOAT);

        a->push_value(6.5);

        n.set(access_mode_attribute{}, access_mode::GET);
        n.set(bounding_mode_attribute{}, bounding_mode::FOLD);
        n.set(domain_attribute{}, make_domain(-10., 10.));
        n.set(default_value_attribute{}, ossia::value(0.));
        n.set(tags_attribute{}, tags{"fancy", "wow", "1234"});
        n.set(refresh_rate_attribute{}, 100);
        n.set(value_step_size_attribute{}, 0.5);
        n.set(repetition_filter_attribute{}, repetition_filter::ON);
        n.set(critical_attribute{}, true);
        n.set(unit_attribute{}, meter_per_second_u{});
        n.set(priority_attribute{}, 50);
        n.set(description_attribute{}, "Such a fancy node?! Incredible! すごい!!");
        n.set(extended_type_attribute{}, "custom");
        n.set(app_name_attribute{}, "AppName");
        n.set(app_version_attribute{}, "1.0.0");
        n.set(app_creator_attribute{}, "Lelouch vi Brittania");



        // Node -> json
        auto str = ossia::oscquery::json_writer::query_namespace(dev);
        std::cerr << str.GetString() << std::endl;

        // Clear the device
        dev.clear_children();

        // Parse json
        rapidjson::Document doc;
        doc.Parse( str.GetString() );

        // Json -> node
        ossia::oscquery::json_parser::parse_namespace(dev, doc);

        {
            auto node = find_node(dev, "/main");
            QVERIFY(node);
            auto& n = *node;

            QVERIFY((bool)get_access_mode(n));
            QCOMPARE(*get_access_mode(n), access_mode::GET);

            QVERIFY((bool)get_bounding_mode(n));
            QCOMPARE(*get_bounding_mode(n), bounding_mode::FOLD);

            QVERIFY((bool)get_domain(n));
            QCOMPARE(get_domain(n), make_domain(-10., 10.));

            QVERIFY((bool)get_default_value(n));
            QCOMPARE(*get_default_value(n), ossia::value(0));

            tags the_tags{"fancy", "wow", "1234"};
            QVERIFY((bool)get_tags(n));
            QCOMPARE(*get_tags(n), the_tags);

            QVERIFY((bool)get_refresh_rate(n));
            QCOMPARE(*get_refresh_rate(n), 100);

            QVERIFY((bool)get_value_step_size(n));
            QCOMPARE(*get_value_step_size(n), 0.5);

            QVERIFY(get_repetition_filter(n));
            QCOMPARE(get_repetition_filter(n), repetition_filter::ON);

            QVERIFY(get_critical(n));

            QVERIFY((bool)get_unit(n));
            QCOMPARE(get_unit(n), ossia::unit_t(meter_per_second_u{}));

            QVERIFY((bool)get_priority(n));
            QCOMPARE(*get_priority(n), 50.f);

            QVERIFY((bool)get_description(n));
            QCOMPARE(*get_description(n), std::string("Such a fancy node?! Incredible! すごい!!"));

            QVERIFY((bool)get_extended_type(n));
            QCOMPARE(*get_extended_type(n), std::string("custom"));

            QVERIFY((bool)get_app_name(n));
            QCOMPARE(*get_app_name(n), std::string("AppName"));

            QVERIFY((bool)get_app_version(n));
            QCOMPARE(*get_app_version(n), std::string("1.0.0"));

            QVERIFY((bool)get_app_creator(n));
            QCOMPARE(*get_app_creator(n), std::string("Lelouch vi Brittania"));
        }
    }

    void test_json_impulse()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      auto json = oscquery::json_writer{}.query_namespace(dev.impulse_addr->get_node());
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "I"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 1);
        QVERIFY(doc["VALUE"][0].IsNull());
      }
    }

    void test_json_bool()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      {
        dev.bool_addr->push_value(false);
        auto json = oscquery::json_writer{}.query_namespace(dev.bool_addr->get_node());
        qDebug() << json.GetString();
        {
          rapidjson::Document doc;
          doc.Parse(json.GetString());
          QVERIFY(doc.IsObject());
          QVERIFY(doc.HasMember("TYPE"));
          QVERIFY(doc.HasMember("VALUE"));
          QVERIFY(doc["TYPE"].IsString());
          QVERIFY(doc["TYPE"].GetString() == "F"s);

          QVERIFY(doc["VALUE"].IsArray());
          QVERIFY(doc["VALUE"].GetArray().Size() == 1);
          QVERIFY(doc["VALUE"][0].IsNull());
        }
      }
      {
        dev.bool_addr->push_value(true);
        auto json = oscquery::json_writer{}.query_namespace(dev.bool_addr->get_node());
        qDebug() << json.GetString();
        {
          rapidjson::Document doc;
          doc.Parse(json.GetString());
          QVERIFY(doc.IsObject());
          QVERIFY(doc.HasMember("TYPE"));
          QVERIFY(doc.HasMember("VALUE"));
          QVERIFY(doc["TYPE"].IsString());
          QVERIFY(doc["TYPE"].GetString() == "T"s);

          QVERIFY(doc["VALUE"].IsArray());
          QVERIFY(doc["VALUE"].GetArray().Size() == 1);
          QVERIFY(doc["VALUE"][0].IsNull());
        }
      }
    }

    void test_json_int()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.int_addr->push_value(1234);
      auto json = oscquery::json_writer{}.query_namespace(dev.int_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "i"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 1);
        QVERIFY(doc["VALUE"][0].IsInt());
        QCOMPARE(doc["VALUE"][0].GetInt(), 1234);
      }
    }

    void test_json_float()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.float_addr->push_value(1234.f);
      auto json = oscquery::json_writer{}.query_namespace(dev.float_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "f"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 1);
        QVERIFY(doc["VALUE"][0].IsFloat());
        QCOMPARE(doc["VALUE"][0].GetFloat(), 1234.f);
      }
    }

    void test_json_string()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.string_addr->push_value("hello world");
      auto json = oscquery::json_writer{}.query_namespace(dev.string_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "s"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 1);
        QVERIFY(doc["VALUE"][0].IsString());
        QCOMPARE(doc["VALUE"][0].GetString(), "hello world");
      }
    }


    void test_json_char()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.char_addr->push_value('x');
      auto json = oscquery::json_writer{}.query_namespace(dev.char_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "c"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 1);
        QVERIFY(doc["VALUE"][0].IsString());
        QCOMPARE(doc["VALUE"][0].GetString(), "x");
      }
    }


    void test_json_vec2f()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.vec2f_addr->push_value(ossia::make_vec(123, 456));
      auto json = oscquery::json_writer{}.query_namespace(dev.vec2f_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "ff"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 2);
        QVERIFY(doc["VALUE"][0].IsFloat());
        QCOMPARE(doc["VALUE"][0].GetFloat(), 123.f);
        QVERIFY(doc["VALUE"][1].IsFloat());
        QCOMPARE(doc["VALUE"][1].GetFloat(), 456.f);
      }
    }

    void test_json_vec3f()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.vec3f_addr->push_value(ossia::make_vec(123, 456, 789));
      auto json = oscquery::json_writer{}.query_namespace(dev.vec3f_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "fff"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 3);
        QVERIFY(doc["VALUE"][0].IsFloat());
        QCOMPARE(doc["VALUE"][0].GetFloat(), 123.f);
        QVERIFY(doc["VALUE"][1].IsFloat());
        QCOMPARE(doc["VALUE"][1].GetFloat(), 456.f);
        QVERIFY(doc["VALUE"][2].IsFloat());
        QCOMPARE(doc["VALUE"][2].GetFloat(), 789.f);
      }
    }


    void test_json_vec4f()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.vec4f_addr->push_value(ossia::make_vec(123, 456, 789, 10));
      auto json = oscquery::json_writer{}.query_namespace(dev.vec4f_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "ffff"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 4);
        QVERIFY(doc["VALUE"][0].IsFloat());
        QCOMPARE(doc["VALUE"][0].GetFloat(), 123.f);
        QVERIFY(doc["VALUE"][1].IsFloat());
        QCOMPARE(doc["VALUE"][1].GetFloat(), 456.f);
        QVERIFY(doc["VALUE"][2].IsFloat());
        QCOMPARE(doc["VALUE"][2].GetFloat(), 789.f);
        QVERIFY(doc["VALUE"][3].IsFloat());
        QCOMPARE(doc["VALUE"][3].GetFloat(), 10.f);
      }
    }


    void test_json_list()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.tuple_addr->push_value(
            std::vector<ossia::value>{1.234,
                                      "foobar",
                                      std::vector<ossia::value>{ 45, 43},
                                      std::vector<ossia::value>{ }
             });
      auto json = oscquery::json_writer{}.query_namespace(dev.tuple_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "fs[ii][]"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 4);
        QVERIFY(doc["VALUE"][0].IsFloat());
        QCOMPARE(doc["VALUE"][0].GetFloat(), 1.234f);
        QVERIFY(doc["VALUE"][1].IsString());
        QCOMPARE(doc["VALUE"][1].GetString(), "foobar");
        QVERIFY(doc["VALUE"][2].IsArray());
        QCOMPARE(doc["VALUE"][2].GetArray().Size(), (unsigned int)2);
        QCOMPARE(doc["VALUE"][2][0].GetInt(), 45);
        QCOMPARE(doc["VALUE"][2][1].GetInt(), 43);
        QVERIFY(doc["VALUE"][3].IsArray());
        QCOMPARE(doc["VALUE"][3].GetArray().Size(), (unsigned int)0);
      }
    }
    void test_json_rgba8()
    {
      /*
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.vec4f_addr->push_value(ossia::rgba8_u{});
      dev.vec4f_addr->push_value(ossia::make_vec(123, 456, 789, 10));
      auto json = oscquery::json_writer{}.query_namespace(dev.int_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "ffff"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 4);
        QVERIFY(doc["VALUE"][0].IsFloat());
        QCOMPARE(doc["VALUE"][0].GetFloat(), 123.f);
        QVERIFY(doc["VALUE"][1].IsFloat());
        QCOMPARE(doc["VALUE"][1].GetFloat(), 456.f);
        QVERIFY(doc["VALUE"][2].IsFloat());
        QCOMPARE(doc["VALUE"][2].GetFloat(), 789.f);
        QVERIFY(doc["VALUE"][3].IsFloat());
        QCOMPARE(doc["VALUE"][3].GetFloat(), 10.f);
      }
      */
    }

    void test_json_unit_float()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.float_addr->set_unit(ossia::midigain_u{});
      dev.float_addr->push_value(100);
      auto json = oscquery::json_writer{}.query_namespace(dev.float_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "f"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 1);
        QVERIFY(doc["VALUE"][0].IsFloat());
        QCOMPARE(doc["VALUE"][0].GetFloat(), 100.f);

        QVERIFY(doc["UNIT"].IsString());
        QVERIFY(doc["UNIT"].GetString() == "gain.midigain"s);
      }
    }

    void test_json_unit_vec3()
    {
      using namespace std::literals;
      generic_device serv{"foo"};
      TestDeviceRef dev{serv};

      dev.vec3f_addr->set_unit(ossia::cartesian_3d_u{});
      dev.vec3f_addr->push_value(ossia::make_vec(1., 2., 3.));
      auto json = oscquery::json_writer{}.query_namespace(dev.vec3f_addr->get_node());
      qDebug() << json.GetString();
      {
        rapidjson::Document doc;
        doc.Parse(json.GetString());
        QVERIFY(doc.IsObject());
        QVERIFY(doc.HasMember("TYPE"));
        QVERIFY(doc.HasMember("VALUE"));
        QVERIFY(doc["TYPE"].IsString());
        QVERIFY(doc["TYPE"].GetString() == "fff"s);

        QVERIFY(doc["VALUE"].IsArray());
        QVERIFY(doc["VALUE"].GetArray().Size() == 3);
        QVERIFY(doc["VALUE"][0].IsFloat());
        QCOMPARE(doc["VALUE"][0].GetFloat(), 1.f);
        QVERIFY(doc["VALUE"][1].IsFloat());
        QCOMPARE(doc["VALUE"][1].GetFloat(), 2.f);
        QVERIFY(doc["VALUE"][2].IsFloat());
        QCOMPARE(doc["VALUE"][2].GetFloat(), 3.f);

        QVERIFY(doc["UNIT"].IsString());
        QVERIFY(doc["UNIT"].GetString() == "position.cart3D"s);
      }
    }
    void test_oscquery_http()
    {
        auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
        generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
        TestDeviceRef dev{serv}; (void) dev;
        ossia::net::parameter_base* a{};
        {
            auto& n = find_or_create_node(serv, "/main");
            a = n.create_parameter(ossia::val_type::FLOAT);

            a->push_value(6);

            n.set(access_mode_attribute{}, access_mode::GET);
            n.set(bounding_mode_attribute{}, bounding_mode::FOLD);
            n.set(domain_attribute{}, make_domain(-10., 10.));
            n.set(default_value_attribute{}, ossia::value(0.));
            n.set(tags_attribute{}, tags{"fancy", "wow", "1234"});
            n.set(refresh_rate_attribute{}, 100);
            n.set(value_step_size_attribute{}, 0.5);
            n.set(repetition_filter_attribute{}, repetition_filter::ON);
            n.set(critical_attribute{}, false);
            n.set(unit_attribute{}, meter_per_second_u{});
            n.set(priority_attribute{}, 50);
            n.set(description_attribute{}, "Such a fancy node?! Incredible! すごい!!");
            n.set(extended_type_attribute{}, "custom");
            n.set(app_name_attribute{}, "AppName");
            n.set(app_version_attribute{}, "1.0.0");
            n.set(app_creator_attribute{}, "Lelouch vi Brittania");
        }

        // HTTP client
        auto http_proto = new ossia::oscquery::oscquery_mirror_protocol("http://127.0.0.1:5678", 10000);
        auto http_clt = new generic_device{std::unique_ptr<ossia::net::protocol_base>(http_proto), "B"};


        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        http_proto->update(http_clt->get_root_node());

        {
            net::full_parameter_data d; d.address = "/float"; d.set_value(123.f);
            http_proto->push_raw(d);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // WS client
        auto ws_proto = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5678", 10001);
        auto ws_clt = new generic_device{std::unique_ptr<ossia::net::protocol_base>(ws_proto), "B"};

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        ws_proto->update(ws_clt->get_root_node());

        double d{7.8};
        a->push_value(d);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        auto node = find_node(ws_clt->get_root_node(), "/main");
        QVERIFY(node);
        auto param = node->get_parameter();
        QVERIFY(param);
        auto b = param->value();
        std::cout << "new value : " << b << " expecting " << d << std::endl;
        ossia::value expected_value{d};

        net::full_parameter_data d2; d2.address = "/int"; d2.set_value(546);
        ws_proto->push_raw(d2);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        delete ws_clt;

        // should use QCOMPARE after device cleaning to avoid hang
        QCOMPARE(b, expected_value);
    }

    void test_oscquery_critical_http()
    {
        auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
        generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
        TestDeviceRef dev{serv}; (void) dev;
        ossia::net::parameter_base* a{};
        {
            auto& n = find_or_create_node(serv, "/main");
            a = n.create_parameter(ossia::val_type::FLOAT);
            a->push_value(6);
            n.set(critical_attribute{}, true);
        }

        // HTTP client
        auto http_proto = new ossia::oscquery::oscquery_mirror_protocol("http://127.0.0.1:5678", 10000);
        std::unique_ptr<generic_device> http_clt{new generic_device{std::unique_ptr<ossia::net::protocol_base>(http_proto), "B"}};


        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        http_proto->update(http_clt->get_root_node());

        auto node = find_node(http_clt->get_root_node(), "/main");
        QVERIFY(node);
        auto param = node->get_parameter();
        QVERIFY(param);
        QCOMPARE(param->value().get<float>(), 6.f);

        {
            net::full_parameter_data d; d.address = "/main"; d.set_value(4.5f);
            http_proto->push_raw(d); //  no way to send raw OSC through http requests: this will god through the OSC port
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        QCOMPARE(a->value().get<float>(), 4.5f);
    }


    void test_oscquery_critical_ws()
    {
        auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
        generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
        TestDeviceRef dev{serv}; (void) dev;
        ossia::net::parameter_base* a{};
        {
            auto& n = find_or_create_node(serv, "/main");
            a = n.create_parameter(ossia::val_type::FLOAT);
            a->push_value(6);
            n.set(critical_attribute{}, true);
        }

        // WS client
        auto ws_proto = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5678", 10001);
        std::unique_ptr<generic_device> ws_clt{new generic_device{std::unique_ptr<ossia::net::protocol_base>(ws_proto), "B"}};

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        ws_proto->update(ws_clt->get_root_node());
        auto node = find_node(ws_clt->get_root_node(), "/main");
        QVERIFY(node);
        auto param = node->get_parameter();
        QVERIFY(param);
        QCOMPARE(param->value().get<float>(), 6.f);

        {
          param->push_value(4.5f); // This will god through the WS port
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::cout << "new value : " << a->value() << " expecting " << 4.5f << std::endl;
        // should use QCOMPARE after device cleaning to avoid hang
        QCOMPARE(a->value().get<float>(), 4.5f);

    }

};


QTEST_APPLESS_MAIN(OSCQueryTest)

#include "OSCQueryTest.moc"

