#pragma once
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/domain/domain.hpp>

#include <ossia/network/minuit/detail/minuit_name_table.hpp>

#include <ossia/editor/value/value.hpp>
#include <ossia/detail/mutex.hpp>
#include <future>
#include <set>
#include <string>
#include <hopscotch_map.h>
#include <atomic>
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
  using lock_type = lock_t;
  std::string mLocalName;
  std::string mIp;
  uint16_t mRemotePort{}; /// the port that a remote device opens
  uint16_t mLocalPort{};  /// the port where a remote device sends OSC messages
                          /// to (opened in this library)

  listened_addresses mListening;

  std::promise<void> mNamespaceFinishedPromise;
  ossia::net::device_base* mDevice{};

  mutex_t mNamespaceRequestsMutex;
  std::set<std::string, std::less<>> mNamespaceRequests;

  mutex_t mGetRequestsMutex;
  std::promise<void> mGetFinishedPromise;
  std::vector<std::string> mGetRequests;
  std::atomic_int mPendingGetRequests{};

  std::unique_ptr<osc::sender> mSender;
  std::unique_ptr<osc::receiver> mReceiver;

  zeroconf_server mZeroconfServer;

  std::atomic<long long> mLastSentMessage;
  std::atomic<long long> mLastReceivedMessage;
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
  std::future<void> pullAsync(address_base&) override;
  void request(ossia::net::address_base& address_base) override;

  bool push(const ossia::net::address_base& address_base) override;

  bool observe(ossia::net::address_base& address_base, bool enable) override;
  bool observe_quietly(ossia::net::address_base& address_base, bool enable) override;

  void namespace_refresh(ossia::string_view req, const std::string& addr);
  void namespace_refreshed(ossia::string_view addr);

  void get_refresh(ossia::string_view req, const std::string& addr);
  void get_refreshed(ossia::string_view req);

  osc::sender& sender() const;
  ossia::minuit::name_table name_table;
private:
  void handleReceivedMessage(
      const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip);

  void update_zeroconf();
};
}
}
