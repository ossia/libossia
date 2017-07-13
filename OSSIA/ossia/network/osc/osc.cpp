// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/logger.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/osc/osc.hpp>
#include <boost/algorithm/string.hpp>
#include <oscpack/osc/OscPrintReceivedElements.h>
namespace ossia
{
namespace net
{
osc_protocol::osc_protocol(
    std::string ip, uint16_t remote_port, uint16_t local_port)
    : m_sender{std::make_unique<osc::sender<osc_outbound_visitor>>(
          m_logger, ip, remote_port)}
    , m_receiver{std::make_unique<osc::receiver>(
          local_port,
          [=](const oscpack::ReceivedMessage& m,
              const oscpack::IpEndpointName& ip) {
            this->on_received_message(m, ip);
          })}
    , m_ip{ip}
    , m_remote_port{remote_port}
    , m_local_port{local_port}
{
  if (m_receiver->port() != local_port)
  {
    ;
    throw ossia::connection_error{
        "osc_protocol::osc_protocol: "
        "Could not connect to port: "
        + boost::lexical_cast<std::string>(local_port)};
  }
  m_receiver->run();
}

osc_protocol::~osc_protocol()
{
}

const std::string& osc_protocol::get_ip() const
{
  return m_ip;
}

osc_protocol& osc_protocol::set_ip(std::string ip)
{
  m_ip = ip;
  m_sender = std::make_unique<osc::sender<osc_outbound_visitor>>(
      m_logger, m_ip, m_remote_port);

  return *this;
}

uint16_t osc_protocol::get_remote_port() const
{
  return m_remote_port;
}

osc_protocol& osc_protocol::set_remote_port(uint16_t in_port)
{
  m_remote_port = in_port;
  m_sender = std::make_unique<osc::sender<osc_outbound_visitor>>(
      m_logger, m_ip, m_remote_port);

  return *this;
}

uint16_t osc_protocol::get_local_port() const
{
  return m_local_port;
}

osc_protocol& osc_protocol::set_local_port(uint16_t out_port)
{
  m_local_port = out_port;
  m_receiver = std::make_unique<osc::receiver>(
      out_port, [=](const oscpack::ReceivedMessage& m,
                    const oscpack::IpEndpointName& ip) {
        this->on_received_message(m, ip);
      });
  return *this;
}

bool osc_protocol::learning() const
{
  return m_learning;
}

osc_protocol& osc_protocol::set_learning(bool newLearn)
{
  m_learning = newLearn;
  return *this;
}

bool osc_protocol::update(ossia::net::node_base& node)
{
  return false;
}

bool osc_protocol::pull(ossia::net::address_base& address)
{
  return false;
}

bool osc_protocol::push(const ossia::net::address_base& addr)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = filter_value(addr);
  if (val.valid())
  {
    m_sender->send(addr, val);
    return true;
  }
  return false;
}

bool osc_protocol::observe(ossia::net::address_base& address, bool enable)
{
  if (enable)
    m_listening.insert(std::make_pair(osc_address_string(address), &address));
  else
    m_listening.erase(osc_address_string(address));

  return true;
}

void osc_protocol::on_received_message(
    const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
  if (!m_learning)
  {
    auto addr_txt = m.AddressPattern();
    auto addr = m_listening.find(addr_txt);
    if (addr && *addr)
    {
      update_value(**addr, m);
    }
    else
    {
      // We still want to save the value even if it is not listened to.
      if (auto n = find_node(m_device->get_root_node(), addr_txt))
      {
        if (auto base_addr = n->get_address())
        {
          update_value_quiet(*base_addr, m);
        }
      }
    }
  }
  else
  {
    on_learn(m);
  }

  if (m_logger.inbound_logger)
    m_logger.inbound_logger->info("In: {0}", m);
}

template <std::size_t N>
static bool is_vec(std::vector<ossia::value>& t)
{
  return t.size() == N && ossia::all_of(t, [](const ossia::value& val) {
           return val.getType() == ossia::val_type::FLOAT;
         });
}

void osc_protocol::on_learn(const oscpack::ReceivedMessage& m)
{
  // TODO put them in a hash map instead.
  // Find-or-add algorithm
  ossia::string_view addr = m.AddressPattern();
  std::vector<std::string> v = address_parts(addr);

  bool is_new = false;
  node_base* n = &m_device->get_root_node();
  for (const auto& part : v)
  {
    auto cld = n->find_child(part);
    if (cld)
    {
      n = cld;
    }
    else
    {
      // Start adding
      n = n->create_child(part);
      is_new = true;
    }
  }

  if (!is_new)
    return;

  // Set-up address
  switch (m.ArgumentCount())
  {
    case 0:
    {
      n->create_address();
      break;
    }
    case 1:
    {
      auto val = osc_utilities::create_value(m.ArgumentsBegin());
      auto addr = n->create_address(val.getType());
      addr->set_value(val);
      break;
    }
    case 2:
    {
      auto val
          = osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount());
      if (is_vec<2>(val))
      {
        auto addr = n->create_address(ossia::val_type::VEC2F);
        addr->set_value(convert<ossia::vec2f>(val));
      }
      else
      {
        auto addr = n->create_address(ossia::val_type::TUPLE);
        addr->set_value(osc_utilities::create_tuple(
            m.ArgumentsBegin(), m.ArgumentCount()));
      }
      break;
    }
    case 3:
    {
      auto val
          = osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount());
      if (is_vec<3>(val))
      {
        auto addr = n->create_address(ossia::val_type::VEC3F);
        addr->set_value(convert<ossia::vec2f>(val));
      }
      else
      {
        auto addr = n->create_address(ossia::val_type::TUPLE);
        addr->set_value(osc_utilities::create_tuple(
            m.ArgumentsBegin(), m.ArgumentCount()));
      }
      break;
    }
    case 4:
    {
      auto val
          = osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount());
      if (is_vec<4>(val))
      {
        auto addr = n->create_address(ossia::val_type::VEC4F);
        addr->set_value(convert<ossia::vec2f>(val));
      }
      else
      {
        auto addr = n->create_address(ossia::val_type::TUPLE);
        addr->set_value(osc_utilities::create_tuple(
            m.ArgumentsBegin(), m.ArgumentCount()));
      }
      break;
    }
    default:
    {
      auto addr = n->create_address(ossia::val_type::TUPLE);
      addr->set_value(
          osc_utilities::create_tuple(m.ArgumentsBegin(), m.ArgumentCount()));
      break;
    }
  }
}

void osc_protocol::set_device(device_base& dev)
{
  m_device = &dev;
}
}
}
