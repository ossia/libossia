#pragma once
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/flat_map.hpp>
#include <ossia/network/base/message_queue.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/rate_limiter_configuration.hpp>

#include <readerwriterqueue.h>

#include <chrono>
#include <thread>

namespace ossia::net
{
struct rate_limiter;

template <bool Bundle, bool Repeat, bool SendAll>
struct rate_limiter_impl;
template <bool Bundle, bool Repeat, bool SendAll>
struct rate_limiter_concrete;

class OSSIA_EXPORT rate_limiting_protocol final : public ossia::net::protocol_base
{
public:
  using clock = std::chrono::steady_clock;
  using duration = clock::duration;
  rate_limiting_protocol(
      rate_limiter_configuration conf, std::unique_ptr<protocol_base> arg);
  ~rate_limiting_protocol() override;

  void set_duration(duration d);

private:
  bool pull(ossia::net::parameter_base&) override;
  bool push(const ossia::net::parameter_base& addr, const ossia::value& v) override;
  bool push_raw(const full_parameter_data&) override;
  bool observe(ossia::net::parameter_base&, bool) override;
  bool update(ossia::net::node_base& node_base) override;

  void set_logger(const network_logger& l) override;
  const network_logger& get_logger() const noexcept override;

  void stop() override;
  void set_device(ossia::net::device_base& dev) override;

  void parameter_removed(const ossia::net::parameter_base& b);

  rate_limiting_protocol() = delete;
  rate_limiting_protocol(const rate_limiting_protocol&) = delete;
  rate_limiting_protocol(rate_limiting_protocol&&) = delete;
  rate_limiting_protocol& operator=(const rate_limiting_protocol&) = delete;
  rate_limiting_protocol& operator=(rate_limiting_protocol&&) = delete;

  friend struct rate_limiter;
  template <bool Bundle, bool Repeat, bool SendAll>
  friend struct rate_limiter_impl;
  template <bool Bundle, bool Repeat, bool SendAll>
  friend struct rate_limiter_concrete;

  std::atomic<duration> m_duration{};
  bool m_bundle{};
  bool m_send_all{};
  bool m_repeat{};
  std::unique_ptr<ossia::net::protocol_base> m_protocol;
  ossia::net::device_base* m_device{};

  std::atomic_bool m_running{true};
  std::thread m_thread;

  clock::time_point m_lastTime;
  using map_t = ossia::flat_map<
      const ossia::net::parameter_base*, std::pair<ossia::value, clock::time_point>>;
  map_t m_userMessages;
  map_t m_buffer;
  map_t m_threadMessages;
  std::mutex m_msgMutex;
};

template <typename Protocol, typename... Args>
auto limit_output_rate(std::chrono::milliseconds ms, Args&&... args)
{
  return std::make_unique<rate_limiting_protocol>(
      ms, std::make_unique<Protocol>(std::forward<Args>(args)...));
}

}
