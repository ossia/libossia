#include <ossia/detail/logger.hpp>
#include <ossia/network/dataspace/dataspace_variant_visitors.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/osc/detail/message_generator.hpp>
#include <ossia/network/osc/detail/osc_fwd.hpp>
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/network/oscquery/detail/osc_writer.hpp>
#include <ossia/network/oscquery/detail/outbound_visitor.hpp>

namespace ossia::oscquery
{

std::string
osc_writer::to_message(std::string_view address, const value& v, const unit_t& u)
{
  std::string buffer;
  buffer.resize(1024);

  while(true)
  {
    try
    {
      oscpack::OutboundPacketStream p{buffer.data(), buffer.size()};
      p << oscpack::BeginMessageN(address);
      if(!u)
      {
        v.apply(oscquery::osc_outbound_visitor{p});
      }
      else
      {
        ossia::apply_nonnull(
            [&](const auto& dataspace) {
          ossia::apply(oscquery::osc_outbound_visitor{p}, v.v, dataspace);
            },
            u.v);
      }
      p << oscpack::EndMessage();
      buffer.resize(p.Size());
      break;
    }
    catch(const oscpack::OutOfBufferMemoryException&)
    {
      buffer.resize(buffer.size() * 2);
    }
  }
  return buffer;
}

void osc_writer::write_value(
    std::string_view address, const value& v, const unit_t& u,
    oscpack::UdpTransmitSocket& socket)
{
  auto send_msg = [&](oscpack::OutboundPacketStream& p) {
    p << oscpack::BeginMessageN(address);
    if(!u)
    {
      v.apply(oscquery::osc_outbound_visitor{p});
    }
    else
    {
      ossia::apply_nonnull(
          [&](const auto& dataspace) {
        ossia::apply(oscquery::osc_outbound_visitor{p}, v.v, dataspace);
          },
          u.v);
    }
    p << oscpack::EndMessage();
    socket.Send(p.Data(), p.Size());
  };

  try
  {
    constexpr int BufferSize = 2048;
    alignas(128) std::array<char, BufferSize> buffer;
    oscpack::OutboundPacketStream p{buffer.data(), buffer.size()};
    send_msg(p);
  }
  catch(const oscpack::OutOfBufferMemoryException&)
  {
    while(true)
    {
      std::string buffer;
      buffer.resize(4096);
      try
      {
        oscpack::OutboundPacketStream p{buffer.data(), buffer.size()};
        send_msg(p);
        break;
      }
      catch(...)
      {
        buffer.resize(buffer.size() * 2);
      }
    }
  }
}

std::string osc_writer::to_message(const net::parameter_base& p, const value& v)
{
  return to_message(p.get_node().osc_address(), v, p.get_unit());
}

std::string osc_writer::to_message(const net::full_parameter_data& p, const value& v)
{
  return to_message(p.address, v, p.unit);
}

void osc_writer::send_message(
    const net::parameter_base& p, const value& v, oscpack::UdpTransmitSocket& socket)
{
  write_value(p.get_node().osc_address(), v, p.get_unit(), socket);
}

void osc_writer::send_message(
    const net::full_parameter_data& p, const value& v,
    oscpack::UdpTransmitSocket& socket)
{
  write_value(p.address, v, p.unit, socket);
}

}
