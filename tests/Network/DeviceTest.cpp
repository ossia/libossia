// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <iostream>
#include <ossia/network/local/local.hpp>

#if defined(OSSIA_PROTOCOL_OSC)
#include <ossia/network/osc/osc.hpp>
#endif
#if defined(OSSIA_PROTOCOL_MINUIT)
#include <ossia/network/minuit/minuit.hpp>
#endif
#include <ossia/network/common/parameter_properties.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include "TestUtils.hpp"
#include "ProtocolTestUtils.hpp"

#include <ossia/detail/logger.hpp>
using namespace ossia;

  /*! test life cycle and accessors functions */
TEST_CASE ("test_basic", "test_basic")
  {
    {
      ossia::net::generic_device local_device{"test" };
    }
    {
#if defined(OSSIA_PROTOCOL_OSC)
      ossia::net::generic_device osc_device{
        std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc" };
#endif
    }
    {
#if defined(OSSIA_PROTOCOL_MINUIT)
      ossia::net::generic_device minuit_device{
        std::make_unique<ossia::net::minuit_protocol>("score", "127.0.0.1", 13579, 13580), "test_minuit" };
#endif
    }
  }
