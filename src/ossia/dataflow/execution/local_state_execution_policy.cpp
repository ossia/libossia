#include "local_state_execution_policy.hpp"

#include <ossia/audio/audio_parameter.hpp>
#include <ossia/audio/audio_protocol.hpp>
#include <ossia/dataflow/execution/pull_visitors.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/typed_value.hpp>
#include <ossia/detail/apply.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/common/value_mapping.hpp>
#include <ossia/protocols/midi/detail/midi_impl.hpp>
#include <ossia/protocols/midi/midi_device.hpp>
#include <ossia/protocols/midi/midi_protocol.hpp>

namespace ossia
{
local_state_execution_policy::local_state_execution_policy()
{
  m_valueState.reserve(100);
#if defined(OSSIA_PROTOCOL_AUDIO)
  m_audioState.reserve(8);
#endif

#if defined(OSSIA_PROTOCOL_MIDI)
  m_midiState.reserve(4);
#endif
}

local_state_execution_policy::~local_state_execution_policy() { }

void local_state_execution_policy::commit_common()
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  // FIXME to solve the deeper issue with virtual audio cables,
  // execution should be :
  // 1. read the addresses from all ports that need that into respective processes
  // 2. clear the local state
  // 3. start execution the tick
  // otherwise there's no way to have mixing of data in virtual ports as we cannot clear
  // at the beginning of the tick (won't be able to read from previous tick) or at the
  // end of the tick (the data that was just written will get deleted).

  // FIXME this does not look necessary?
  // Why not just push to the audio address
  for(auto& elt : m_audioState)
  {
    assert(elt.first);
    elt.first->push_value(elt.second);

    for(auto& vec : elt.second.get())
      ossia::audio_buffer_pool::instance().release(std::move(vec));
    elt.second.get().clear();
  }
#endif

#if defined(OSSIA_PROTOCOL_MIDI)
  for(auto& elt : m_midiState)
  {
    if(!elt.second.empty())
    {
      auto proto = dynamic_cast<ossia::net::midi::midi_protocol*>(
          &elt.first->get_node().get_device().get_protocol());
      if(proto)
      {
        for(const auto& v : elt.second)
        {
          proto->push_value(v);
        }
      }
      elt.second.clear();
    }
  }
#endif
}

// Note: this should be combined with the code in the commit_ methods
// in order to handle both domain and unit conversion in a single place...

static void map_value_to_param(
    const ossia::net::parameter_base& param, const ossia::timed_value& v,
    const ossia::value_port& val, int idx,
    value_vector<std::pair<typed_value, int>>& out, ossia_audio_lock_t& lck)
{
  auto& tgt_domain = param.get_domain();
  if(!val.index.empty())
  {
    if(val.type.target<ossia::unit_t>())
    {
      lck.lock();
      out.emplace_back(
          std::piecewise_construct, std::forward_as_tuple(v, val.index),
          std::forward_as_tuple(idx));
      lck.unlock();
    }
    else
    {
      lck.lock();
      out.emplace_back(
          std::piecewise_construct, std::forward_as_tuple(v, val.index, val.type),
          std::forward_as_tuple(idx));
      lck.unlock();
    }

    return;
  }

  if(val.type.target<ossia::unit_t>() || !tgt_domain || !val.domain)
  {
    lck.lock();
    out.emplace_back(
        std::piecewise_construct, std::forward_as_tuple(v), std::forward_as_tuple(idx));
    lck.unlock();
    return;
  }

  {

    lck.lock();
    auto& ret = out.emplace_back(
                       std::piecewise_construct, std::forward_as_tuple(v, val.type),
                       std::forward_as_tuple(idx))
                    .first;

    // FIXME won't work in the merge case for same timestamps, both can write at the same time
    lck.unlock();

    // Map to the target domain
    map_value(ret.value, ossia::destination_index{}, val.domain, tgt_domain);

    // Convert to the parameter type
    ossia::convert_inplace(ret.value, param.get_value_type());
  }
}

static ossia::typed_value map_value_to_param(
    const ossia::net::parameter_base& param, const ossia::timed_value& v,
    const ossia::value_port& val)
{
  ossia::typed_value ret{v, val.index, val.type};
  auto& tgt_domain = param.get_domain();
  if(!val.index.empty())
    goto def_case;

  if(!tgt_domain)
    goto def_case;

  if(!val.domain)
    goto def_case;

  if(val.type.target<ossia::unit_t>())
    goto def_case;

  {
    // Map to the target domain
    map_value(ret.value, ossia::destination_index{}, val.domain, tgt_domain);

    // Convert to the parameter type
    ossia::convert_inplace(ret.value, param.get_value_type());
  }

def_case:
  return ret;
}

static ossia::typed_value map_value_to_param(
    const ossia::net::parameter_base& param, ossia::timed_value&& v,
    const ossia::value_port& val)
{
  auto& tgt_domain = param.get_domain();
  if(!val.index.empty())
    goto def_case;

  if(!tgt_domain)
    goto def_case;

  if(!val.domain)
    goto def_case;

  if(val.type.target<ossia::unit_t>())
    goto def_case;

  {
    // Map to the target domain
    map_value(v.value, ossia::destination_index{}, val.domain, tgt_domain);

    // Convert to the parameter type
    auto vv = ossia::convert(std::move(v.value), param.get_value_type());

    return ossia::typed_value{std::move(vv), val.index, val.type};
  }

def_case:
  return ossia::typed_value{std::move(v), val.index, val.type};
}

void local_state_execution_policy::insert(
    ossia::net::parameter_base& param, const value_port& val)
{
  OSSIA_EXEC_STATE_LOCK_WRITE(*this);
  int idx = m_msgIndex;
  auto& st = m_valueState[&param];
  st.reserve(st.size() + val.get_data().size());
  ossia_write_lock.unlock();

  // here reserve is a pessimization if we push only a few values...
  // just letting log2 growth do its job is much better.
  switch(val.mix_method)
  {
    case ossia::data_mix_method::mix_replace: {
      for(const ossia::timed_value& v : val.get_data())
      {
        auto it = ossia::find_if(st, [&](const std::pair<typed_value, int>& val) {
          return val.first.timestamp == v.timestamp;
        });
        if(it != st.end())
        {
          it->first = map_value_to_param(param, v, val);
        }
        else
        {
          map_value_to_param(param, v, val, idx++, st, ossia_write_lock);
        }
      }
      break;
    }
    case ossia::data_mix_method::mix_append: {
      for(const auto& v : val.get_data())
      {
        map_value_to_param(param, v, val, idx++, st, ossia_write_lock);
      }
      break;
    }
    case ossia::data_mix_method::mix_merge: {
      // TODO;
      break;
    }
  }
  idx = m_msgIndex;
  m_msgIndex += val.get_data().size();
}

void local_state_execution_policy::insert(
    ossia::audio_parameter& param, const audio_port& v)
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  OSSIA_EXEC_STATE_LOCK_WRITE(*this);
  mix(v.get(), m_audioState[&param].get());
#endif
}

void local_state_execution_policy::insert(
    ossia::net::midi::midi_parameter& param, const midi_port& v)
{
#if defined(OSSIA_PROTOCOL_MIDI)
  if(!v.messages.empty())
  {
    OSSIA_EXEC_STATE_LOCK_WRITE(*this);
    auto& vec = m_midiState[&param];
    vec.insert(vec.end(), v.messages.begin(), v.messages.end());
  }
#endif
}

struct state_exec_visitor
{
  ossia::local_state_execution_policy& e;
  void operator()(const ossia::state& st)
  {
    for(auto& msg : st)
      ossia::apply(*this, msg);
  }

  void operator()(const ossia::message& msg)
  {
    OSSIA_EXEC_STATE_LOCK_WRITE(e);
    e.m_valueState[&msg.dest.address()].emplace_back(
        ossia::typed_value{msg.message_value, msg.dest.index, msg.dest.unit},
        e.m_msgIndex++);
  }

  template <std::size_t N>
  void operator()(const ossia::piecewise_vec_message<N>& st)
  {
  }

  void operator()(const ossia::piecewise_message& st) { }

  void operator()(const ossia::monostate&) { }
};

static bool is_in(
    net::parameter_base& other,
    const ossia::hash_map<
        ossia::net::parameter_base*, value_vector<std::pair<typed_value, int>>>&
        container)
{
  auto it = container.find(&other);
  if(it == container.end())
    return false;
  return !it->second.empty();
}
static bool is_in(
    net::parameter_base& other,
    const ossia::hash_map<ossia::net::parameter_base*, value_vector<libremidi::ump>>&
        container)
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  auto it = container.find(&other);
  if(it == container.end())
    return false;
  return !it->second.empty();
#else
  return false;
#endif
}
static bool is_in(
    net::parameter_base& other,
    const ossia::hash_map<ossia::audio_parameter*, audio_port>& container)
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  // TODO dangerous
  auto it = container.find(static_cast<ossia::audio_parameter*>(&other));
  if(it == container.end())
    return false;
  return !it->second.empty();
#else
  return false;
#endif
}

bool local_state_execution_policy::in_local_scope(net::parameter_base& other) const
{
  OSSIA_EXEC_STATE_LOCK_READ(*this);
  return (
      is_in(other, m_valueState) || is_in(other, m_audioState)
      || is_in(other, m_midiState));
}

void local_state_execution_policy::clear_local_state()
{
  m_msgIndex = 0;
  /*
  for(auto& st : m_valueState)
    st.second.clear();
  for(auto& st : m_audioState)
    for(auto& samples : st.second.samples)
      samples.clear();
  for(auto& st : m_midiState)
    st.second.clear();
    */
}

bool local_state_execution_policy::find_and_copy(net::parameter_base& addr, inlet& in)
{
  return in.visit(local_pull_visitor{*this, &addr});
}
}
