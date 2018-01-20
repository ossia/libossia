// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/context.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <iostream>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/oscquery/detail/http_query_parser.hpp>
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
        TestDeviceRef dev{serv};
        {
          auto& n = find_or_create_node(serv, "/main");
          auto a = n.create_parameter(ossia::val_type::FLOAT);

          a->push_value(6);

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
        }

        // HTTP client
        auto http_proto = new ossia::oscquery::oscquery_mirror_protocol("http://127.0.0.1:5678", 10000);
        auto http_clt = new generic_device{std::unique_ptr<ossia::net::protocol_base>(http_proto), "B"};


        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        http_proto->update(http_clt->get_root_node());

        net::full_parameter_data d; d.address = "/float"; d.set_value(123.f);
        http_proto->push_raw(d);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // WS client
        auto ws_proto = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5678", 10001);
        auto ws_clt = new generic_device{std::unique_ptr<ossia::net::protocol_base>(ws_proto), "B"};

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        ws_proto->update(ws_clt->get_root_node());

        net::full_parameter_data d2; d2.address = "/int"; d2.set_value(546);
        ws_proto->push_raw(d2);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        delete ws_proto;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        delete http_proto;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
};


QTEST_APPLESS_MAIN(OSCQueryTest)

#include "OSCQueryTest.moc"

