// The echo of a device and the critical flag of a parameter are set by the
// host while the network threads of a server read them. This test races
// them: under ThreadSanitizer it reports the plain bools, otherwise it only
// checks that the server keeps working.
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <atomic>
#include <thread>

using namespace ossia;
using namespace ossia::net;

TEST_CASE(
    "test_oscquery_echo_and_critical_threads", "test_oscquery_echo_and_critical_threads")
{
  auto proto = new ossia::oscquery::oscquery_server_protocol{1740, 5740};
  generic_device dev{std::unique_ptr<protocol_base>(proto), "A"};
  auto level = find_or_create_node(dev, "/level").create_parameter(val_type::FLOAT);

  // Two clients: what one writes, the server's network thread applies,
  // reading the echo of the device and the critical flag to forward it
  auto make_client = [](int local_port) {
    auto mirror = new ossia::oscquery::oscquery_mirror_protocol(
        "ws://127.0.0.1:5740", local_port);
    auto d
        = std::make_unique<generic_device>(std::unique_ptr<protocol_base>(mirror), "B");
    mirror->update(d->get_root_node());
    return d;
  };
  auto writer = make_client(11740);
  auto reader = make_client(11741);
  auto node = find_node(writer->get_root_node(), "/level");
  REQUIRE(node);
  auto remote = node->get_parameter();
  REQUIRE(remote);

  std::atomic_bool done{false};
  std::thread host{[&] {
    for(int i = 0; !done; i++)
    {
      dev.set_echo(i % 2 == 0);
      level->set_critical(i % 3 == 0);
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
  }};

  for(int i = 0; i < 200; i++)
  {
    remote->push_value(float(i));
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  done = true;
  host.join();

  REQUIRE(level->value() == ossia::value{199.f});
}
