#pragma once
#include <ossia/detail/buffer_pool.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/common/network_logger.hpp>
#include <ossia/network/osc/detail/message_generator.hpp>
#include <ossia/network/osc/detail/osc_messages.hpp>
#include <ossia/network/value/format_value.hpp>

#include <oscpack/ip/UdpSocket.h>
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscPrintReceivedElements.h>

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
template <typename ValueWriter>
class sender
{
public:
  sender(const ossia::net::network_logger& l, const std::string& ip, const int port)
      : m_logger{l}
      , m_socket{oscpack::IpEndpointName(ip.c_str(), port)}
      , m_ip(ip)
      , m_port(port)
  {
  }
  template <typename... Args>
  void send(const ossia::net::parameter_base& address, Args&&... args)
  {
    send_base(address.get_node().osc_address(), std::forward<Args>(args)...);
  }
  template <typename... Args>
  void send(const ossia::net::full_parameter_data& address, Args&&... args)
  {
    send_base(address.address, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void send(const std::string& address, Args&&... args)
  {
    send_base(address, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void send(std::string_view address, Args&&... args)
  {
    send_base(address, std::forward<Args>(args)...);
  }

  const std::string& ip() const { return m_ip; }

  int port() const { return m_port; }

  int localPort() const { return m_socket.LocalPort(); }

  oscpack::UdpTransmitSocket& socket() { return m_socket; }

private:
  void debug(const oscpack::OutboundPacketStream& out)
  {
    std::string s(out.Data(), out.Data() + out.Size());
    std::replace(s.begin(), s.end(), '\0', ' ');
    std::cerr << s << "\n";
  }

  template <typename... Args>
  void send_base(Args&&... args)
  {
    auto buf = m_pool.acquire();
    while(buf.size() < ossia::net::max_osc_message_size)
    {
      try
      {
        oscpack::OutboundPacketStream p{buf.data(), buf.size()};
        oscpack::osc_message_generator<ValueWriter> generate_message{p};
        generate_message(std::forward<Args>(args)...);
        send_impl(p);
        break;
      }
      catch(...)
      {
        auto n = buf.size();
        buf.clear();
        buf.resize(n * 2 + 1);
      }
    }

    m_pool.release(std::move(buf));
    if(m_logger.outbound_logger)
    {
      std::string format_string;
      format_string.reserve(5 + 3 * sizeof...(args));
      format_string += "Out: ";
      for(std::size_t i = 0; i < sizeof...(args); i++)
        format_string += "{} ";
      m_logger.outbound_logger->info(fmt::runtime(format_string), args...);
    }
  }

  void send_impl(const oscpack::OutboundPacketStream& m)
  {
    try
    {
      m_socket.Send(m.Data(), m.Size());
    }
    catch(...)
    {
    }
  }

  const ossia::net::network_logger& m_logger;
  oscpack::UdpTransmitSocket m_socket;
  std::string m_ip;
  int m_port;

  ossia::buffer_pool& m_pool = ossia::buffer_pool::instance();
};
}
