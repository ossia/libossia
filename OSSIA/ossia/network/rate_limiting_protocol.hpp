#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/base/message_queue.hpp>
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <chrono>
#include <vector>

namespace ossia::net
{
class OSSIA_EXPORT rate_limiting_protocol final
    : public ossia::net::protocol_base
{
public:
  using clock = std::chrono::high_resolution_clock;
  using duration = clock::duration;
  rate_limiting_protocol() = delete;
  rate_limiting_protocol(const rate_limiting_protocol&) = delete;
  rate_limiting_protocol(rate_limiting_protocol&&) = delete;
  rate_limiting_protocol& operator=(const rate_limiting_protocol&) = delete;
  rate_limiting_protocol& operator=(rate_limiting_protocol&&) = delete;

  rate_limiting_protocol(duration d, std::unique_ptr<protocol_base> arg);

  ~rate_limiting_protocol() override;

  bool pull(ossia::net::parameter_base&) override;
  bool push(const ossia::net::parameter_base& addr, const ossia::value& v) override;
  bool push_raw(const full_parameter_data&) override;
  bool observe(ossia::net::parameter_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;

  void stop() override;
  void set_device(ossia::net::device_base& dev) override;

private:
  void parameter_removed(const ossia::net::parameter_base& b);
  duration m_duration{};
  std::unique_ptr<ossia::net::protocol_base> m_protocol;
  ossia::net::device_base* m_device{};

  std::atomic_bool m_running{true};
  std::thread m_thread;
  moodycamel::ConcurrentQueue<received_value> m_queue;
  moodycamel::ConcurrentQueue<const ossia::net::parameter_base*> m_removedQueue;
  clock::time_point m_lastTime;
  ossia::flat_map<const ossia::net::parameter_base*, std::pair<ossia::value, clock::time_point>> m_messages;
};
}
