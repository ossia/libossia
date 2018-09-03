// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/editor/state/message.hpp>

#include <iostream>

using namespace ossia;

/*! test life cycle and accessors functions */
TEST_CASE ("test_basic", "test_basic")
{
  // Local device
  ossia::net::generic_device device{"test"};

  // Local tree building
  auto localIntNode = device.create_child("my_int");
  auto localIntAddress = localIntNode->create_parameter(val_type::INT);

  message message{*localIntAddress, 10};

  //! \todo test clone()
}

/*! test execution functions */
TEST_CASE ("test_execution", "test_execution")
{
  ;
}
