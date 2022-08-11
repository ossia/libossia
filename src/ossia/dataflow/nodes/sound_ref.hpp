#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/nodes/sound.hpp>
#include <ossia/dataflow/nodes/sound_sampler.hpp>
#include <ossia/dataflow/nodes/sound_utils.hpp>

namespace ossia::nodes
{
class sound_ref final : public ossia::sound_node
{
public:
  sound_ref() { m_outlets.push_back(&audio_out); }

  ~sound_ref() = default;

  [[nodiscard]] std::string label() const noexcept override { return "sound_ref"; }

  void transport(time_value date) override { m_sampler.transport(date); }

  void set_start(std::size_t v) { m_sampler.set_start(v); }

  void set_upmix(std::size_t v) { m_sampler.set_upmix(v); }

  // Used for testing only
  void set_sound(audio_array data) { m_sampler.set_sound(std::move(data)); }

  void set_sound(const audio_handle& hdl, int channels, int sampleRate)
  {
    m_sampler.set_sound(hdl, channels, sampleRate);
  }

  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    return m_sampler.run(t, e);
  }

  ossia::audio_outlet audio_out;
  sound_sampler m_sampler{this, &audio_out.data};
};
}
