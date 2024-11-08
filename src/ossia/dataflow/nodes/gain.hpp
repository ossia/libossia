#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

namespace ossia::nodes
{

struct gain_node final : public ossia::nonowning_graph_node
{
public:
  double gain{0.005};

  gain_node()
  {
    m_inlets.push_back(&audio_in);
    m_inlets.push_back(&gain_in);
    gain_in.target<ossia::value_port>()->type = ossia::decibel_u{};
    m_outlets.push_back(&audio_out);
  }

  std::string label() const noexcept override { return "gain"; }
  void run(const ossia::token_request& t, ossia::exec_state_facade st) noexcept override
  {
    auto& vals = gain_in.target<ossia::value_port>()->get_data();
    if(!vals.empty())
      gain = ossia::clamp(
          ossia::linear{ossia::decibel{ossia::convert<float>(vals.back().value)}}
              .dataspace_value,
          0.f, 1.f);

    auto& in = *audio_in;
    auto& out = *audio_out;

    const auto [first_pos, N] = st.timings(t);
    const int64_t last_pos = first_pos + N;

    const auto channels = in.channels();
    out.set_channels(channels);

    for(std::size_t i = 0; i < channels; i++)
    {
      auto& in_c = in.channel(i);
      auto& out_c = out.channel(i);

      const int64_t cur_chan_size = in_c.size();

      out_c.resize(st.bufferSize());

      const auto* input = in_c.data();
      auto* output = out_c.data();
      if(cur_chan_size < last_pos)
      {
        for(int64_t j = first_pos; j < cur_chan_size; j++)
          output[j] = gain * input[j];

        for(int64_t j = cur_chan_size; j < last_pos; j++)
          output[j] = 0.;
      }
      else
      {
        for(int64_t j = first_pos; j < last_pos; j++)
          output[j] = gain * input[j];
      }
    }
  }

private:
  ossia::audio_inlet audio_in;
  ossia::value_inlet gain_in;
  ossia::audio_outlet audio_out;
};
}
