#pragma once
#include <ossia/dataflow/graph_node.hpp>

namespace ossia::nodes
{
class forward_node : public ossia::nonowning_graph_node
{
public:

  forward_node()
  {
    m_inlets.push_back(&audio_in);
    m_inlets.push_back(&midi_in);
    m_outlets.push_back(&audio_out);
    m_outlets.push_back(&midi_out);
  }

  void run(token_request t, execution_state&) noexcept override
  {
    {
      auto i = audio_in.data.target<ossia::audio_port>();
      auto o = audio_out.data.target<ossia::audio_port>();
      o->samples = i->samples;
    }
    {
      auto i = midi_in.data.target<ossia::midi_port>();
      auto o = midi_out.data.target<ossia::midi_port>();
      o->messages = i->messages;
    }
  }
  ossia::inlet audio_in{ossia::audio_port{}};
  ossia::inlet midi_in{ossia::midi_port{}};
  ossia::outlet audio_out{ossia::audio_port{}};
  ossia::outlet midi_out{ossia::midi_port{}};
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
