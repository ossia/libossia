#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>

#include <atomic>
#include <thread>

// Validates UDP multicast send + receive end-to-end.
//
// Sender pushes OSC messages to the multicast group; two independent
// receivers both join the same group on loopback and should each see
// every message. If the receive counters match the send counter, the
// host-loopback path, group join, reuse_address (+ SO_REUSEPORT on BSD)
// and IP_MULTICAST_LOOP defaults are all working as intended.

namespace
{
constexpr const char* multicast_group = "239.255.7.42"; // admin-scoped block
constexpr uint16_t multicast_port = 14242;
constexpr int messages_to_send = 20;

std::atomic<int> received_a{};
std::atomic<int> received_b{};
std::atomic_bool senders_done{};

void receiver_main(std::atomic<int>& counter, const char* tag)
{
  auto ctx = std::make_shared<ossia::net::network_context>();

  using conf = ossia::net::osc_protocol_configuration;
  conf host_config{
      .mode = conf::HOST,
      .version = conf::OSC1_1,
      .transport = ossia::net::udp_configuration{
          {.local = ossia::net::inbound_socket_configuration{
               .bind = "0.0.0.0",
               .port = multicast_port,
               .multicast_group = multicast_group,
               .multicast_interface = "0.0.0.0"},
           .remote = std::nullopt}}};

  ossia::net::generic_device device{
      ossia::net::make_osc_protocol(ctx, host_config), "P"};

  auto cb = [&counter, tag](ossia::string_view v, const ossia::value& val) {
    counter.fetch_add(1, std::memory_order_relaxed);
    ossia::logger().info("[{}] {} => {}", tag, v, val);
  };
  device.on_unhandled_message.connect(&cb);

  // Run until sender signals done, plus a small grace period for in-flight pkts.
  while(!senders_done.load(std::memory_order_acquire))
    ctx->context.run_for(std::chrono::milliseconds(100));
  ctx->context.run_for(std::chrono::milliseconds(500));
}

void sender_main()
{
  auto ctx = std::make_shared<ossia::net::network_context>();

  using conf = ossia::net::osc_protocol_configuration;
  conf send_config{
      .mode = conf::MIRROR,
      .version = conf::OSC1_1,
      .transport = ossia::net::udp_configuration{
          {.local = std::nullopt,
           .remote = ossia::net::outbound_socket_configuration{
               .host = multicast_group,
               .port = multicast_port,
               .broadcast = false,
               // TTL=1 keeps the test on the local host; bump for routed nets.
               .multicast_ttl = 1,
               .multicast_interface = "0.0.0.0",
               // Must stay true: receivers and sender share the host.
               .multicast_loopback = true}}}};

  ossia::net::generic_device device{
      ossia::net::make_osc_protocol(ctx, send_config), "S"};

  // Give the two receivers time to bind + join_group before we start sending.
  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  for(int i = 0; i < messages_to_send; i++)
  {
    device.get_protocol().push_raw({"/mcast", i});
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  // Drain the io_context so the last sends actually leave the socket.
  ctx->context.run_for(std::chrono::milliseconds(200));
  senders_done = true;
}
}

int main()
{
  std::thread rx_a{receiver_main, std::ref(received_a), "A"};
  std::thread rx_b{receiver_main, std::ref(received_b), "B"};

  sender_main();

  rx_a.join();
  rx_b.join();

  const int a = received_a.load();
  const int b = received_b.load();
  ossia::logger().info(
      "multicast test: sent={} received_A={} received_B={}", messages_to_send, a, b);

  // Each receiver should see every message. Allow zero loss tolerance on
  // loopback — if any packet went missing, multicast plumbing is wrong.
  if(a != messages_to_send || b != messages_to_send)
  {
    ossia::logger().error("multicast test FAILED");
    return 1;
  }
  ossia::logger().info("multicast test OK");
  return 0;
}
