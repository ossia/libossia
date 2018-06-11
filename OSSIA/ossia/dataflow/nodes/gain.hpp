#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

namespace ossia::nodes
{

struct gain final : public ossia::nonowning_graph_node
{
    double m_gain{0.005};
    ossia::inlet audio_in{ossia::audio_port{}};
    ossia::inlet gain_in{ossia::value_port{}};
    ossia::outlet audio_out{ossia::audio_port{}};
  public:
    gain()
    {
      m_inlets.push_back(&audio_in);
      m_inlets.push_back(&gain_in);
      gain_in.data.target<ossia::value_port>()->type = ossia::decibel_u{};
      m_outlets.push_back(&audio_out);
    }

    void run(ossia::token_request t, ossia::exec_state_facade st) noexcept override
    {
      auto& vals = gain_in.data.target<ossia::value_port>()->get_data();
      if(!vals.empty())
        m_gain = ossia::clamp(ossia::linear{ossia::decibel{ossia::convert<float>(vals.back().value)}}.dataspace_value, 0.f, 1.f);

      auto& in = audio_in.data.target<ossia::audio_port>()->samples;
      auto& out = audio_out.data.target<ossia::audio_port>()->samples;

      const auto N = t.date - t.prev_date;
      const std::size_t last_pos = t.offset.impl + N;
      const auto channels = in.size();
      out.resize(channels);

      for(std::size_t i = 0; i < channels; i++)
      {
        const auto cur_chan_size = in[i].size();

        out[i].resize(t.offset.impl + N);
        if(cur_chan_size < last_pos)
        {
          for(std::size_t j = t.offset.impl; j < cur_chan_size; j++)
            out[i][j] = m_gain * in[i][j];

          for(std::size_t j = cur_chan_size; j < last_pos; j++)
            out[i][j] = 0.;
        }
        else
        {
          for(std::size_t j = t.offset.impl; j < last_pos; j++)
            out[i][j] = m_gain * in[i][j];
        }
      }
    }
};

}
