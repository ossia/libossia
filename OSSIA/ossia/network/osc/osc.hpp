#pragma once

#include <string>

#include <ossia/network/base/protocol.hpp>
#include <hopscotch_map.h>
#include <mutex>
#include <atomic>

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
class OSSIA_EXPORT osc_protocol final : public ossia::net::protocol_base
{
private:
  std::string mIp;
  uint16_t mRemotePort{}; /// the port that a remote device opens
  uint16_t mLocalPort{};  /// the port where a remote device sends OSC messages
                          /// to (opened in this library)
  std::atomic_bool mLearning{};       /// if the device is currently learning from inbound
                          /// messages.
  std::mutex mListeningMutex;
  tsl::hopscotch_map<std::string, ossia::net::address_base*> mListening;

  std::unique_ptr<osc::sender> mSender;
  std::unique_ptr<osc::receiver> mReceiver;

  ossia::net::device_base* mDevice{};

public:
  osc_protocol(std::string ip, uint16_t remote_port, uint16_t local_port);

  osc_protocol(const osc_protocol&) = delete;
  osc_protocol(osc_protocol&&) = delete;
  osc_protocol& operator=(const osc_protocol&) = delete;
  osc_protocol& operator=(osc_protocol&&) = delete;

  ~osc_protocol();

  const std::string& getIp() const;
  osc_protocol& setIp(std::string);

  uint16_t getRemotePort() const;
  osc_protocol& setRemotePort(uint16_t);

  uint16_t getLocalPort() const;
  osc_protocol& setLocalPort(uint16_t);

  bool getLearningStatus() const;
  osc_protocol& setLearningStatus(bool);

  bool update(ossia::net::node_base& node_base) override;

  bool pull(ossia::net::address_base& address_base) override;

  bool push(const ossia::net::address_base& address_base) override;

  bool observe(ossia::net::address_base& address_base, bool enable) override;

private:
  void handleReceivedMessage(
      const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip);
  void handleLearn(const oscpack::ReceivedMessage& m);
  void setDevice(ossia::net::device_base& dev) override;
};
}
}
