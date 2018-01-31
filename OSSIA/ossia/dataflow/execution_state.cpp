// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/audio_parameter.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/detail/apply.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/midi/midi_device.hpp>
#include <ossia/network/midi/midi_parameter.hpp>
#include <ossia/network/midi/midi_protocol.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>
namespace ossia
{

struct local_pull_visitor
{
    execution_state& st;
    ossia::net::parameter_base* addr{};
    bool operator()(value_port& val)
    {
      auto it = st.m_valueState.find(addr);
      if (it != st.m_valueState.end() && !it->second.empty())
      {
        copy_data{}(it->second, val);
        return true;
      }
      return false;
    }

    bool operator()(audio_port& val)
    {
      auto it = st.m_audioState.find(addr);
      if (it != st.m_audioState.end() && !it->second.samples.empty())
      {
        copy_data{}(it->second, val);
        return true;
      }
      return false;
    }

    bool operator()(midi_port& val)
    {
      auto it = st.m_midiState.find(addr);
      if (it != st.m_midiState.end() && !it->second.empty())
      {
        copy_data{}(it->second, val);
        return true;
      }
      return false;
    }

    bool operator()()
    {
      return false;
    }
};

// TODO if addresses are missing should they be created automatically ?
// Else how to create a custom address
struct global_pull_visitor
{
    ossia::execution_state& state;
    const net::parameter_base& out;
    void operator()(value_port& val)
    {
      if(!val.is_event)
      {
        val.add_raw_value(out.value());
      }
      else
      {
        auto it = state.m_receivedValues.find(const_cast<net::parameter_base*>(&out));
        if(it != state.m_receivedValues.end())
        {
          for(auto& v : it->second)
            val.add_raw_value(std::move(v));
        }
      }
    }

    void operator()(audio_port& val)
    {
      auto aa = dynamic_cast<const audio_parameter*>(&out);
      assert(aa);
      aa->clone_value(val.samples);
    }

    void operator()(midi_port& val)
    {
      auto& proto = out.get_node().get_device().get_protocol();
      if(auto midi = dynamic_cast<ossia::net::midi::midi_protocol*>(&proto))
      {
        auto it = state.m_receivedMidi.find(midi);
        if(it != state.m_receivedMidi.end())
        {
          for(auto& v : it->second)
          {
            val.messages.push_back(std::move(v));
          }
        }
      }
    }

    void operator()()
    {
    }
};

void execution_state::clear_devices()
{
  valueDevices.clear();
  allDevices.clear();
  midiDevices.clear();
}

execution_state::execution_state()
{
  m_valueState.reserve(1024);
  m_audioState.reserve(64);
  m_midiState.reserve(64);
}

void execution_state::register_device(net::device_base* d)
{
  valueDevices.push_back(d);
  allDevices.push_back(d);
  m_valueQueues.emplace_back(*d);
}

void execution_state::register_device(net::midi::midi_device* d)
{
  valueDevices.push_back(d);
  allDevices.push_back(d);
  midiDevices.push_back(static_cast<ossia::net::midi::midi_protocol*>(&d->get_protocol()));
  m_valueQueues.emplace_back(*d);
}

void execution_state::register_parameter(net::parameter_base& p)
{
  for(std::size_t i = 0; i < valueDevices.size(); i++)
  {
    if(&p.get_node().get_device() == valueDevices[i])
    {
      auto it = m_valueQueues.begin();
      std::advance(it, i);
      it->reg(p);
      return;
    }
  }
}

void execution_state::unregister_parameter(net::parameter_base& p)
{
  for(std::size_t i = 0; i < valueDevices.size(); i++)
  {
    if(&p.get_node().get_device() == valueDevices[i])
    {
      auto it = m_valueQueues.begin();
      std::advance(it, i);
      it->unreg(p);
      return;
    }
  }
}

void execution_state::register_midi_parameter(net::midi::midi_protocol& p)
{
  p.enable_registration();
  m_receivedMidi.insert({&p, {}});
}

void execution_state::get_new_values()
{
  for(auto it = m_receivedValues.begin(), end = m_receivedValues.end(); it != end; ++it)
    it.value().clear();

  for(auto& mq : m_valueQueues)
  {
    ossia::received_value recv;
    while(mq.try_dequeue(recv))
      m_receivedValues[recv.address].push_back(recv.value);
  }

  for(auto it = m_receivedMidi.begin(), end = m_receivedMidi.end(); it != end; ++it)
  {
    it.value().clear();
    it->first->clone_value(it.value());
  }
}

void execution_state::register_inlet(const inlet& port)
{
  if(auto vp = port.data.target<ossia::value_port>())
  {
    if(vp->is_event)
    {
      if(auto addr = port.address.target<ossia::net::parameter_base*>())
        register_parameter(**addr);
    }
  }
  else if(auto mp = port.data.target<ossia::midi_port>())
  {
    if(auto addr = port.address.target<ossia::net::node_base*>())
    {
      if(auto midi_addr = dynamic_cast<ossia::net::midi::midi_protocol*>(&(*addr)->get_device().get_protocol()))
      {
        register_midi_parameter(*midi_addr);
      }
    }
    else if(auto addr = port.address.target<ossia::net::parameter_base*>())
    {
      if(auto midi_addr = dynamic_cast<ossia::net::midi::midi_protocol*>(&(*addr)->get_node().get_device().get_protocol()))
      {
        register_midi_parameter(*midi_addr);
      }
    }
  }
}

void execution_state::clear_local_state()
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
void execution_state::reset()
{
  // TODO unregister everything ?
  clear_local_state();
  clear_devices();
  m_valueQueues.clear();
  m_receivedValues.clear();
  m_receivedMidi.clear();
}

ossia::message to_state_element(ossia::net::parameter_base& p, ossia::tvalue&& v)
{
  ossia::message m{p, std::move(v.value)};
  if(auto u = v.type.target<ossia::unit_t>())
    m.dest.unit = std::move(*u);
  m.dest.index = std::move(v.index);
  return m;
}

ossia::message to_state_element(ossia::net::parameter_base& p, const ossia::tvalue& v)
{
  ossia::message m{p, v.value};
  if(auto u = v.type.target<ossia::unit_t>())
    m.dest.unit = std::move(*u);
  m.dest.index = std::move(v.index);
  return m;
}

void execution_state::commit_common()
{
  for (auto& elt : m_audioState)
  {
    auto addr = dynamic_cast<audio_parameter*>(elt.first);
    if(addr)
      addr->push_value(elt.second);
    elt.second.samples.clear();
  }

  for (auto& elt : m_midiState)
  {
    if(auto addr = dynamic_cast<ossia::net::midi::midi_parameter*>(elt.first))
    {
      auto& proto = static_cast<ossia::net::midi::midi_protocol&>(addr->get_protocol());

      for (const auto& v : elt.second)
      {
        proto.push_value(v);
      }
    }
    elt.second.clear();
  }
}
void execution_state::commit_merged()
{
  //int i = 0;
  for (auto it = m_valueState.begin(), end = m_valueState.end(); it != end; ++it)
  {
    switch(it->second.size())
    {
      case 0:
        continue;
      case 1:
      {
        to_state_element(*it->first, it->second[0].first).launch();
        break;
      }
      default:
      {
        m_monoState.e = ossia::state_element{};
        state_flatten_visitor<ossia::mono_state, false, true> vis{m_monoState};
        //i += it->second.size();
        for(auto& val : it->second)
        {
          vis(to_state_element(*it->first, std::move(val.first)));
        }
        ossia::launch(m_monoState.e);
      }
    }
    it->second.clear();
  }
  //std::cout << "NUM MESSAGES: " << i << std::endl;

  commit_common();
}

void execution_state::commit()
{
  state_flatten_visitor<ossia::flat_vec_state, false, true> vis{m_commitOrderedState};
  for (auto it = m_valueState.begin(), end = m_valueState.end(); it != end; ++it)
  {
    switch(it->second.size())
    {
      case 0:
        continue;
      case 1:
      {
        to_state_element(*it->first, it->second[0].first).launch();
        break;
      }
      default:
      {
        m_commitOrderedState.clear();
        m_commitOrderedState.reserve(it->second.size());
        for(auto& val : it->second)
        {
          //std::cerr << "mergin : " <<  val.first.value << std::endl;
          vis(to_state_element(*it->first, std::move(val.first)));
        }

        m_commitOrderedState.launch();
      }
    }

    it->second.clear();
  }

  commit_common();
}

void execution_state::commit_ordered()
{
  // TODO same for midi
  //m_flatMessagesCache.reserve(m_valueState.size());
  for (auto it = m_valueState.begin(), end = m_valueState.end(); it != end; ++it)
  {
    m_commitOrderedState.clear();
    m_commitOrderedState.reserve(it->second.size());
    state_flatten_visitor<ossia::flat_vec_state, false, true> vis{m_commitOrderedState};

    int64_t cur_ts = 0; // timestamp
    int cur_ms = 0; // message stamp
    for(auto& val : it->second)
    {
      cur_ms = std::max(cur_ms, val.second);
      cur_ts = std::max(cur_ts, val.first.timestamp);
      vis(to_state_element(*it->first, std::move(val.first)));
    }

    for(auto& e : m_commitOrderedState)
      m_flatMessagesCache[std::make_pair(cur_ts, cur_ms)].push_back(std::move(e));

    it->second.clear();
  }

  for(auto& vec : m_flatMessagesCache)
  {
    for(auto& mess : vec.second)
      ossia::launch(mess);
    vec.second.clear();
  }

  commit_common();
}

void execution_state::find_and_copy(net::parameter_base& addr, inlet& in)
{
  bool ok = ossia::apply(local_pull_visitor{*this, &addr}, in.data);
  if (!ok)
  {
    copy_from_global(addr, in);
  }
}

void execution_state::copy_from_global(net::parameter_base& addr, inlet& in)
{
  if (in.scope & port::scope_t::global)
  {
    ossia::apply(global_pull_visitor{*this, addr}, in.data);
  }
}

void execution_state::insert(ossia::net::parameter_base& param, const data_type& v)
{
  switch(v.which())
  {
    case 0:
    {
      auto audio = static_cast<const ossia::audio_port*>(v.target());
      insert(param, std::move(*audio));
      break;
    }
    case 1:
    {
      auto midi = static_cast<const ossia::midi_port*>(v.target());
      insert(param, std::move(*midi));
      break;
    }
    case 2:
    {
      auto val = static_cast<const ossia::value_port*>(v.target());
      int idx = m_msgIndex;
      auto& st = m_valueState[&param];

      // here reserve is a pessimization if we push only a few values...
      // just letting log2 growth do its job is much better.
      switch(val->mix_method)
      {
        case ossia::data_mix_method::mix_replace:
        {
          for(const auto& v : val->get_data())
          {
            auto it = ossia::find_if(st, [&] (const std::pair<tvalue, int>& val) { return val.first.timestamp == v.timestamp; });
            if(it != st.end())
              it->first = v;
            else
              st.emplace_back(v, idx++);
          }
          break;
        }
        case ossia::data_mix_method::mix_append:
        {
          for(const auto& v : val->get_data())
            st.emplace_back(v, idx++);
          break;
        }
        case ossia::data_mix_method::mix_merge:
        {
          // TODO;
          break;
        }
      }
      idx = m_msgIndex;
      m_msgIndex += val->get_data().size();
      break;
    }
  }
}
void execution_state::insert(ossia::net::parameter_base& param, data_type&& v)
{
  switch(v.which())
  {
    case 0:
    {
      auto audio = static_cast<ossia::audio_port*>(v.target());
      insert(param, std::move(*audio));
      break;
    }
    case 1:
    {
      auto midi = static_cast<ossia::midi_port*>(v.target());
      insert(param, std::move(*midi));
      break;
    }
    case 2:
    {
      auto val = static_cast<ossia::value_port*>(v.target());
      int idx = m_msgIndex;
      auto& st = m_valueState[&param];

      // here reserve is a pessimization if we push only a few values...
      // just letting log2 growth do its job is much better.switch(val->mix_method)

      switch(val->mix_method)
      {
        case ossia::data_mix_method::mix_replace:
        {
          for(auto& v : val->get_data())
          {
            auto it = ossia::find_if(st, [&] (const std::pair<tvalue, int>& val) { return val.first.timestamp == v.timestamp; });
            if(it != st.end())
              it->first = v;
            else
              st.emplace_back(std::move(v), idx++);
          }
          break;
        }
        case ossia::data_mix_method::mix_append:
        {
          for(auto& v : val->get_data())
            st.emplace_back(std::move(v), idx++);
          break;
        }
        case ossia::data_mix_method::mix_merge:
        {
          // TODO;
          break;
        }
      }
      idx = m_msgIndex;
      m_msgIndex += val->get_data().size();
      break;
    }
  }
}

void execution_state::insert(ossia::net::parameter_base& param, const tvalue& v)
{
    m_valueState[&param].emplace_back(v, m_msgIndex++);
}
void execution_state::insert(ossia::net::parameter_base& param, tvalue&& v)
{
  m_valueState[&param].emplace_back(std::move(v), m_msgIndex++);
}

void execution_state::insert(ossia::net::parameter_base& param, const audio_port& v)
{
  mix{}(v.samples, m_audioState[&param].samples, false);
}

void execution_state::insert(ossia::net::parameter_base& param, const midi_port& v)
{
  auto& vec = m_midiState[&param];
  vec.insert(vec.end(), v.messages.begin(), v.messages.end());
}

bool execution_state::in_local_scope(net::parameter_base& other) const
{
  bool ok = (m_valueState.find(&other) != m_valueState.end())
            || (m_audioState.find(&other) != m_audioState.end())
            || (m_midiState.find(&other) != m_midiState.end());
  if (!ok)
  {
    // TODO check if there is any pattern matching the current destination
  }
  return ok;
}
}
