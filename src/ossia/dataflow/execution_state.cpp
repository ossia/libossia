// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "execution_state.hpp"

#include <ossia/audio/audio_protocol.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/dataflow/exec_state_facade.hpp>
#include <ossia/dataflow/execution/default_policy.hpp>
#include <ossia/dataflow/execution/direct_policy.hpp>
#include <ossia/dataflow/execution/execution_policy.hpp>
#include <ossia/dataflow/execution/merged_policy.hpp>
#include <ossia/dataflow/execution/ordered_policy.hpp>
#include <ossia/dataflow/execution/priorized_policy.hpp>
#include <ossia/dataflow/execution/pull_visitors.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/network/base/message_queue.hpp>

#include <libremidi/libremidi.hpp>
namespace ossia
{
execution_state::execution_state() {
  m_policy = std::make_unique<merged_execution_state_policy>();
}

execution_state::~execution_state() { }

void execution_state::set_policy(const tick_setup_options& opt)
{
  switch(opt.commit)
  {
#if !defined(OSSIA_FREESTANDING)
    case tick_setup_options::Default:
      m_policy = std::make_unique<default_execution_state_policy>();
      return;
    case tick_setup_options::Ordered:
      m_policy = std::make_unique<ordered_execution_state_policy>();
      return;
    case tick_setup_options::Priorized:
      m_policy = std::make_unique<priorized_execution_state_policy>();
      return;
#endif
    case tick_setup_options::Merged:
      m_policy = std::make_unique<merged_execution_state_policy>();
      return;
#if !defined(OSSIA_FREESTANDING)
    case tick_setup_options::MergedThreaded:
      m_policy = std::make_unique<threaded_merged_execution_state_policy>();
      return;
    case tick_setup_options::DirectThreaded:
      m_policy = std::make_unique<direct_execution_state_policy>();
      return;
#endif
  }
}

void execution_state::clear_devices()
{
  m_devices_edit.clear();
  m_devices_exec.clear();
}

void execution_state::register_device(net::device_base* d)
{
  if(d)
  {
    m_devices_edit.push_back(d);
    m_device_change_queue.enqueue({device_operation::REGISTER, d});
  }
}

void execution_state::unregister_device(net::device_base* d)
{
  if(d)
  {
    ossia::remove_erase(m_devices_edit, d);
    m_device_change_queue.enqueue({device_operation::UNREGISTER, d});
  }
}

void execution_state::register_parameter(net::parameter_base& p)
{
  auto device = &p.get_node().get_device();
  for(auto& q : m_valueQueues)
  {
    if(&q.device == device)
    {
      q.reg(p);
      break;
    }
  }
}

void execution_state::unregister_parameter(net::parameter_base& p)
{
  auto device = &p.get_node().get_device();
  for(auto& q : m_valueQueues)
  {
    if(&q.device == device)
    {
      q.unreg(p);
      break;
    }
  }
}

void execution_state::register_midi_parameter(net::midi::midi_protocol& p)
{
#if defined(OSSIA_PROTOCOL_MIDI)
  p.enable_registration();
  auto it = m_receivedMidi.find(&p);
  if(it == m_receivedMidi.end())
  {
    m_receivedMidi.insert({&p, received_midi_state{.messages = {}, .count = 1}});
  }
  else
  {
    it->second.count++;
  }
#endif
}

void execution_state::unregister_midi_parameter(net::midi::midi_protocol& p)
{
#if defined(OSSIA_PROTOCOL_MIDI)
  auto it = m_receivedMidi.find(&p);
  if(it != m_receivedMidi.end())
  {
    it->second.count--;
    if(it->second.count <= 0)
    {
      m_receivedMidi.erase(it);
      // TODO p.disable_registration();
    }
  }
#endif
}

void execution_state::get_new_values()
{
  for(auto it = m_receivedValues.begin(), end = m_receivedValues.end(); it != end; ++it)
    it->second.clear();

  for(auto& mq : m_valueQueues)
  {
    ossia::received_value recv;
    while(mq.try_dequeue(recv))
      m_receivedValues[recv.address].push_back(recv.value);
  }

  for(auto it = m_receivedMidi.begin(), end = m_receivedMidi.end(); it != end; ++it)
  {
    auto& [proto, state] = *it;

    auto midi = proto->midi_in();
    auto& input_messages = proto->messages;
    auto& port = state.messages;
    libremidi::ump msg;
    port.clear();
    port.reserve(input_messages.size_approx());
    if(midi->get_current_api() == libremidi::API::JACK_MIDI)
    {
      // FIXME make it sample-accurate for pipewire & jack_ump too
      // sample-accurate, see MIDIDevice.cpp in score
      while(input_messages.try_dequeue(msg))
      {
        port.push_back(msg);
      }
    }
    else
    {
      // Adapt the Absolute timestamps
      const auto cur_t = state.current_buffer_start;
      const auto prev_t = state.last_buffer_start;
      const double timestamp_mult = cur_t > prev_t ? 1. / (cur_t - prev_t) : 0.;

      while(input_messages.try_dequeue(msg))
      {
        msg.timestamp = this->bufferSize * ((msg.timestamp - prev_t) * timestamp_mult);
        if(msg.timestamp >= bufferSize)
          msg.timestamp = this->bufferSize - 1;
        if(msg.timestamp < 0)
          msg.timestamp = 0;

        port.push_back(msg);
      }
    }
  }
}

void execution_state::register_port(const inlet& port)
{
  if(auto vp = port.target<ossia::value_port>())
  {
    if(vp->is_event)
    {
      if(auto addr = port.address.target<ossia::net::parameter_base*>())
      {
        register_parameter(**addr);
      }
      else if(auto p = port.address.target<ossia::traversal::path>())
      {
        std::vector<ossia::net::node_base*> roots{};

        for(auto n : m_devices_exec)
          roots.push_back(&n->get_root_node());

        ossia::traversal::apply(*p, roots);
        for(auto n : roots)
          if(auto param = n->get_parameter())
            register_parameter(*param);
      }
    }
  }
#if defined(OSSIA_PROTOCOL_MIDI)
  else if(port.target<ossia::midi_port>())
  {
    if(auto addr = port.address.target<ossia::net::node_base*>())
    {
      if(auto midi_addr = dynamic_cast<ossia::net::midi::midi_protocol*>(
             &(*addr)->get_device().get_protocol()))
      {
        register_midi_parameter(*midi_addr);
      }
    }
    else if(auto addr = port.address.target<ossia::net::parameter_base*>())
    {
      if(auto midi_addr = dynamic_cast<ossia::net::midi::midi_protocol*>(
             &(*addr)->get_node().get_device().get_protocol()))
      {
        register_midi_parameter(*midi_addr);
      }
    }
  }
#endif
}

void execution_state::register_port(const outlet& port)
{
  // nothing to do
}

void execution_state::unregister_port(const inlet& port)
{
  if(auto vp = port.target<ossia::value_port>())
  {
    if(vp->is_event)
    {
      if(auto addr = port.address.target<ossia::net::parameter_base*>())
      {
        unregister_parameter(**addr);
      }
      else if(auto p = port.address.target<ossia::traversal::path>())
      {
        std::vector<ossia::net::node_base*> roots{};

        for(auto n : m_devices_exec)
          roots.push_back(&n->get_root_node());

        ossia::traversal::apply(*p, roots);
        for(auto n : roots)
          if(auto param = n->get_parameter())
            unregister_parameter(*param);
      }
    }
  }
#if defined(OSSIA_PROTOCOL_MIDI)
  else if(port.target<ossia::midi_port>())
  {
    if(auto addr = port.address.target<ossia::net::node_base*>())
    {
      if(auto midi_addr = dynamic_cast<ossia::net::midi::midi_protocol*>(
             &(*addr)->get_device().get_protocol()))
      {
        unregister_midi_parameter(*midi_addr);
      }
    }
    else if(auto addr = port.address.target<ossia::net::parameter_base*>())
    {
      if(auto midi_addr = dynamic_cast<ossia::net::midi::midi_protocol*>(
             &(*addr)->get_node().get_device().get_protocol()))
      {
        unregister_midi_parameter(*midi_addr);
      }
    }
  }
#endif
}

void execution_state::unregister_port(const outlet& port)
{
  // nothing to do
}

void execution_state::apply_device_changes()
{
  device_operation op;
  ossia::small_vector<device_operation, 8> flattened;
  while(m_device_change_queue.try_dequeue(op))
  {
    flattened.push_back(op);
  }

  if(flattened.empty())
    return;

  // We must first check for devices that were registered and unregistered
  // afterwards. IMPORTANT: here we must *not* use iterators, but indices to
  // prevent invalidation issues. Likewise, since the size of the vec changes,
  // it must be rechecked at each loop iteration !
  for(std::size_t i = 0; i < flattened.size();)
  {
    auto& first = flattened[i];
    if(first.operation == device_operation::REGISTER)
    {
      bool erased = false;
      for(std::size_t j = i + 1; j < flattened.size(); ++j)
      {
        if(flattened[j].operation == device_operation::UNREGISTER
           && flattened[j].device == first.device)
        {
          erased = true;
          flattened.erase(flattened.begin() + j);
          flattened.erase(flattened.begin() + i);

          break;
        }
      }

      if(erased)
      {
        // i does not increase here as the previous element was removed
        continue;
      }
      else
      {
        ++i;
        continue;
      }
    }
    else
    {
      ++i;
      continue;
    }
  }

  for(auto& op : flattened)
  {
    switch(op.operation)
    {
      case device_operation::REGISTER: {
        m_devices_exec.push_back(op.device);
        m_valueQueues.emplace_back(*op.device);
        break;
      }
      case device_operation::UNREGISTER: {
        ossia::remove_erase(m_devices_exec, op.device);
        auto it = ossia::find_if(
            m_valueQueues, [&](auto& mq) { return &mq.device == op.device; });
        if(it != m_valueQueues.end())
          m_valueQueues.erase(it);

        break;
      }
    }
  }
}

void execution_state::init_midi_timings()
{
  for(auto& [proto, state] : m_receivedMidi)
  {
    if(auto m = proto->midi_in())
    {
      state.last_buffer_start = state.current_buffer_start;
      state.current_buffer_start = m->absolute_timestamp();
    }
  }
}

void execution_state::begin_tick()
{
  init_midi_timings();
  m_policy->clear_local_state();
  get_new_values();
  apply_device_changes();
}

void execution_state::reset()
{
  // TODO unregister everything ?
  m_policy->clear_local_state();
  clear_devices();
  m_valueQueues.clear();
  m_receivedValues.clear();
  m_receivedMidi.clear();
}

void execution_state::advance_tick(std::size_t t)
{
#if defined(OSSIA_PROTOCOL_AUDIO)
  /*
        for (auto& elt : st.m_audioState)
        {
          auto addr = dynamic_cast<audio_parameter*>(elt.first);
          if(addr)
          {
            for(auto& chan : addr->audio)
            {
              if(!chan.empty())
              {
                chan = chan.subspan(1);
              }
            }
          }
        }
        */

  for(auto& dev : m_devices_exec)
  {
    auto& proto = dev->get_protocol();
    if(auto ap = dynamic_cast<ossia::audio_protocol*>(&proto))
    {
      ap->advance_tick(t);
    }
  }
#endif
}

void execution_state::find_and_copy(net::parameter_base& addr, inlet& in)
{
  if(!m_policy->find_and_copy(addr, in))
  {
    copy_from_global(addr, in);
  }
}

void execution_state::copy_from_global(net::parameter_base& addr, inlet& in)
{
  if(in.scope & port::scope_t::global)
  {
    in.visit(global_pull_visitor{*this, addr});
  }
}

void execution_state::copy_from_global_node(net::node_base& node, inlet& in)
{
  if(in.scope & port::scope_t::global)
  {
    in.visit(global_pull_node_visitor{*this, node});
  }
}

bool execution_state::in_local_scope(net::parameter_base& other) const
{
  return m_policy->in_local_scope(other);
}

void execution_state::commit()
{
  m_policy->commit();
}
void execution_state::insert(
    ossia::net::parameter_base& dest, const ossia::value_port& v)
{
  m_policy->insert(dest, std::move(v));
}
void execution_state::insert(ossia::audio_parameter& dest, const ossia::audio_port& v)
{
  m_policy->insert(dest, std::move(v));
}
void execution_state::insert(
    ossia::net::midi::midi_parameter& dest, const ossia::midi_port& v)
{
  m_policy->insert(dest, std::move(v));
}

int exec_state_facade::sampleRate() const noexcept
{
  return impl->sampleRate;
}

int exec_state_facade::bufferSize() const noexcept
{
  return impl->bufferSize;
}

double exec_state_facade::modelToSamples() const noexcept
{
  return impl->modelToSamplesRatio;
}

double exec_state_facade::samplesToModel() const noexcept
{
  return impl->samplesToModelRatio;
}

int64_t exec_state_facade::samplesSinceStart() const noexcept
{
  return impl->samples_since_start;
}

double exec_state_facade::startDate() const noexcept
{
  return impl->start_date;
}

double exec_state_facade::currentDate() const noexcept
{
  return impl->cur_date;
}

ossia::net::node_base* exec_state_facade::find_node(std::string_view name) const noexcept
{
  return impl->find_node(name);
}

auto exec_state_facade::timings(const token_request& t) const noexcept -> sample_timings
{
  sample_timings tm;
  if(t.speed > 0.)
  {
    [[likely]];
    tm.start_sample = t.physical_start(impl->modelToSamplesRatio);

    const auto tick_dur = t.physical_write_duration(impl->modelToSamplesRatio);
    auto max_dur = int64_t(impl->bufferSize - tm.start_sample);
    if(max_dur < 0)
      max_dur = 0;

    tm.length = std::min(tick_dur, max_dur);
  }
  else if(t.speed == 0.)
  {
    tm.start_sample = 0;
    tm.length = 0;
    return tm;
  }
  else
  {
    tm.start_sample = -t.physical_start(impl->modelToSamplesRatio);

    const auto tick_dur = -t.physical_write_duration(impl->modelToSamplesRatio);
    auto max_dur = int64_t(impl->bufferSize - tm.start_sample);
    if(max_dur < 0)
      max_dur = 0;

    tm.length = std::min(tick_dur, max_dur);
  }

  if(tm.start_sample < 0)
  {
    [[unlikely]];
    assert(false);
    return {};
  }
  if(tm.start_sample >= impl->bufferSize)
  {
    [[unlikely]];
    assert(false);
    return {};
  }
  if(tm.length < 0)
  {
    [[unlikely]];
    assert(false);
    return {};
  }
  if(tm.start_sample + tm.length > impl->bufferSize)
  {
    [[unlikely]];
    assert(false);
    return {};
  }
  return tm;
}

void exec_state_facade::insert(audio_parameter& dest, const audio_port& v)
{
  impl->insert(dest, v);
}

void exec_state_facade::insert(net::midi::midi_parameter& dest, const midi_port& v)
{
  impl->insert(dest, v);
}

std::vector<ossia::net::node_base*>
exec_state_facade::list_destinations(const destination_t& address)
{
  return ossia::list_destinations(address, impl->exec_devices());
}

ossia::net::node_base*
exec_state_facade::get_first_destination(const destination_t& address)
{
  return ossia::get_first_destination(address, impl->exec_devices());
}

std::vector<ossia::net::node_base*> list_destinations(
    const destination_t& address,
    const ossia::small_vector<ossia::net::device_base*, 4>& devices)
{
  std::vector<ossia::net::node_base*> ret;
  switch(address.which().index())
  {
    // ossia::net::parameter_base*
    case destination_t::index_of<ossia::net::parameter_base*>().index(): {
      auto addr = *address.target<ossia::net::parameter_base*>();
      if(addr)
      {
        ret.push_back(&addr->get_node());
        break;
      }
      break;
    }

      // ossia::traversal::path
    case destination_t::index_of<ossia::traversal::path>().index(): {
      std::vector<ossia::net::node_base*> roots{};

      for(auto n : devices)
        roots.push_back(&n->get_root_node());

      auto& p = *address.target<ossia::traversal::path>();
      ossia::traversal::apply(p, roots);

      for(auto n : roots)
        ret.push_back(n);
      break;
    }

      // ossia::net::node_base*
    case destination_t::index_of<ossia::net::node_base*>().index(): {
      ret.push_back(*address.target<ossia::net::node_base*>());
      break;
    }
    default:
      break;
  }
  return ret;
}

ossia::net::node_base* get_first_destination(
    const destination_t& address,
    const ossia::small_vector<ossia::net::device_base*, 4>& devices)
{
  switch(address.which().index())
  {
    // ossia::net::parameter_base*
    case destination_t::index_of<ossia::net::parameter_base*>().index(): {
      auto addr = *address.target<ossia::net::parameter_base*>();
      if(addr)
        return &addr->get_node();
      break;
    }

      // ossia::traversal::path
    case destination_t::index_of<ossia::traversal::path>().index(): {
      std::vector<ossia::net::node_base*> roots{};

      for(auto n : devices)
        roots.push_back(&n->get_root_node());

      auto& p = *address.target<ossia::traversal::path>();
      ossia::traversal::apply(p, roots);

      for(auto n : roots)
        return n;
      break;
    }

      // ossia::net::node_base*
    case destination_t::index_of<ossia::net::node_base*>().index(): {
      return *address.target<ossia::net::node_base*>();
    }
    default:
      break;
  }
  return nullptr;
}
}
