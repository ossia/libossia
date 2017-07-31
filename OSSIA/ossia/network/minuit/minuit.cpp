// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/base/address_data.hpp>
#include <ossia/network/osc/detail/osc.hpp>

#include <ossia/detail/string_view.hpp>
#include <ossia/network/minuit/detail/minuit_common.hpp>
#include <ossia/network/minuit/detail/minuit_parser.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <oscpack/osc/OscPrintReceivedElements.h>

namespace ossia
{
namespace net
{

static auto get_time()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

minuit_protocol::minuit_protocol(
    const std::string& local_name, const std::string& remote_ip,
    uint16_t remote_port, uint16_t local_port)
    : m_localName{local_name}
    , m_ip{remote_ip}
    , m_remotePort{remote_port}
    , m_localPort{local_port}
    , m_sender{std::make_unique<osc::sender<osc_outbound_visitor>>(
          m_logger, remote_ip, remote_port)}
    , m_receiver{std::make_unique<osc::receiver>(
          local_port,
          [=](const oscpack::ReceivedMessage& m,
              const oscpack::IpEndpointName& ip) {
            this->on_received_message(m, ip);
          })}
    , m_lastSentMessage{get_time()}
    , m_lastRecvMessage{get_time()}
{
  if (m_receiver->port() != local_port)
  {
    throw ossia::connection_error{
        "minuit_protocol::minuit_protocol: "
        "Could not connect to port: "
        + boost::lexical_cast<std::string>(local_port)};
  }

  name_table.set_device_name(m_localName);
  m_receiver->run();

  update_zeroconf();
}

minuit_protocol::~minuit_protocol()
{
  m_receiver->stop();
}

void minuit_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;
}

const std::string& minuit_protocol::get_ip() const
{
  return m_ip;
}

minuit_protocol& minuit_protocol::set_ip(std::string ip)
{
  m_ip = ip;
  m_sender = std::make_unique<osc::sender<osc_outbound_visitor>>(
      m_logger, m_ip, m_remotePort);

  update_zeroconf();

  return *this;
}

uint16_t minuit_protocol::get_remote_port() const
{
  return m_remotePort;
}

minuit_protocol& minuit_protocol::set_remote_port(uint16_t in_port)
{
  m_remotePort = in_port;
  m_sender = std::make_unique<osc::sender<osc_outbound_visitor>>(
      m_logger, m_ip, m_remotePort);

  update_zeroconf();

  return *this;
}

uint16_t minuit_protocol::get_local_port() const
{
  return m_localPort;
}

minuit_protocol& minuit_protocol::set_local_port(uint16_t out_port)
{
  m_localPort = out_port;
  m_receiver = std::make_unique<osc::receiver>(
      out_port, [=](const oscpack::ReceivedMessage& m,
                    const oscpack::IpEndpointName& ip) {
        this->on_received_message(m, ip);
      });

  update_zeroconf();

  return *this;
}

bool minuit_protocol::update(ossia::net::node_base& node)
{
  // Reset node
  node.clear_children();
  node.remove_address();

  // Send "namespace" request
  m_namespaceFinishedPromise = std::promise<void>{};
  auto fut = m_namespaceFinishedPromise.get_future();

  auto act
      = name_table.get_action(ossia::minuit::minuit_action::NamespaceRequest);
  namespace_refresh(act, ossia::net::osc_address_string(node));

  auto status = fut.wait_for(std::chrono::seconds(5));
  // Won't return as long as the tree exploration request haven't finished.

  // If there are still un-explored nodes, we go for a second round
  if (status != std::future_status::ready)
  {
    auto req = name_table.get_action(minuit::minuit_action::NamespaceRequest);
    lock_type lock(m_nsRequestMutex);
    for (const auto& addr : m_nsRequests)
    {
      m_sender->send(req, ossia::string_view(addr));
    }
    m_lastSentMessage = get_time();
  }
  // While messages are being received regularly, we wait.
  m_lastRecvMessage = get_time();
  auto prev_t = m_lastRecvMessage.load();
  bool ok = false;
  while (!ok)
  {
    status = fut.wait_for(std::chrono::milliseconds(100));
    if (m_lastRecvMessage == prev_t)
      ok = true;
    else
      prev_t = m_lastRecvMessage;
  }

  auto check_unfinished = [&] {
    lock_type lock(m_nsRequestMutex);
    return m_nsRequests.size() > 0 || m_pendingGetRequests > 0
           || status != std::future_status::ready;
  };

  if (check_unfinished())
  {
    std::vector<std::string> nreq, greq;
    for (int i = 0; i < 100; i++)
    {
      status = fut.wait_for(std::chrono::milliseconds(250));
      if (!check_unfinished())
      {
        break;
      }
      else
      {
        {
          lock_type lock(m_nsRequestMutex);
          nreq.clear();
          ossia::copy(m_nsRequests, nreq);
        }
        for (const auto& req : nreq)
        {
          m_sender->send(
              name_table.get_action(
                  ossia::minuit::minuit_action::NamespaceRequest),
              ossia::string_view(req));
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        {
          lock_type lock(m_getRequestMutex);
          greq.clear();
          ossia::copy(m_getRequests, greq);
        }
        for (const auto& req : greq)
        {
          m_sender->send(
              name_table.get_action(ossia::minuit::minuit_action::GetRequest),
              ossia::string_view(req));
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
      }
    }
  }

  m_pendingGetRequests = 0;
  {
    lock_type lock(m_nsRequestMutex);
    for (const auto& node : m_nsRequests)
    {
      logger().error("Namespace request unmatched: {0}", node);
    }
    m_nsRequests.clear();
  }
  {
    lock_type lock(m_getRequestMutex);
    for (const auto& node : m_getRequests)
    {
      logger().error("Namespace request unmatched: {0}", node);
    }
    m_getRequests.clear();
  }

  return status == std::future_status::ready || node.children().size() != 0;
}

void minuit_protocol::request(ossia::net::address_base& address)
{
  auto act = name_table.get_action(ossia::minuit::minuit_action::GetRequest);
  auto addr = ossia::net::osc_address_string(address);
  addr += ":value";
  this->m_sender->send(act, ossia::string_view(addr));
  m_lastSentMessage = get_time();
}

std::future<void> minuit_protocol::pull_async(address_base& address)
{
  // Send "get" request
  m_getFinishedPromise = std::promise<void>();
  auto fut = m_getFinishedPromise.get_future();

  auto act = name_table.get_action(ossia::minuit::minuit_action::GetRequest);
  auto addr = ossia::net::osc_address_string(address);
  addr += ":value";

  get_refresh(act, addr);

  return fut;
}

bool minuit_protocol::pull(ossia::net::address_base& address)
{
  auto fut = pull_async(address);

  fut.wait_for(std::chrono::milliseconds(25));

  return fut.valid();
}

bool minuit_protocol::push_raw(const full_address_data& addr)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = filter_value(addr);
  if (val.valid())
  {
    m_sender->send(addr, val);
    m_lastSentMessage = get_time();
    return true;
  }

  return false;

}

bool minuit_protocol::push(const ossia::net::address_base& addr)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = filter_value(addr);
  if (val.valid())
  {
    m_sender->send(addr, val);
    m_lastSentMessage = get_time();
    return true;
  }

  return false;
}

bool minuit_protocol::observe(ossia::net::address_base& address, bool enable)
{

  auto act
      = name_table.get_action(ossia::minuit::minuit_action::ListenRequest);

  if (enable)
  {
    this->m_sender->send(act, address, "enable");
    m_listening.insert(std::make_pair(osc_address_string(address), &address));
  }
  else
  {
    this->m_sender->send(act, address, "disable");
    m_listening.erase(osc_address_string(address));
  }

  m_lastSentMessage = get_time();

  return true;
}

bool minuit_protocol::observe_quietly(
    ossia::net::address_base& address, bool enable)
{
  if (enable)
    m_listening.insert(std::make_pair(osc_address_string(address), &address));
  else
    m_listening.erase(osc_address_string(address));

  return true;
}

void minuit_protocol::namespace_refresh(
    ossia::string_view req, const std::string& addr)
{
  lock_type lock(m_nsRequestMutex);
  auto it = m_nsRequests.find(addr);
  if (it == m_nsRequests.end())
  {
    m_nsRequests.insert(addr);
    m_sender->send(req, ossia::string_view(addr));
    m_lastSentMessage = get_time();
  }
}

void minuit_protocol::namespace_refreshed(ossia::string_view addr)
{
  lock_type lock(m_nsRequestMutex);
#if defined(__ANDROID_API__)
  auto it = m_nsRequests.find(addr.to_string());
#else
  auto it = m_nsRequests.find(addr);
#endif
  if (it != m_nsRequests.end())
  {
    m_nsRequests.erase(it);
  }

  if (m_nsRequests.empty())
  {
    m_namespaceFinishedPromise.set_value();
  }
}

void minuit_protocol::get_refresh(
    ossia::string_view req, const std::string& addr)
{
  lock_type lock(m_getRequestMutex);
  auto it = ossia::find(m_getRequests, addr);
  if (it == m_getRequests.end())
  {
    m_getRequests.push_back(addr);
    m_pendingGetRequests++;
    m_sender->send(req, ossia::string_view(addr));

    m_lastSentMessage = get_time();
  }
}

void minuit_protocol::get_refreshed(ossia::string_view addr)
{
  lock_type lock(m_getRequestMutex);
  auto it = ossia::find(m_getRequests, addr);
  if (it != m_getRequests.end())
  {
    m_getRequests.erase(it);
    m_pendingGetRequests--;
  }

  if (m_getRequests.empty())
  {
    try
    {
      m_getFinishedPromise.set_value();
    }
    catch (...)
    {
    }
  }
}

osc::sender<osc_outbound_visitor>& minuit_protocol::sender() const
{
  return *m_sender;
}

void minuit_protocol::on_received_message(
    const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
  ossia::string_view address{m.AddressPattern()};

  if (address.size() > 0 && address[0] == '/')
  {
    // Handle the OSC-like case where we receive a plain value.
    auto addr = m_listening.find(std::string(address)); // TODO string_set<>
    if (addr && *addr)
    {
      update_value(**addr, m);
    }
    else
    {
      // We still want to save the value even if it is not listened to.
      if (auto n = find_node(m_device->get_root_node(), address))
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
    if (m_device)
      ossia::minuit::minuit_message_handler::handleMinuitMessage(
          *this, *m_device, address, m);
  }

  if (m_logger.inbound_logger)
    m_logger.inbound_logger->info("In: {0}", m);

  m_lastRecvMessage = get_time();
}

void minuit_protocol::update_zeroconf()
{
  m_zcServer = make_zeroconf_server(
      m_localName + " Minuit server", "_minuit._tcp", m_localName, m_localPort,
      m_remotePort);
}
}
}
