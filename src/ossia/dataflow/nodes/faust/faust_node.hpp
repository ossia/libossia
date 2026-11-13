#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/nodes/faust/faust_utils.hpp>
#include <ossia/detail/triple_buffer.hpp>

#include <optional>
#include <vector>

namespace ossia::nodes
{
using faust_port_array
    = ossia::small_vector<std::pair<ossia::value_port*, FAUSTFLOAT*>, 8>;

//! The values of the controls then of the displays of a Faust node, as of its
//! latest tick, for the interface: reading the zones directly would race with
//! the DSP writing them.
struct faust_ui_values
{
  std::optional<ossia::triple_buffer<std::vector<float>>> buffer;

  //! Once the zones are known; every slot gets its size, so that publishing
  //! never allocates.
  void init(const faust_port_array& controls, const faust_port_array& displays)
  {
    buffer.emplace(std::vector<float>(controls.size() + displays.size()));
  }

  void publish(const faust_port_array& controls, const faust_port_array& displays) noexcept
  {
    if(!buffer)
      return;
    auto& w = buffer->write_buffer();
    std::size_t i = 0;
    for(auto& c : controls)
      w[i++] = *c.second;
    for(auto& d : displays)
      w[i++] = *d.second;
    buffer->publish();
  }

  //! The latest values, or nullptr when there are none since the previous call.
  const std::vector<float>* consume() noexcept
  {
    if(!buffer || !buffer->consume())
      return nullptr;
    return &buffer->read_buffer();
  }
};
class faust_mono_fx final : public ossia::graph_node
{
public:
  std::shared_ptr<dsp> m_dsp{};
  faust_port_array controls;
  faust_port_array displays;
  faust_ui_values ui;
  int generation{};

  struct clone
  {
    explicit clone(dsp* f, const clone& orig)
        : fx{f}
    {
      controls = orig.controls;
      displays = orig.displays;

      fx->init(orig.fx->getSampleRate());

      faust_exec_ui_clone<clone> ex{*this};
      fx->buildUserInterface(&ex);
    }

    clone(dsp* f, const faust_port_array& controls, const faust_port_array& disps)
        : fx{f}
        , controls{controls}
        , displays{disps}
    {
    }

    dsp* fx{};
    faust_port_array controls;
    faust_port_array displays;
  };

  void set_control(int i, int v) noexcept
  { set_control(i, (float) v); }
  void set_control(int i, bool v) noexcept
  { set_control(i, v ? 1.f : 0.f); }
  void set_control(int i, const std::string& v) noexcept
  { }
  void set_control(int i, ossia::impulse v) noexcept
  { }
  template<std::size_t N>
  void set_control(int i, const std::array<float, N>& v) noexcept
  {
    *controls[i].second = v[0];
    for(std::size_t c = 0, n = std::min(clones.size(), N); c < n; c++)
    {
      *clones[c].controls[i].second = v[c];
    }
  }
  void set_control(int i, const std::vector<ossia::value>& v) noexcept
  {
    if(v.empty()) return;
    *controls[i].second = ossia::convert<float>(v[0]);
    for(std::size_t c = 0, n = std::min(clones.size(), v.size()); c < n; c++)
    {
      *clones[c].controls[i].second = ossia::convert<float>(v[c]);
    }
  }
  void set_control(int i, const ossia::value_map_type& v) noexcept
  {

  }

  void set_control(int i, float v) noexcept
  {
    *controls[i].second = v;
    for(std::size_t c = 1; c < clones.size(); c++)
    {
      *clones[c].controls[i].second = v;
    }
  }

  std::vector<clone> clones;
  faust_mono_fx(std::shared_ptr<dsp> dsp)
      : m_dsp{std::move(dsp)}
  {
    m_inlets.push_back(new ossia::audio_inlet);
    m_outlets.push_back(new ossia::audio_outlet);

    // Initialize the controls
    faust_exec_ui<faust_mono_fx, false> ex{*this};
    m_dsp->buildUserInterface(&ex);
    ui.init(controls, displays);

    // Preallocate for the most common case, two channels
    clones.emplace_back(m_dsp.get(), controls, displays);
    clones.emplace_back(m_dsp->clone(), clones[0]);
  }

  void run(const ossia::token_request& tk, ossia::exec_state_facade e) noexcept override
  {
    faust_node_utils{}.exec_mono_fx(*this, *m_dsp, tk, e);
    ui.publish(controls, displays);
  }

  [[nodiscard]] std::string label() const noexcept override { return "Faust"; }

  void all_notes_off() noexcept override { }
  void clear() noexcept override
  {
    ossia::graph_node::clear();
    generation = -1;
  }

  ~faust_mono_fx()
  {
    // Clone 0 is the original instance
    for(std::size_t i = 1; i < clones.size(); i++)
      delete clones[i].fx;
  }
};

class faust_fx final : public ossia::graph_node
{
  std::shared_ptr<dsp> m_dsp{};

public:
  faust_port_array controls;
  faust_port_array displays;
  faust_ui_values ui;
  int generation{};
  faust_fx(std::shared_ptr<dsp> dsp)
      : m_dsp{std::move(dsp)}
  {
    m_inlets.push_back(new ossia::audio_inlet);
    m_outlets.push_back(new ossia::audio_outlet);
    faust_exec_ui<faust_fx, false> ex{*this};
    m_dsp->buildUserInterface(&ex);
    ui.init(controls, displays);
  }

  void set_control(int i, float v) noexcept { *controls[i].second = v; }
  void run(const ossia::token_request& tk, ossia::exec_state_facade e) noexcept override
  {
    faust_node_utils{}.exec(*this, *m_dsp, tk, e);
    ui.publish(controls, displays);
  }

  [[nodiscard]] std::string label() const noexcept override { return "Faust"; }

  void all_notes_off() noexcept override { }
  void clear() noexcept override
  {
    ossia::graph_node::clear();
    generation = -1;
  }
};

class faust_synth final : public ossia::graph_node
{
  std::shared_ptr<ossia::nodes::custom_dsp_poly_effect> m_dsp{};

public:
  faust_port_array controls;
  faust_port_array displays;
  faust_ui_values ui;
  int generation{};

  void set_control(int i, float v) noexcept { *controls[i].second = v; }
  std::array<int8_t, 128> in_flight{};
  faust_synth(std::shared_ptr<ossia::nodes::custom_dsp_poly_effect> dsp)
      : m_dsp{std::move(dsp)}
  {
    m_inlets.push_back(new ossia::audio_inlet);
    m_inlets.push_back(new ossia::midi_inlet);
    m_outlets.push_back(new ossia::audio_outlet);
    faust_exec_ui<faust_synth, true> ex{*this};
    m_dsp->buildUserInterface(&ex);
    ui.init(controls, displays);
  }

  void run(const ossia::token_request& tk, ossia::exec_state_facade e) noexcept override
  {
    faust_node_utils{}.exec_synth(*this, *m_dsp, tk, e);
    ui.publish(controls, displays);
  }

  [[nodiscard]] std::string label() const noexcept override { return "Faust Synth"; }

  void all_notes_off() noexcept override
  {
    faust_node_utils{}.all_notes_off(*this, *m_dsp);
  }
  void clear() noexcept override
  {
    ossia::graph_node::clear();
    generation = -1;
  }
};

}
