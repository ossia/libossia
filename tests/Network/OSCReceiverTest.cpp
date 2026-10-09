// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/osc/detail/receiver.hpp>

#include <atomic>
#include <chrono>
#include <thread>

/**
 * osc::receiver on port 0 (any free port) reports the port the system gave
 * it, and stops cleanly.
 *
 * It used to keep port() == 0. stop() then woke its thread with a packet sent
 * to 127.0.0.1:0, which macOS refuses to connect to: the receiver detached its
 * thread, freed the socket under it, and the thread called Run() on the null
 * socket.
 */
TEST_CASE("test_osc_receiver_port_zero_reports_bound_port", "test_osc_receiver")
{
  std::atomic_int received{0};
  osc::receiver r{
      0, [&](const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName&) {
    if(std::string_view{m.AddressPattern()} == "/ping")
      received++;
  }};
  REQUIRE(r.port() != 0);

  r.run();
  {
    oscpack::UdpTransmitSocket s{oscpack::IpEndpointName("127.0.0.1", r.port())};
    // "/ping" with an empty type tag string, each padded to 4 bytes.
    const char message[] = {'/', 'p', 'i', 'n', 'g', 0, 0, 0, ',', 0, 0, 0};
    s.Send(message, sizeof(message));
  }
  for(int i = 0; i < 200 && received == 0; i++)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  CHECK(received == 1);
  r.stop();
}

TEST_CASE("test_osc_receiver_port_zero_stops", "test_osc_receiver")
{
  for(int i = 0; i < 20; i++)
  {
    osc::receiver r{
        0, [](const oscpack::ReceivedMessage&, const oscpack::IpEndpointName&) { }};
    r.run();
    r.stop();
  }
  SUCCEED();
}
