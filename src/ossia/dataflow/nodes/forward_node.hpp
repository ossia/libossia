#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
namespace ossia::nodes
{
class forward_node : public ossia::nonowning_graph_node
{
public:
  forward_node()
  {
    m_inlets.push_back(&audio_in);
    // m_inlets.push_back(&midi_in);
    m_outlets.push_back(&audio_out);
    // m_outlets.push_back(&midi_out);
  }

  void run(const token_request& t, exec_state_facade) noexcept override
  {
    // OPTIMIZEME : we copy the ins & outs of the token more
    // than necessary
    {
      ossia::audio_port& i = *audio_in;
      ossia::audio_port& o = *audio_out;
      if(!audio_out.has_gain)
      {
        o.samples = i.samples;
      }
      else
      {
        switch(i.samples.size())
        {
          case 0:
            return;

          case 1:
            process_audio_out_mono(i, audio_out);
            break;

          default:
            process_audio_out_general(i, audio_out);
            break;
        }
      }
    }
    // {
    //   auto i = midi_in.target<ossia::midi_port>();
    //   auto o = midi_out.target<ossia::midi_port>();
    //   o->messages = i->messages;
    // }
  }
  ossia::audio_inlet audio_in;
  // ossia::midi_inlet midi_in;
  ossia::audio_outlet audio_out;
  // ossia::midi_outlet midi_out;
};

class interval final : public forward_node
{
public:
  using forward_node::forward_node;
  std::string label() const noexcept override
  {
    return "Interval";
  }
};
class loop final : public forward_node
{
public:
  using forward_node::forward_node;
  std::string label() const noexcept override
  {
    return "Loop";
  }
};
class scenario final : public forward_node
{
public:
  using forward_node::forward_node;
  std::string label() const noexcept override
  {
    return "Scenario";
  }
};
}
