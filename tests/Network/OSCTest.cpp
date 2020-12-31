// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include "TestUtils.hpp"
#include "ProtocolTestUtils.hpp"

#if defined(OSSIA_PROTOCOL_OSC)
#include <ossia/network/osc/osc.hpp>
#endif

#if defined(OSSIA_PROTOCOL_OSC)
TEST_CASE ("test_bundle", "test_bundle")
    {
        ossia::net::generic_device osc_A{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc"};
        ossia::net::generic_device osc_B{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996), "test_osc"};

        auto a1 = ossia::net::create_node(osc_A, "/foo/bar.0").create_parameter(ossia::val_type::FLOAT);
        auto a2 = ossia::net::create_node(osc_A, "/foo/bar.1").create_parameter(ossia::val_type::FLOAT);
        auto a3 = ossia::net::create_node(osc_A, "/foo/bar.2").create_parameter(ossia::val_type::FLOAT);
        auto a4 = ossia::net::create_node(osc_A, "/foo/bar.3").create_parameter(ossia::val_type::FLOAT);

        auto b1 = ossia::net::create_node(osc_B, "/foo/bar.0").create_parameter(ossia::val_type::FLOAT);
        auto b2 = ossia::net::create_node(osc_B, "/foo/bar.1").create_parameter(ossia::val_type::FLOAT);
        auto b3 = ossia::net::create_node(osc_B, "/foo/bar.2").create_parameter(ossia::val_type::FLOAT);
        auto b4 = ossia::net::create_node(osc_B, "/foo/bar.3").create_parameter(ossia::val_type::FLOAT);

        for(auto b : {b1, b2, b3, b4})
        {
          b->set_value(1234.);
        }

        auto& b_proto = (ossia::net::osc_protocol&) osc_B.get_protocol();

        b_proto.push_bundle({b1, b2, b3, b4});

        std::this_thread::sleep_for(std::chrono::microseconds(10000));
        REQUIRE(a1->value() == ossia::value{1234.});
        REQUIRE(a2->value() == ossia::value{1234.});
        REQUIRE(a3->value() == ossia::value{1234.});
        REQUIRE(a4->value() == ossia::value{1234.});
    }

TEST_CASE ("test_bundle_raw", "test_bundle_raw")
    {
        ossia::net::generic_device osc_A{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc"};
        ossia::net::generic_device osc_B{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996), "test_osc"};

        auto a1 = ossia::net::create_node(osc_A, "/foo/bar.0").create_parameter(ossia::val_type::FLOAT);
        auto a2 = ossia::net::create_node(osc_A, "/foo/bar.1").create_parameter(ossia::val_type::FLOAT);
        auto a3 = ossia::net::create_node(osc_A, "/foo/bar.2").create_parameter(ossia::val_type::FLOAT);
        auto a4 = ossia::net::create_node(osc_A, "/foo/bar.3").create_parameter(ossia::val_type::FLOAT);

        int recv = 0;
        a1->add_callback([&] (const auto& v) { recv++; });
        a2->add_callback([&] (const auto& v) { recv++; });
        a3->add_callback([&] (const auto& v) { recv++; });
        a4->add_callback([&] (const auto& v) { recv++; });
        auto& b_proto = (ossia::net::osc_protocol&) osc_B.get_protocol();

        std::vector<ossia::net::full_parameter_data> vec(4);
        vec[0].address = "/foo/bar.0";
        vec[0].set_value(0.);
        vec[1].address = "/foo/bar.1";
        vec[1].set_value(1.);
        vec[2].address = "/foo/bar.2";
        vec[2].set_value(2.);
        vec[3].address = "/foo/bar.3";
        vec[3].set_value(3.);

        b_proto.push_raw_bundle(vec);

        while(recv < 4)
          std::this_thread::sleep_for(std::chrono::milliseconds(10));

        REQUIRE(a1->value() == ossia::value{0.});
        REQUIRE(a2->value() == ossia::value{1.});
        REQUIRE(a3->value() == ossia::value{2.});
        REQUIRE(a4->value() == ossia::value{3.});
    }

TEST_CASE ("test_pattern_match", "test_pattern_match")
    {
        ossia::net::generic_device osc_A{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc"};
        ossia::net::generic_device osc_B{
          std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996), "test_osc"};

        auto a1 = ossia::net::create_node(osc_A, "/foo/bar.0").create_parameter(ossia::val_type::FLOAT);
        auto a2 = ossia::net::create_node(osc_A, "/foo/bar.1").create_parameter(ossia::val_type::FLOAT);
        auto a3 = ossia::net::create_node(osc_A, "/foo/bar.2").create_parameter(ossia::val_type::FLOAT);
        auto a4 = ossia::net::create_node(osc_A, "/foo/bar.3").create_parameter(ossia::val_type::FLOAT);

        int recv = 0;
        a1->add_callback([&] (const auto& v) { recv++; });
        a2->add_callback([&] (const auto& v) { recv++; });
        a3->add_callback([&] (const auto& v) { recv++; });
        a4->add_callback([&] (const auto& v) { recv++; });
        auto& b_proto = (ossia::net::osc_protocol&) osc_B.get_protocol();

        ossia::net::full_parameter_data dat{"/foo/bar.*"};
        dat.set_value(2.3f);
        b_proto.push_raw(dat);

        while(recv < 4)
          std::this_thread::sleep_for(std::chrono::milliseconds(10));

        REQUIRE(a1->value() == ossia::value{2.3});
        REQUIRE(a2->value() == ossia::value{2.3});
        REQUIRE(a3->value() == ossia::value{2.3});
        REQUIRE(a4->value() == ossia::value{2.3});
    }
#endif



#if defined(OSSIA_PROTOCOL_OSC)
TEST_CASE ("test_comm_osc", "test_comm_osc")
  {
    test_comm_generic([] { return std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997); },
                      [] { return std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996); });
  }
#endif
