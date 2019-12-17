#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/nodes/faust/faust_utils.hpp>

namespace ossia::nodes
{
class faust_fx final : public ossia::graph_node
{
  dsp* m_dsp{};

public:
  ossia::small_vector<std::pair<ossia::value_port*, FAUSTFLOAT*>, 8> controls;
  faust_fx(llvm_dsp* dsp) : m_dsp{dsp}
  {
    m_inlets.push_back(new ossia::audio_inlet);
    m_outlets.push_back(new ossia::audio_outlet);
    faust_exec_ui<faust_fx> ex{*this};
    m_dsp->buildUserInterface(&ex);
  }

  void run(const ossia::token_request& tk, ossia::exec_state_facade e) noexcept override
  {
    faust_exec(*this, *m_dsp, tk, e);
  }

  std::string label() const noexcept override
  {
    return "Faust";
  }

  void all_notes_off() noexcept override
  {
  }
};


class faust_synth final : public ossia::graph_node
{
  dsp_poly* m_dsp{};

public:
  ossia::small_vector<std::pair<ossia::value_port*, FAUSTFLOAT*>, 8> controls;
  faust_synth(dsp_poly* dsp) : m_dsp{dsp}
  {
    m_inlets.push_back(new ossia::midi_inlet);
    m_outlets.push_back(new ossia::audio_outlet);
    faust_exec_ui<faust_synth> ex{*this};
    m_dsp->buildUserInterface(&ex);
  }

  void run(const ossia::token_request& tk, ossia::exec_state_facade e) noexcept override
  {
    faust_exec_synth(*this, *m_dsp, tk, e);
  }

  std::string label() const noexcept override
  {
    return "Faust Synth";
  }

  void all_notes_off() noexcept override
  {
  }
};

}
