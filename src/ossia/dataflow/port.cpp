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

midi_inlet::~midi_inlet()
{

}

midi_outlet::~midi_outlet()
{

}

}
