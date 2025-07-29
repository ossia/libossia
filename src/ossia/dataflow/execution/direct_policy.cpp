#include "direct_policy.hpp"

#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
#include <ossia/dataflow/value_port.hpp>
#include <ossia/detail/thread.hpp>
#include <ossia/protocols/midi/midi_parameter.hpp>
#include <ossia/protocols/midi/midi_protocol.hpp>

namespace ossia
{
static thread_local std::vector<ossia::bundle_element> g_direct_policy_message_cache
    = [] {
  std::vector<ossia::bundle_element> vec;
  vec.reserve(4096);
  return vec;
}();

direct_execution_state_policy::direct_execution_state_policy()
    : m_messagesToApply(16000)
{
  const ossia::thread_specs& threads = ossia::get_thread_specs();

  for(int i = 0, N = threads.at(thread_type::Net).num_threads; i < N; i++)
  {
    m_threads[i].thread = std::thread{[this, i] {
      while(!m_startFlag)
        std::this_thread::yield();

      ossia::set_thread_realtime(m_threads[i].thread, 80, 0);
      ossia::set_thread_name(m_threads[i].thread, "ossia net " + std::to_string(i));
      ossia::set_thread_pinned(ossia::thread_type::Net, i);

      process_messages();
    }};
  }

  m_midiThread = std::thread{[this] {
    while(!m_startFlag)
      std::this_thread::yield();

    ossia::set_thread_realtime(m_midiThread, 90, 0);
    ossia::set_thread_name(m_midiThread, "ossia midi");
    ossia::set_thread_pinned(ossia::thread_type::Midi, 0);

    process_midi();
  }};

  m_startFlag = true;
}

void direct_execution_state_policy::process_messages()
{
  // TODO investigate per-protocol queues.. linearization, etc
  // Idea: "direct" flag in value_port comptued to tell if the mesasge can go
  // (e.g; if it has no index, is not critical, and is not used in the local state)
  static constexpr const int message_pack_size = 250;

  ossia::bundle_element m[message_pack_size];
  while(!m_stopFlag)
  {
    int n = 0;
    ossia::net::protocol_base* current_protocol{};
    while((n = m_messagesToApply.wait_dequeue_bulk_timed(m, message_pack_size, 10'000))
          > 0)
    {
      int start_index = 0;
      for(int i = 0; i < n; i++)
      {
        auto& p = m[i].parameter->get_protocol();
        if(&p == current_protocol)
        {
          // Add to bundle
          continue;
        }
        else
        {
          // submit bundle
          p.push_bundle_bounded(std::span(m + start_index, i - start_index));

          start_index = i;
          current_protocol = &p;
        }
      }

      // !!! FIXME make sure that this does not contain the Control Messages
      m[n - 1].parameter->get_protocol().push_bundle_bounded(
          std::span(m + start_index, n - start_index));
    }
  }
}

void direct_execution_state_policy::process_midi()
{
  static constexpr const int message_pack_size = 250;
  ossia::bundle_element m[message_pack_size];
  while(!m_stopFlag)
  {
    int n = 0;
    ossia::net::protocol_base* current_protocol{};
    while((n = m_messagesToApply.wait_dequeue_bulk_timed(m, message_pack_size, 10'000))
          > 0)
    {
      int start_index = 0;
      for(int i = 0; i < n; i++)
      {
        auto& p = m[i].parameter->get_protocol();
        if(&p == current_protocol)
        {
          // Add to bundle
          continue;
        }
        else
        {
          // submit bundle
          p.push_bundle_bounded(std::span(m + start_index, i - start_index));

          start_index = i;
          current_protocol = &p;
        }
      }

      // !!! FIXME make sure that this does not contain the Control Messages
      m[n - 1].parameter->get_protocol().push_bundle_bounded(
          std::span(m + start_index, n - start_index));
    }
  }
}
direct_execution_state_policy::~direct_execution_state_policy()
{
  m_stopFlag = true;

  const ossia::thread_specs& threads = ossia::get_thread_specs();
  for(int i = 0, N = threads.at(thread_type::Net).num_threads; i < N; i++)
  {
    if(m_threads[i].thread.joinable())
      m_threads[i].thread.join();
  }
  if(m_midiThread.joinable())
    m_midiThread.join();
}

void direct_execution_state_policy::commit()
{
  {
    audio_msg m;
    while(m_audioQueue.try_dequeue(m))
    {
      auto [src, dst] = m;
      dst->push_value(*src);
    }
  }

#if OSSIA_MIDI_OUT_DIRECT
  {
    midi_msg m;
    while(m_midiQueue.try_dequeue(m))
    {
      auto [src, dst] = m;

      auto proto = static_cast<ossia::net::midi::midi_protocol*>(&dst->get_protocol());
      for(const auto& v : src->messages)
        proto->push_value(v);
    }
  }
#endif
}

void direct_execution_state_policy::clear_local_state() { }

bool direct_execution_state_policy::find_and_copy(net::parameter_base& addr, inlet& in)
{
  return false;
}
bool direct_execution_state_policy::in_local_scope(net::parameter_base& other) const
{
  return false;
}

void direct_execution_state_policy::insert(
    ossia::net::parameter_base& dest, const ossia::value_port& v)
{
  // FIXME if it's a MIDI param we have to put it in the MIDI queue / thread / etc.
  // e.g. an automation that writes to a MIDI address

  const auto& d = v.get_data();
  const auto N = d.size();
  switch(N)
  {
    case 0: {
      return;
    }
    case 1: {
      if(dest.get_critical())
        m_messagesToApply.enqueue({&dest, d[0].value});
      else
        m_messagesToApply.try_enqueue({&dest, d[0].value});
      break;
    }
    case 2:
    case 3:
    case 4:
    case 5:
      if(dest.get_critical())
        for(auto& val : v.get_data())
          m_messagesToApply.enqueue({&dest, val.value});
      else
        for(auto& val : v.get_data())
          m_messagesToApply.try_enqueue({&dest, val.value});
      break;
    default: {
      auto& cache = g_direct_policy_message_cache;
      cache.clear();
      cache.reserve(v.get_data().size());

      for(auto& val : v.get_data())
      {
        cache.push_back({&dest, val.value});
      }

      if(dest.get_critical())
        m_messagesToApply.enqueue_bulk(cache.data(), cache.size());
      else
        m_messagesToApply.try_enqueue_bulk(cache.data(), cache.size());
    }
  }
}

void direct_execution_state_policy::insert(
    ossia::audio_parameter& dest, const audio_port& v)
{
  if(!v.empty())
    m_audioQueue.enqueue(audio_msg{&v, &dest});
}

void direct_execution_state_policy::insert(
    ossia::net::midi::midi_parameter& dest, const midi_port& v)
{
  if(!v.messages.empty())
    m_midiQueue.enqueue(midi_msg{&v, &dest});
}
}
