#pragma once
#include <ossia/network/osc/detail/message_generator.hpp>
#include <oscpack/ip/UdpSocket.h>
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <ossia/network/base/address.hpp>
#include <iostream>
#include <memory>
#include <string>
namespace osc
{
/**
 * @brief The sender class
 *
 * Sends OSC packets to a given address on an UDP port.
 *
 */
class sender
{
public:
  sender() = default;
  sender(sender&&) = default;
  sender(const sender&) = delete;
  sender& operator=(const sender&) = default;
  sender& operator=(sender&&) = default;

  sender(const std::string& ip, const int port)
      : m_socket{oscpack::IpEndpointName(ip.c_str(), port)}
      , m_ip(ip)
      , m_port(port)
  {
  }
  template <typename... Args>
  void send(const ossia::net::address_base& address, Args&&... args)
  {
      auto addr = ossia::net::address_string_from_node(address);
      auto begin = addr.find(':') + 1;

      send_impl(
          oscpack::MessageGenerator<>{}(
                      boost::string_view(addr.data() + begin, addr.size() - begin),
                      std::forward<Args>(args)...));
  }

  template <typename... Args>
  void send(const std::string& address, Args&&... args)
  {
    send_impl(
        oscpack::MessageGenerator<>{}(address, std::forward<Args>(args)...));
  }

  template <typename... Args>
  void send(boost::string_view address, Args&&... args)
  {
    send_impl(
        oscpack::MessageGenerator<>{}(address, std::forward<Args>(args)...));
  }

  template <int N, typename... Args>
  void send(oscpack::small_string_base<N> address, Args&&... args)
  {
    send_impl(
        oscpack::MessageGenerator<>{}(address, std::forward<Args>(args)...));
  }

  const std::string& ip() const
  {
    return m_ip;
  }

  int port() const
  {
    return m_port;
  }

private:
  void debug(const oscpack::OutboundPacketStream& out)
  {
    std::string s(out.Data(), out.Data() + out.Size());
    std::replace(s.begin(), s.end(), '\0', ' ');
    std::cerr << s << "\n";
  }
  void send_impl(const oscpack::OutboundPacketStream& m)
  {
    // debug(m);
    m_socket.Send(m.Data(), m.Size());
  }

  oscpack::UdpTransmitSocket m_socket;
  std::string m_ip;
  int m_port;
};
}
