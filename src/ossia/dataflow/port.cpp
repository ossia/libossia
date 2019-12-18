// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/network/value/destination.hpp>

namespace ossia
{

struct push_data
{
  ossia::net::parameter_base& dest;
  void operator()(const value_port& p) const
  {
    // TODO do the unit conversion
    for (auto& val : p.get_data())
      dest.push_value(val.value);
  }

  void operator()(const midi_port& p) const
  {
    auto& proto = dest.get_node().get_device().get_protocol();
    if (auto midi = dynamic_cast<ossia::net::midi::midi_protocol*>(&proto))
    {
      for (auto& val : p.messages)
        midi->push_value(val);
    }
  }

  void operator()(const audio_port& p) const
  {
    if (auto audio = dynamic_cast<ossia::audio_parameter*>(&dest))
    {
      audio->push_value(p);
    }
  }
  void operator()() const noexcept { }
};

void outlet::write(execution_state& e)
{
  apply_to_destination(
      address, e.exec_devices(),
      [&](ossia::net::parameter_base* addr, bool unique) {
        if (unique)
        {
          if (scope & port::scope_t::local)
          {
            visit([&] (auto& data) { e.insert(*addr, std::move(data)); });
          }
          else if (scope & port::scope_t::global)
          {
            visit(push_data{*addr});
          }
        }
        else
        {
          if (scope & port::scope_t::local)
          {
            visit([&] (const auto& data) { e.insert(*addr, data); });
          }
          else if (scope & port::scope_t::global)
          {
            ((const outlet&)(*this)).visit(push_data{*addr});
          }
        }
  }, do_nothing_for_nodes{});
}

value_inlet::~value_inlet()
{

}

value_outlet::~value_outlet()
{

}
audio_inlet::~audio_inlet()
{

}

audio_outlet::~audio_outlet()
{

}

void audio_outlet::post_process()
{
  // TODO do that better in a sample accurate way
  if(auto& gain_msg = gain_inlet.data.get_data(); !gain_msg.empty())
    gain = ossia::convert<float>(gain_msg.back().value);

  // TODO pan
  switch(data.samples.size())
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

midi_inlet::~midi_inlet()
{

}

midi_outlet::~midi_outlet()
{

}

void process_audio_out_mono(ossia::audio_port& i, ossia::audio_outlet& audio_out)
{
  ossia::audio_port& o = *audio_out;
  const double g = audio_out.gain;

  ensure_vector_sizes(i.samples, audio_out.data.samples);

  const auto N = i.samples[0].size();
  const auto i_ptr = i.samples[0].data();
  const auto o_ptr  = o.samples[0].data();

  for(std::size_t sample = 0; sample < N; sample++)
  {
    o_ptr[sample] = i_ptr[sample] * g;
  }
}

void process_audio_out_general(ossia::audio_port& i, ossia::audio_outlet& audio_out)
{
  const auto C = i.samples.size();
  ossia::audio_port& o = *audio_out;
  const double g = audio_out.gain;

  while(audio_out.pan.size() < C)
    audio_out.pan.push_back(ossia::sqrt_2 / 2.);

  ensure_vector_sizes(i.samples, audio_out.data.samples);

  for(auto chan = 0U; chan < C; chan++)
  {
    auto N = i.samples[chan].size();

    auto i_ptr = i.samples[chan].data();
    auto o_ptr  = o.samples[chan].data();

    const auto vol = audio_out.pan[chan] * g;
    for(std::size_t sample = 0; sample < N; sample++)
    {
      o_ptr[sample] = i_ptr[sample] * vol;
    }
  }
}

void process_audio_out_mono(ossia::audio_outlet& audio_out)
{
  ossia::audio_port& o = *audio_out;

  const double g = audio_out.gain;

  const auto N = o.samples[0].size();
  const auto o_ptr  = o.samples[0].data();

  for(std::size_t sample = 0; sample < N; sample++)
  {
    o_ptr[sample] *= g;
  }
}

void process_audio_out_general(ossia::audio_outlet& audio_out)
{
  ossia::audio_port& o = *audio_out;
  const auto C = o.samples.size();
  const double g = audio_out.gain;

  while(audio_out.pan.size() < C)
    audio_out.pan.push_back(ossia::sqrt_2 / 2.);

  for(auto chan = 0U; chan < C; chan++)
  {
    auto N = o.samples[chan].size();

    auto o_ptr  = o.samples[chan].data();

    const auto vol = audio_out.pan[chan] * g;
    for(std::size_t sample = 0; sample < N; sample++)
    {
      o_ptr[sample] *= vol;
    }
  }
}

}
