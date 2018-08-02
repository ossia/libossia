// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/network/value/destination.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/port.hpp>


namespace ossia
{

static void copy_to_local(
    const data_type& out, const destination& d, execution_state& in)
{
  in.insert(d, out);
}
static void copy_to_local(
    data_type&& out, const destination& d, execution_state& in)
{
  in.insert(d, std::move(out));
}

struct push_data
{
  const ossia::destination& dest;
  void operator()(const value_port& p) const
  {
    // TODO do the unit conversion
    for(auto& val : p.get_data())
      dest.address().push_value(val.value);
  }

  void operator()(const midi_port& p) const
  {
    auto& proto = dest.address().get_node().get_device().get_protocol();
    if(auto midi = dynamic_cast<ossia::net::midi::midi_protocol*>(&proto))
    {
      for(auto& val : p.messages)
        midi->push_value(val);
    }
  }

  void operator()(const audio_port& p) const
  {
    if(auto audio = dynamic_cast<ossia::audio_parameter*>(&dest.address()))
    {
      audio->push_value(p);
    }
  }
};
static void copy_to_global(
    const data_type& out, const destination& d, execution_state& in)
{
  switch(out.which())
  {
    case 0: push_data{d}(*reinterpret_cast<const ossia::audio_port*>(out.target())); break;
    case 1: push_data{d}(*reinterpret_cast<const ossia::midi_port*>(out.target())); break;
    case 2: push_data{d}(*reinterpret_cast<const ossia::value_port*>(out.target())); break;
    default: break;
  }
}

static void copy_to_global(
    data_type&& out, const destination& d, execution_state& in)
{
  switch(out.which())
  {
    case 0: push_data{d}(std::move(*reinterpret_cast<ossia::audio_port*>(out.target()))); break;
    case 1: push_data{d}(std::move(*reinterpret_cast<ossia::midi_port*>(out.target()))); break;
    case 2: push_data{d}(std::move(*reinterpret_cast<ossia::value_port*>(out.target()))); break;
    default: break;
  }
}

void outlet::write(execution_state& e)
{
  apply_to_destination(address, e.exec_devices(), [&] (ossia::net::parameter_base* addr, bool unique) {
    if(unique)
    {
      if (scope & port::scope_t::local)
      {
        copy_to_local(std::move(data), *addr, e);
      }
      else if (scope & port::scope_t::global)
      {
        copy_to_global(std::move(data), *addr, e);
      }
    }
    else
    {
      if (scope & port::scope_t::local)
      {
        copy_to_local(data, *addr, e);
      }
      else if (scope & port::scope_t::global)
      {
        copy_to_global(data, *addr, e);
      }
    }
  });
}
}
