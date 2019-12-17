#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

namespace ossia::nodes
{
class dummy_node : public ossia::graph_node
{
public:
  dummy_node()
  {
  }

  void run(const token_request& t, exec_state_facade) noexcept override
  {
  }
};

class dummy_audio_node : public ossia::graph_node
{
public:
  dummy_audio_node()
  {
    m_inlets.push_back(new ossia::audio_inlet);
    m_outlets.push_back(new ossia::audio_outlet );
  }

  void run(const token_request& t, exec_state_facade) noexcept override
  {
    auto i = m_inlets[0]->target<ossia::audio_port>();
    auto o = m_outlets[0]->target<ossia::audio_port>();
    o->samples = i->samples;
  }
};

class dummy_midi_node : public ossia::graph_node
{
public:
  dummy_midi_node()
  {
    m_inlets.push_back(new ossia::midi_inlet);
    m_outlets.push_back(new ossia::midi_outlet);
  }

  void run(const token_request& t, exec_state_facade) noexcept override
  {
    auto i = m_inlets[0]->target<ossia::midi_port>();
    auto o = m_outlets[0]->target<ossia::midi_port>();
    o->messages = i->messages;
  }
};

class dummy_value_node : public ossia::graph_node
{
public:
  dummy_value_node()
  {
    m_inlets.push_back(new ossia::value_inlet);
    m_outlets.push_back(new ossia::value_outlet);
  }

  void run(const token_request& t, exec_state_facade) noexcept override
  {
    auto i = m_inlets[0]->target<ossia::value_port>();
    auto o = m_outlets[0]->target<ossia::value_port>();
    o->set_data(i->get_data());
  }
};

}
