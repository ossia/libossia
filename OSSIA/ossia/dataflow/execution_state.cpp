#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/audio_address.hpp>
#include <ossia/dataflow/port.hpp>

namespace ossia
{

struct local_pull_visitor
{
  execution_state& st;
  destination_t addr;
  bool operator()(value_port& val)
  {
    auto it = st.valueState.find(addr);
    if(it != st.valueState.end())
    {
      copy_data{}(it->second, val);
      return true;
    }
    return false;
  }

  bool operator()(audio_port& val)
  {
    auto it = st.audioState.find(addr);
    if(it != st.audioState.end())
    {
      copy_data{}(it->second, val);
      return true;
    }
    return false;
  }

  bool operator()(midi_port& val)
  {
    auto it = st.midiState.find(addr);
    if(it != st.midiState.end())
    {
      copy_data{}(it->second, val);
      return true;
    }
    return false;
  }

  bool operator()() { return false; }
};



// TODO if addresses are missing should they be created automatically ?
// Else how to create a custom address
struct global_pull_visitor
{
  const net::address_base& out;
  void operator()(value_port& val)
  {
    val.data.push_back(out.value());
  }

  void operator()(audio_port& val)
  {
    auto aa = dynamic_cast<const audio_address*>(&out);
    assert(aa);
    val.samples.resize(val.samples.size() + 1);
    aa->clone_value(val.samples.back());
  }

  void operator()(midi_port& val)
  {
    auto ma = dynamic_cast<const midi_generic_address*>(&out);
    assert(ma);
    ma->clone_value(val.messages);
  }

  void operator()()
  {

  }
};


void execution_state::clear()
{
  valueState.clear();
  audioState.clear();
  midiState.clear();
}

void execution_state::commit()
{
  // TODO pattern matching
  for(auto& elt: valueState)
  {
    if(auto addr = elt.first.target<ossia::net::address_base*>())
    {
      for(auto v : elt.second.data)
      {
        (*addr)->push_value(v);
      }
    }
  }

  for(auto& elt : audioState)
  {
    if(auto base_addr = elt.first.target<ossia::net::address_base*>())
    {
      auto addr = dynamic_cast<audio_address*>(*base_addr);
      assert(addr);
      addr->push_value(elt.second);
    }
  }

  for(auto& elt : midiState)
  {
    if(auto base_addr = elt.first.target<ossia::net::address_base*>())
    {
      auto addr = dynamic_cast<midi_generic_address*>(*base_addr);
      assert(addr);

      for(auto v : elt.second.messages)
      {
        addr->push_value(v);
      }
    }

  }
}

void execution_state::find_and_copy(net::address_base& addr, inlet& in)
{
  if(!ossia::apply(local_pull_visitor{*this, &addr}, in.data))
  {
    copy_from_global(addr, in);
  }
}

void execution_state::copy_from_global(net::address_base& addr, inlet& in)
{
  if(in.scope & port::scope_t::global)
  {
    ossia::apply(global_pull_visitor{addr}, in.data);
  }
}

void execution_state::insert(const destination_t& dest, data_type v)
{
  using namespace eggs::variants;
  if(auto audio = eggs::variants::get_if<audio_port>(&v))
  {
    insert(dest, std::move(*audio));
  }
  else if(auto midi = eggs::variants::get_if<midi_port>(&v))
  {
    insert(dest, std::move(*midi));
  }
  else if(auto val = eggs::variants::get_if<value_port>(&v))
  {
    insert(dest, std::move(*val));
  }

  // thanks mr skeltal:
  //eggs::variants::apply([&] (auto&& val) { this->insert(dest, std::move(val)); }, std::move(v));
}

bool execution_state::in_local_scope(net::address_base& other) const
{
  destination_t dest{&other};
  bool ok =
         (valueState.find(dest) != valueState.end())
      || (audioState.find(dest) != audioState.end())
      || (midiState.find(dest) != midiState.end());
  if(!ok)
  {
    // TODO check if there is any pattern matching the current destination
  }
  return ok;
}

}
