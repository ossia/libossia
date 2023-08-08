// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/port.hpp>
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

  void operator()(const midi_port& p) const { }

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
    if(auto midi = dynamic_cast<ossia::net::midi::midi_protocol*>(&proto))
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
      [&](ossia::net::node_base* node, bool) { visit(push_data_to_node{*node}); });
}

value_inlet::~value_inlet() = default;

value_outlet::~value_outlet() = default;
audio_inlet::~audio_inlet() = default;

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

  // TODO pan
  switch(data.channels())
  {
    case 0:
      return;

    case 1:
      process_audio_out_mono(*this);
      break;

    default:
      process_audio_out_general(*this);
      break;
  }
}

midi_inlet::~midi_inlet() = default;

midi_outlet::~midi_outlet() = default;

void process_audio_out_mono(ossia::audio_port& i, ossia::audio_outlet& audio_out)
{
  ossia::audio_port& o = *audio_out;
  const double g = audio_out.gain;

  ensure_vector_sizes(i.get(), audio_out.data.get());

  const auto N = i.channel(0).size();
  const auto i_ptr = i.channel(0).data();
  const auto o_ptr = o.channel(0).data();

  for(std::size_t sample = 0; sample < N; sample++)
  {
    o_ptr[sample] = i_ptr[sample] * g;
  }
}

void process_audio_out_general(ossia::audio_port& i, ossia::audio_outlet& audio_out)
{
  const auto C = i.channels();
  ossia::audio_port& o = *audio_out;
  const double g = audio_out.gain;

  while(audio_out.pan.size() < C)
    audio_out.pan.push_back(1.);

  ensure_vector_sizes(i.get(), audio_out.data.get());

  for(auto chan = 0U; chan < C; chan++)
  {
    auto N = i.channel(chan).size();

    auto i_ptr = i.channel(chan).data();
    auto o_ptr = o.channel(chan).data();

    const auto vol = audio_out.pan[chan] * g;
    if(vol == 1.)
    {
      for(std::size_t sample = 0; sample < N; sample++)
      {
        o_ptr[sample] = i_ptr[sample];
      }
    }
    else
    {
      for(std::size_t sample = 0; sample < N; sample++)
      {
        o_ptr[sample] = i_ptr[sample] * vol;
      }
    }
  }
}

void process_audio_out_mono(ossia::audio_outlet& audio_out)
{
  ossia::audio_port& o = *audio_out;

  const double g = audio_out.gain;
  if(g == 1.)
    return;

  const auto N = o.channel(0).size();
  const auto o_ptr = o.channel(0).data();

  for(std::size_t sample = 0; sample < N; sample++)
  {
    o_ptr[sample] *= g;
  }
}

void process_audio_out_general(ossia::audio_outlet& audio_out)
{
  ossia::audio_port& o = *audio_out;
  const auto C = o.channels();
  const double g = audio_out.gain;

  while(audio_out.pan.size() < C)
    audio_out.pan.push_back(1.);

  for(auto chan = 0U; chan < C; chan++)
  {
    auto N = o.channel(chan).size();

    auto o_ptr = o.channel(chan).data();

    const auto vol = audio_out.pan[chan] * g;
    if(vol == 1.)
      continue;
    for(std::size_t sample = 0; sample < N; sample++)
    {
      o_ptr[sample] *= vol;
    }
  }
}

}
