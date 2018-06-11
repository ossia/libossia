#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/nodes/faust/faust_utils.hpp>

namespace ossia::nodes
{
class faust final : public ossia::graph_node
{
    llvm_dsp* m_dsp{};
  public:
    ossia::small_vector<std::pair<ossia::value_port*, FAUSTFLOAT*>, 8> controls;
    faust(llvm_dsp* dsp):
      m_dsp{dsp}
    {
      m_inlets.push_back(ossia::make_inlet<ossia::audio_port>());
      m_outlets.push_back(ossia::make_outlet<ossia::audio_port>());
      faust_exec_ui<faust> ex{*this};
      buildUserInterfaceCDSPInstance(m_dsp, &ex.glue);
    }

    void run(ossia::token_request tk, ossia::exec_state_facade) noexcept override
    {
      struct dsp_wrap
      {
          llvm_dsp* dsp;
          int getNumInputs() const { return getNumInputsCDSPInstance(dsp); }
          int getNumOutputs() const { return getNumOutputsCDSPInstance(dsp); }
          void compute(int n, FAUSTFLOAT** i, FAUSTFLOAT**o)
          { computeCDSPInstance(dsp, n, i, o); }
      } d{m_dsp};
      faust_exec(*this, d, tk);
    }

    std::string label() const noexcept override
    {
      return "Faust";
    }

    void all_notes_off() noexcept override
    {
    }
};
}
