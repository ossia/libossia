// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/audio_parameter.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/detail/apply.hpp>

namespace ossia
{

struct local_pull_visitor
{
  execution_state& st;
  destination_t addr;
  bool operator()(value_port& val)
  {
    auto it = st.valueState.find(addr);
    if (it != st.valueState.end())
    {
      copy_data{}(it->second, val);
      return true;
    }
    return false;
  }

  bool operator()(audio_port& val)
  {
    auto it = st.audioState.find(addr);
    if (it != st.audioState.end())
    {
      copy_data{}(it->second, val);
      return true;
    }
    return false;
  }

#if defined(OSSIA_PROTOCOL_MIDI)
  bool operator()(midi_port& val)
  {
    auto it = st.midiState.find(addr);
    if (it != st.midiState.end())
    {
      copy_data{}(it->second, val);
      return true;
    }
    return false;
  }
#endif

  bool operator()()
  {
    return false;
  }
};

// TODO if addresses are missing should they be created automatically ?
// Else how to create a custom address
struct global_pull_visitor
{
  const net::parameter_base& out;
  void operator()(value_port& val)
  {
    val.add_value(out.value());
  }

  void operator()(audio_port& val)
  {
    auto aa = dynamic_cast<const audio_parameter*>(&out);
    assert(aa);
    aa->clone_value(val.samples);
  }

#if defined(OSSIA_PROTOCOL_MIDI)
  void operator()(midi_port& val)
  {
    auto ma = dynamic_cast<const midi_generic_parameter*>(&out);
    assert(ma);
    ma->clone_value(val.messages);
  }
#endif

  void operator()()
  {
  }
};

void execution_state::clear()
{
  valueState.clear();
  audioState.clear();
#if defined(OSSIA_PROTOCOL_MIDI)
  midiState.clear();
#endif
}

void execution_state::commit()
{
  for (auto& elt : valueState)
  {
    apply_to_destination(elt.first, *this, [&] (ossia::net::parameter_base* addr) {
      for(auto& val : elt.second)
        addr->push_value(val.value);
    });
  }

  for (auto& elt : audioState)
  {
    apply_to_destination(elt.first, *this, [&] (ossia::net::parameter_base* base_addr) {
      auto addr = dynamic_cast<audio_parameter*>(base_addr);
      if(addr)
        addr->push_value(elt.second);
    });
  }

#if defined(OSSIA_PROTOCOL_MIDI)
  for (auto& elt : midiState)
  {
    apply_to_destination(elt.first, *this, [&] (ossia::net::parameter_base* base_addr) {
      auto addr = dynamic_cast<midi_generic_parameter*>(base_addr);
      if(addr)
      {
        for (auto v : elt.second.messages)
        {
          addr->push_value(v);
        }
      }
    });
  }
#endif

}

void execution_state::find_and_copy(net::parameter_base& addr, inlet& in)
{
  if (!ossia::apply(local_pull_visitor{*this, &addr}, in.data))
  {
    copy_from_global(addr, in);
  }
}

void execution_state::copy_from_global(net::parameter_base& addr, inlet& in)
{
  if (in.scope & port::scope_t::global)
  {
    ossia::apply(global_pull_visitor{addr}, in.data);
  }
}

void execution_state::insert(const destination_t& dest, data_type v)
{
  using namespace eggs::variants;
  if (auto audio = eggs::variants::get_if<audio_port>(&v))
  {
    insert(dest, std::move(*audio));
  }
#if defined(OSSIA_PROTOCOL_MIDI)
  else if (auto midi = eggs::variants::get_if<midi_port>(&v))
  {
    insert(dest, std::move(*midi));
  }
#endif
  else if (auto val = eggs::variants::get_if<value_port>(&v))
  {
    insert(dest, std::move(*val));
  }

  // thanks mr skeltal:
  // eggs::variants::apply([&] (auto&& val) { this->insert(dest,
  // std::move(val)); }, std::move(v));
}

bool execution_state::in_local_scope(net::parameter_base& other) const
{
  destination_t dest{&other};
#if defined(OSSIA_PROTOCOL_MIDI)
  bool ok = (valueState.find(dest) != valueState.end())
            || (audioState.find(dest) != audioState.end())
            || (midiState.find(dest) != midiState.end());

#else
  bool ok = (valueState.find(dest) != valueState.end())
            || (audioState.find(dest) != audioState.end());
#endif
  if (!ok)
  {
    // TODO check if there is any pattern matching the current destination
  }
  return ok;
}
}
