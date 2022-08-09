#pragma once
#include <ossia/network/common/network_logger.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>

#include <oscpack/ip/UdpSocket.h>
namespace ossia::oscquery
{
// TODO this export is only needed for tests...
struct OSSIA_EXPORT osc_writer
{
  static std::string to_message(const ossia::net::parameter_base&, const ossia::value&);
  static std::string
  to_message(const ossia::net::full_parameter_data&, const ossia::value&);
  static std::string
  to_message(std::string_view address, const value& v, const unit_t& u);

  static void send_message(
      const ossia::net::parameter_base&, const ossia::value&,
      oscpack::UdpTransmitSocket&);
  static void send_message(
      const ossia::net::full_parameter_data&, const ossia::value&,
      oscpack::UdpTransmitSocket&);

  static void write_value(
      std::string_view address, const value& v, const unit_t& u,
      oscpack::UdpTransmitSocket& socket);
};
}
