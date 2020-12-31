// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include "TestUtils.hpp"
#include "ProtocolTestUtils.hpp"

#include <iostream>

#if defined(OSSIA_PROTOCOL_OSCQUERY)
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#endif
#if defined(OSSIA_PROTOCOL_OSCQUERY)
TEST_CASE ("test_oscquery_multi", "test_oscquery_multi")
  {
    try {

    ossia::net::generic_device device1{std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 4567), "x"};
    ossia::net::generic_device device2{std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 4567), "y"};
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } catch (const std::exception& e) {

    }
  }
#endif


#if defined(OSSIA_PROTOCOL_OSCQUERY)
TEST_CASE ("test_comm_oscquery", "test_comm_oscquery")
  {
    try {
      test_comm_generic([] { return std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 5678); },
                        [] { return std::make_unique<ossia::oscquery::oscquery_mirror_protocol>("ws://127.0.0.1:5678"); });
    }
    catch(const std::exception& e) {
      std::cerr << "Error : " << e.what();
      REQUIRE(false);
    }
  }
#endif
