// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ProtocolTestUtils.hpp"
#include "TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/local/local.hpp>

#include "include_catch.hpp"
#if defined(OSSIA_PROTOCOL_MINUIT)
#include <ossia/network/minuit/minuit.hpp>
#endif

#if defined(OSSIA_PROTOCOL_MINUIT)
TEST_CASE("test_comm_minuit", "test_comm_minuit")
{
  test_comm_generic(
      [] {
    return std::make_unique<ossia::net::minuit_protocol>(
        "score-remote", "127.0.0.1", 13579, 13580);
      },
      [] {
    return std::make_unique<ossia::net::minuit_protocol>(
        "score-remote", "127.0.0.1", 13580, 13579);
  });

  int N = 10;
  auto proto = std::make_unique<ossia::net::multiplex_protocol>();
  auto proto_p = proto.get();
  ossia::net::generic_device local_device{std::move(proto), "score"};

  for(int i = 0; i < N; i++)
  {
    auto cld = local_device.create_child(std::to_string(i));
    cld->create_parameter((ossia::val_type)i);
  }

  proto_p->expose_to(std::make_unique<ossia::net::minuit_protocol>(
      "score-remote", "127.0.0.1", 13579, 13580));

  ossia::net::generic_device remote_device{
      std::make_unique<ossia::net::minuit_protocol>(
          "score-remote", "127.0.0.1", 13580, 13579),
      "score-remote"};
  remote_device.get_protocol().update(remote_device);

  for(auto n : remote_device.children_copy())
  {
    if(auto a = n->get_parameter())
    {
      a->pull_value();
    }
  }
}
#endif
