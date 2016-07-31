#pragma once

#include <ossia/network/base/protocol.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <future>
#include <set>
#include <string>

#include <ossia/network/minuit/detail/minuit_name_table.hpp>

#include <ossia/editor/value/value.hpp>
#include <mutex>
#include <unordered_map>

namespace ossia
{
namespace net
{
class generic_device;
class OSSIA_EXPORT minuit_protocol final : public ossia::net::protocol_base
{
private:
  std::string mLocalName;
  std::string mIp;
  uint16_t mRemotePort{}; /// the port that a remote device opens
  uint16_t mLocalPort{};  /// the port where a remote device sends OSC messages
                          /// to (opened in this library)

  std::mutex mListeningMutex;
  std::unordered_map<std::string, ossia::net::address_base*> mListening;

  std::promise<void> mNamespacePromise;
  ossia::net::generic_device* mDevice{};

  std::set<std::string, std::less<>> mNamespaceRequests;

public:
  osc::sender sender;
  ossia::minuit::name_table name_table;
  std::promise<void> get_promise;

private:
  osc::receiver mReceiver;

public:
  minuit_protocol(
      const std::string& local_name, const std::string& remote_ip,
      uint16_t remote_port, uint16_t local_port);
  ~minuit_protocol();

  void setDevice(ossia::net::device_base& dev) override;

  const std::string& getIp() const;
  minuit_protocol& setIp(std::string);

  uint16_t getRemotePort() const;
  minuit_protocol& setRemotePort(uint16_t);

  uint16_t getLocalPort() const;
  minuit_protocol& setLocalPort(uint16_t);

  bool update(ossia::net::node_base& node_base) override;

  bool pull(ossia::net::address_base& address_base) override;

  bool push(const ossia::net::address_base& address_base) override;

  bool observe(ossia::net::address_base& address_base, bool enable) override;

  void refresh(boost::string_ref req, const std::string& addr)
  {
    auto it = mNamespaceRequests.find(addr);
    if (it == mNamespaceRequests.end())
    {
      mNamespaceRequests.insert(addr);
      sender.send(req, boost::string_ref{addr});
    }
  }

  void refreshed(boost::string_ref addr)
  {
    auto it = mNamespaceRequests.find(addr);
    if (it != mNamespaceRequests.end())
    {
      mNamespaceRequests.erase(it);
    }

    if (mNamespaceRequests.empty())
    {
      mNamespacePromise.set_value();
    }
  }

private:
  void handleReceivedMessage(
      const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip);
};
}
}
