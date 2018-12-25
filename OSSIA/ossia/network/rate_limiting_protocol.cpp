// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/rate_limiting_protocol.hpp>

namespace ossia::net
{
rate_limiting_protocol::rate_limiting_protocol(
    rate_limiting_protocol::duration d
    , std::unique_ptr<protocol_base> arg)
  : m_duration{d}
  , m_protocol{std::move(arg)}

{
  m_userMessages.container.reserve(4096);
  m_buffer.container.reserve(4096);
  m_threadMessages.container.reserve(4096);
  m_lastTime = clock::now();
  m_thread = std::thread{[this] {
    using namespace std::literals;
    const auto duration = m_duration.load();
    thread_local auto time_to_sleep = duration;
    while(m_running)
    {
      try {
        auto prev_time = clock::now();
        if(time_to_sleep > 1ms)
          std::this_thread::sleep_for(time_to_sleep);

        // TODO find safe way to handle if a parameter is removed
        // TODO instead we should do the value filtering in the parameter ...
        // but still have to handle cases that can be optimized, such as midi
        {
          std::lock_guard lock{m_msgMutex};
          std::swap(m_buffer, m_userMessages);
        }

        // Copy newest messages in local map
        for(auto& msg : m_buffer.container)
        {
          if(msg.second.first.valid())
          {
            m_threadMessages[msg.first] = std::move(msg.second);
            msg.second.first = ossia::value{};
          }
        }

        // Push the actual messages
        for(auto& v : m_threadMessages.container)
        {
          auto val = v.second.first;
          if(val.valid())
          {
            m_protocol->push(*v.first, v.second.first);
          }
        }

        // Clear both containers (while keeping memory allocated for sent messages
        // so that it stays fast)
        for(auto& v : m_buffer.container)
        {
          if(v.second.first.valid())
          {
            v.second.first = ossia::value{};
          }
        }

        for(auto& v : m_threadMessages.container)
        {
          if(v.second.first.valid())
          {
            v.second.first = ossia::value{};
          }
        }
        auto new_time = clock::now();
        auto observed_duration = std::chrono::duration_cast<std::chrono::milliseconds>(new_time - prev_time);
        if(observed_duration > duration)
        {
          if(observed_duration >= 2 * duration)
            time_to_sleep = 0ms;
          else
            time_to_sleep = 2 * duration - observed_duration ;
        }
        else
        {
          time_to_sleep = duration;
        }
      } catch(...) {

      }
    }
  }};

}

rate_limiting_protocol::~rate_limiting_protocol()
{
  m_running = false;
  m_thread.join();
}

void rate_limiting_protocol::set_duration(rate_limiting_protocol::duration d)
{
  m_duration = d;
}

bool rate_limiting_protocol::pull(ossia::net::parameter_base& address)
{
  return m_protocol->pull(address);
}

bool rate_limiting_protocol::push(const ossia::net::parameter_base& address, const ossia::value& v)
{
  std::lock_guard lock{m_msgMutex};
  m_userMessages[&address] = {v, {}};
  return true;
}

bool rate_limiting_protocol::push_raw(const full_parameter_data& address)
{
  return m_protocol->push_raw(address);
}

bool rate_limiting_protocol::observe(
    ossia::net::parameter_base& address, bool enable)
{
  return m_protocol->observe_quietly(address, enable);
}

bool rate_limiting_protocol::update(ossia::net::node_base& node)
{
  return m_protocol->update(node);
}

void rate_limiting_protocol::set_logger(const network_logger& l)
{
  m_protocol->set_logger(l);
}

const network_logger&rate_limiting_protocol::get_logger() const
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
}

}
