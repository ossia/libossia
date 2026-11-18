// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "ProtocolTestUtils.hpp"
#include "TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/local/local.hpp>

#include "include_catch.hpp"

#include <chrono>
#if defined(OSSIA_PROTOCOL_MINUIT)
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>
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

#if defined(OSSIA_DNSSD)
// The Minuit listener of ossia-max builds a device for each instance it finds
// under _minuit._tcp. If that device announced itself, the listener would find
// its announcement, "<instance> Minuit server", build a device for it, which
// would announce itself in turn, without end.
TEST_CASE("test_minuit_announce", "test_minuit_announce")
{
  const auto id = std::to_string(
      std::chrono::steady_clock::now().time_since_epoch().count() % 1000000);
  const std::string announced = "minuit-test-announced-" + id;
  const std::string silent = "minuit-test-silent-" + id;

  ossia::net::generic_device announced_device{
      std::make_unique<ossia::net::minuit_protocol>(
          announced, "127.0.0.1", 13581, 13582),
      announced};
  ossia::net::generic_device silent_device{
      std::make_unique<ossia::net::minuit_protocol>(
          silent, "127.0.0.1", 13583, 13584, false),
      silent};

  bool announced_seen = false;
  bool silent_seen = false;
  for(const auto& dev : ossia::net::list_minuit_devices())
  {
    announced_seen = announced_seen || dev.name.rfind(announced, 0) == 0;
    silent_seen = silent_seen || dev.name.rfind(silent, 0) == 0;
  }

  if(!announced_seen)
  {
    WARN("The announced device was not found: no mDNS on this machine");
    return;
  }
  CHECK(!silent_seen);
}
#endif
#endif
