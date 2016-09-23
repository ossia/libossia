#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/domain/domain.hpp>

#include <ossia/network/minuit/detail/minuit_name_table.hpp>

#include <ossia/editor/value/value.hpp>
#include <mutex>
#include <future>
#include <set>
#include <string>
#include <unordered_map>
#include <ossia/network/minuit/minuit_zeroconf.hpp>

namespace oscpack
{
class ReceivedMessage;
class IpEndpointName;
}
namespace osc
{
class sender;
class receiver;
}

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

  std::unique_ptr<osc::sender> mSender;
  std::unique_ptr<osc::receiver> mReceiver;

  zeroconf_server mZeroconfServer;

public:
  minuit_protocol(
      const std::string& local_name, const std::string& remote_ip,
      uint16_t remote_port, uint16_t local_port);

  minuit_protocol(const minuit_protocol&) = delete;
  minuit_protocol(minuit_protocol&&) = delete;
  minuit_protocol& operator=(const minuit_protocol&) = delete;
  minuit_protocol& operator=(minuit_protocol&&) = delete;

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
  bool observe_quietly(ossia::net::address_base& address_base, bool enable) override;

  void refresh(boost::string_ref req, const std::string& addr);

  void refreshed(boost::string_ref addr);

  osc::sender& sender() const;
  ossia::minuit::name_table name_table;
  std::promise<void> get_promise;
  std::atomic_int pending_get_requests{};

private:
  void handleReceivedMessage(
      const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip);

  void update_zeroconf();
};
}
}
