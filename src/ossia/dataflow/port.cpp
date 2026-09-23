// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/telemetry.hpp>
#include <ossia/network/value/destination.hpp>
#include <ossia/protocols/midi/midi_parameter.hpp>
#include <ossia/protocols/midi/midi_protocol.hpp>

namespace ossia
{

namespace
{
void apply_map(ossia::net::node_base& dest, const ossia::value& v)
{
  if(dest.children().empty())
    return;

  const auto& m = *v.target<ossia::value_map_type>();
  for(const auto& [k, child_v] : m)
  {
    if(auto cld = dest.find_child(k))
    {
      if(!cld->children().empty())
      {
        if(child_v.get_type() == ossia::val_type::MAP)
          apply_map(*cld, child_v);
      }
      else if(cld->get_parameter())
      {
        if(child_v.get_type() != ossia::val_type::MAP)
          cld->get_parameter()->push_value(child_v);
      }
    }
  }
}

struct push_data_to_node
{
  ossia::net::node_base& dest;
  void operator()(const value_port& p) const
  {
    // TODO do the unit conversion
    for(auto& val : p.get_data())
    {
      if(val.value.get_type() == ossia::val_type::MAP)
      {
        apply_map(dest, val.value);
      }
    }
  }

  /**
   * A whole device, or one of its channels: what is written to the port goes
   * out of the port the device is on.
   *
   * A port bound to a channel sends on that channel, whatever channel the
   * process authored, so that an inlet and an outlet on the same node are a
   * round trip -- the read side filters by the same number.
   */
  void operator()(const midi_port& p) const
  {
#if defined(OSSIA_PROTOCOL_MIDI)
    auto& proto = dest.get_device().get_protocol();
    auto midi = dynamic_cast<ossia::net::midi::midi_stream*>(&proto);
    if(!midi)
      return;

    const auto chan = midi->stream_channel(dest);
    for(auto& val : p.messages)
    {
      if(!chan)
      {
        midi->push_value(val);
        continue;
      }

      switch(val.get_type())
      {
        using enum libremidi::midi2::message_type;
        case MIDI_1_CHANNEL:
        case MIDI_2_CHANNEL: {
          libremidi::ump m = val;
          m.data[0] = (m.data[0] & ~uint32_t(0x000F0000))
                      | (uint32_t((*chan - 1) & 0x0F) << 16);
          midi->push_value(m);
          break;
        }
        default:
          midi->push_value(val);
          break;
      }
    }
#endif
  }

  void operator()(const audio_port& p) const { }

  [[noreturn]] void operator()(const geometry_port& p) const { assert(false); }
  void operator()() const noexcept { }
};
struct push_data
{
  ossia::net::parameter_base& dest;
  void operator()(const value_port& p) const
  {
    // TODO do the unit conversion
    for(auto& val : p.get_data())
      dest.push_value(val.value);
  }

  void operator()(const midi_port& p) const
  {
#if defined(OSSIA_PROTOCOL_MIDI)
    auto& proto = dest.get_node().get_device().get_protocol();
    if(auto midi = dynamic_cast<ossia::net::midi::midi_stream*>(&proto))
    {
      for(auto& val : p.messages)
        midi->push_value(val);
    }
#endif
  }

  void operator()(const audio_port& p) const
  {
#if defined(OSSIA_PROTOCOL_AUDIO)
    if(auto audio = dynamic_cast<ossia::audio_parameter*>(&dest))
    {
      audio->push_value(p);
    }
#endif
  }

  [[noreturn]] void operator()(const geometry_port& p) const { assert(false); }
  void operator()() const noexcept { }
};
/*
void process_port_values(
    ossia::value_port& source,
    ossia::net::parameter_base& sink)
{
  if(!source.domain)
    return;

  const auto& addr_dom = sink.get_domain();
  if(!addr_dom)
    return;

  for(ossia::timed_value& value : source.get_data())
  {
    process_control_value(value.value, source.domain, addr_dom);
  }
}*/
}

inlet::~inlet() = default;

outlet::~outlet() = default;

void inlet::pre_process() { }

void inlet::post_process() { }

void outlet::pre_process() { }

void outlet::post_process() { }

struct outlet_inserter
{
  ossia::execution_state& e;
  ossia::net::parameter_base* addr;
  void operator()(const ossia::audio_port& data) const noexcept
  {
#if defined(OSSIA_PROTOCOL_AUDIO)
    if(data.empty())
      return;

#if !defined(NDEBUG)
    auto audio_addr = dynamic_cast<ossia::audio_parameter*>(addr);
    assert(audio_addr);
#else
    auto audio_addr = static_cast<ossia::audio_parameter*>(addr);
#endif

    e.insert(*audio_addr, data);
#endif
  }

  void operator()(const ossia::value_port& data) const noexcept
  {
    if(data.get_data().empty())
      return;

    //auto vp = data;
    // TODO process_port_values(vp, *addr);
    e.insert(*addr, data);
  }

  void operator()(const ossia::midi_port& data) const noexcept
  {
#if defined(OSSIA_PROTOCOL_MIDI)
    if(data.messages.empty())
      return;

    if(auto p = dynamic_cast<ossia::net::midi::midi_parameter*>(addr))
      e.insert(*p, data);
#endif
  }

  [[noreturn]] void operator()(const ossia::geometry_port& data) const noexcept
  {
    assert(false);
  }

  void operator()(ossia::value_port&& data) const noexcept = delete;
};

void outlet::write(execution_state& e)
{
  apply_to_destination(
      address, e.exec_devices(),
      [&](ossia::net::parameter_base* addr, bool unique) {
    if(unique)
    {
      // TODO right now with visit this branch is useless - there is never
      // any move
      // TODO we don't really care about moves anyways
      if(scope & port::scope_t::local)
      {
        visit(outlet_inserter{e, addr});
      }
      else if(scope & port::scope_t::global)
      {
        visit(push_data{*addr});
      }
    }
    else
    {
      if(scope & port::scope_t::local)
      {
        visit(outlet_inserter{e, addr});
      }
      else if(scope & port::scope_t::global)
      {
        ((const outlet&)(*this)).visit(push_data{*addr});
      }
    }
      },
      [&](ossia::net::node_base* node, bool) {
    // A node has no parameter to stage the write in, so there is no local
    // branch to take: either it goes out or the port propagates nothing.
    if(scope & (port::scope_t::local | port::scope_t::global))
      visit(push_data_to_node{*node});
      });
}

value_inlet::~value_inlet() = default;

value_outlet::~value_outlet() = default;
audio_inlet::~audio_inlet() = default;

void audio_inlet::pre_process()
{
  if(meter)
    meter->pending.accumulate(data.get());
}

audio_outlet::~audio_outlet() = default;

texture_inlet::~texture_inlet() = default;
texture_outlet::~texture_outlet() = default;

geometry_inlet::~geometry_inlet() = default;
geometry_outlet::~geometry_outlet() = default;

void audio_outlet::post_process()
{
  // TODO do that better in a sample accurate way
  if(auto& gain_msg = std::as_const(gain_inlet).data.get_data(); !gain_msg.empty())
    gain = ossia::convert<float>(gain_msg.back().value);

  // TODO pan inlet
  const std::size_t C = data.channels();
  if(C == 0)
  {
    if(meter)
      meter->pending.accumulate(data.get());
    return;
  }

  const bool ramp = m_applied;
  const double to_gain = gain;
  const double from_gain = ramp ? m_applied_gain : to_gain;
  m_applied = true;
  m_applied_gain = to_gain;

  for(std::size_t c = 0; c < C; c++)
  {
    const double to_pan = (C == 1 || c >= pan.size()) ? 1. : pan[c];
    double from_pan = to_pan;
    if(c < ramped_pan_channels)
    {
      if(ramp)
        from_pan = m_applied_pan[c];
      m_applied_pan[c] = float(to_pan);
    }

    const double from = from_gain * from_pan;
    const double to = to_gain * to_pan;

    auto& chan = data.channel(c);
    const std::size_t N = chan.size();
    double* samples = chan.data();
    if(from == to)
    {
      if(to == 1.)
        continue;
      for(std::size_t i = 0; i < N; i++)
        samples[i] *= to;
    }
    else if(N > 0)
    {
      const double step = (to - from) / double(N);
      for(std::size_t i = 0; i < N; i++)
        samples[i] *= from + step * double(i + 1);
    }
  }

  if(meter)
    meter->pending.accumulate(data.get());
}

midi_inlet::~midi_inlet() = default;

midi_outlet::~midi_outlet() = default;

}
