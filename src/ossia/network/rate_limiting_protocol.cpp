// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/thread.hpp>
#include <ossia/network/base/bundle.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/rate_limiting_protocol.hpp>
#if defined(__cpp_exceptions)
namespace ossia::net
{
// FIXME refactor with coalescing_queue
// FIXME refactor with sleep_accurate (MIDISync.hpp)
// FIXME refactor with exp wait in audio_spin_mutex

struct rate_limiter
{
  rate_limiting_protocol& self;
  std::chrono::steady_clock::duration duration;
  std::chrono::steady_clock::duration time_to_sleep{duration};

  std::chrono::steady_clock::time_point sleep_before()
  {
    using namespace std::literals;
    using clock = rate_limiting_protocol::clock;
    auto prev_time = clock::now();
    if(time_to_sleep > 1ms)
      std::this_thread::sleep_for(time_to_sleep);
    return prev_time;
  }

  void sleep_after(std::chrono::steady_clock::time_point prev_time)
  {
    using namespace std::literals;
    using clock = rate_limiting_protocol::clock;

    auto new_time = clock::now();
    auto observed_duration
        = std::chrono::duration_cast<std::chrono::milliseconds>(new_time - prev_time);
    if(observed_duration > duration)
    {
      if(observed_duration >= 2 * duration)
        time_to_sleep = 0ms;
      else
        time_to_sleep = 2 * duration - observed_duration;
    }
    else
    {
      time_to_sleep = duration;
    }
  }
};

template <bool Bundle, bool Repeat, bool SendAll>
struct rate_limiter_concrete : rate_limiter
{
  void send()
  {
    // TODO find safe way to handle if a parameter is removed
    // TODO instead we should do the value filtering in the parameter ...
    // but still have to handle cases that can be optimized, such as midi
    if constexpr(SendAll)
    {
      ossia::net::iterate_all_children(
          &this->self.m_device->get_root_node(), [&](ossia::net::parameter_base& ptr) {
        self.m_threadMessages[&ptr].first = ptr.value();
      });
    }
    else
    {
      {
        std::lock_guard lock{self.m_msgMutex};
        std::swap(self.m_buffer, self.m_userMessages);
      }

      for(auto& msg : self.m_buffer)
      {
        if(msg.second.first.valid())
        {
          self.m_threadMessages[msg.first] = std::move(msg.second);
          msg.second.first = ossia::value{};
        }
      }
    }

    // Push the actual messages
    if constexpr(Bundle)
    {
      // FIXME add a push_bundle overload that takes a std::generator
      thread_local std::vector<bundle_element> e;
      e.clear();
      for(auto& v : self.m_threadMessages)
      {
        if(v.second.first.valid())
          e.push_back(bundle_element{
              const_cast<ossia::net::parameter_base*>(v.first), v.second.first});
      }
      if(!e.empty())
        self.m_protocol->push_bundle(e);
    }
    else
    {
      for(auto& v : self.m_threadMessages)
      {
        if(v.second.first.valid())
        {
          self.m_protocol->push(*v.first, v.second.first);
        }
      }
    }

    // Clear both containers (while keeping memory allocated for sent
    // messages so that it stays fast)
    for(auto& v : self.m_buffer)
      if(v.second.first.valid())
        v.second.first = ossia::value{};

    if constexpr(!Repeat)
    {
      for(auto& v : self.m_threadMessages)
        if(v.second.first.valid())
          v.second.first = ossia::value{};
    }
  }

  void operator()()
  {
    ossia::set_thread_name("ossia ratelim");

    while(this->self.m_running)
    {
      try
      {
        const auto prev_time = this->sleep_before();
        this->send();
        this->sleep_after(prev_time);
      }
      catch(...)
      {
      }
    }
  }
};

rate_limiting_protocol::rate_limiting_protocol(
    rate_limiter_configuration d, std::unique_ptr<protocol_base> arg)
    : protocol_base{flags{SupportsMultiplex}}
    , m_duration{d.duration}
    , m_bundle{d.bundle}
    , m_repeat{d.repeat}
    , m_send_all{d.send_all}
    , m_protocol{std::move(arg)}
{
  m_userMessages.reserve(4096);
  m_buffer.reserve(4096);
  m_threadMessages.reserve(4096);
}

rate_limiting_protocol::~rate_limiting_protocol()
{
  m_running = false;
  m_thread.join();
}

void rate_limiting_protocol::set_duration(rate_limiting_protocol::duration d)
{
  m_duration = d;
  // FIXME update thread
}

bool rate_limiting_protocol::pull(ossia::net::parameter_base& address)
{
  return m_protocol->pull(address);
}

bool rate_limiting_protocol::push(
    const ossia::net::parameter_base& address, const ossia::value& v)
{
  if(m_send_all)
    return true;

  std::lock_guard lock{m_msgMutex};
  m_userMessages[&address] = {v, {}};
  return true;
}

bool rate_limiting_protocol::push_raw(const full_parameter_data& address)
{
  // FIXME
  return m_protocol->push_raw(address);
}

bool rate_limiting_protocol::observe(ossia::net::parameter_base& address, bool enable)
{
  return m_protocol->observe(address, enable);
}

bool rate_limiting_protocol::update(ossia::net::node_base& node)
{
  return m_protocol->update(node);
}

void rate_limiting_protocol::set_logger(const network_logger& l)
{
  m_protocol->set_logger(l);
}

const network_logger& rate_limiting_protocol::get_logger() const noexcept
{
  return m_protocol->get_logger();
}

void rate_limiting_protocol::stop()
{
  return m_protocol->stop();
}

void rate_limiting_protocol::set_device(device_base& dev)
{
  m_device = &dev;
  m_protocol->set_device(dev);
  m_lastTime = clock::now();
  uint8_t flags = 0;
  flags |= m_send_all ? 0b001 : 0b000;
  flags |= m_repeat ? 0b010 : 0b000;
  flags |= m_bundle ? 0b100 : 0b000;
  switch(flags)
  {
    case 0b000:
      m_thread = std::thread{rate_limiter_concrete<0, 0, 0>{*this, m_duration}};
      break;
    case 0b001:
      m_thread = std::thread{rate_limiter_concrete<0, 0, 1>{*this, m_duration}};
      break;
    case 0b010:
      m_thread = std::thread{rate_limiter_concrete<0, 1, 0>{*this, m_duration}};
      break;
    case 0b011:
      m_thread = std::thread{rate_limiter_concrete<0, 1, 1>{*this, m_duration}};
      break;
    case 0b100:
      m_thread = std::thread{rate_limiter_concrete<1, 0, 0>{*this, m_duration}};
      break;
    case 0b101:
      m_thread = std::thread{rate_limiter_concrete<1, 0, 1>{*this, m_duration}};
      break;
    case 0b110:
      m_thread = std::thread{rate_limiter_concrete<1, 1, 0>{*this, m_duration}};
      break;
    case 0b111:
      m_thread = std::thread{rate_limiter_concrete<1, 1, 1>{*this, m_duration}};
      break;
  }
}
}
#endif
