#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

namespace ossia::nodes
{

struct gain final : public ossia::nonowning_graph_node
{
  double m_gain{0.005};
  ossia::audio_inlet audio_in;
  ossia::value_inlet gain_in;
  ossia::audio_outlet audio_out;

public:
  gain()
  {
    m_inlets.push_back(&audio_in);
    m_inlets.push_back(&gain_in);
    gain_in.target<ossia::value_port>()->type = ossia::decibel_u{};
    m_outlets.push_back(&audio_out);
  }

  void
  run(const ossia::token_request& t, ossia::exec_state_facade st) noexcept override
  {
    auto& vals = gain_in.target<ossia::value_port>()->get_data();
    if (!vals.empty())
      m_gain = ossia::clamp(
          ossia::linear{
              ossia::decibel{ossia::convert<float>(vals.back().value)}}
              .dataspace_value,
          0.f, 1.f);

    auto& in = audio_in->samples;
    auto& out = audio_out->samples;

    const int64_t N = t.physical_write_duration(st.modelToSamples());

    const int64_t first_pos = t.physical_start(st.modelToSamples());
    const int64_t last_pos = first_pos + N;

    const auto channels = in.size();
    out.resize(channels);

    for (std::size_t i = 0; i < channels; i++)
    {
      const int64_t cur_chan_size = in[i].size();

      out[i].resize(st.bufferSize());

      const auto* input = in[i].data();
      auto* output = out[i].data();
      if (cur_chan_size < last_pos)
      {
        for (int64_t j = first_pos; j < cur_chan_size; j++)
          output[j] = m_gain * input[j];

        for (int64_t j = cur_chan_size; j < last_pos; j++)
          output[j] = 0.;
      }
      else
      {
        for (int64_t j = first_pos; j < last_pos; j++)
          out[i][j] = m_gain * input[j];
      }
    }
  }
};
}
