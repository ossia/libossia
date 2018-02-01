#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <random>
namespace ossia
{
struct rand_float_node final : public ossia::nonowning_graph_node
{
    ossia::outlet value_out{ossia::value_port{}};
  public:
    std::uniform_real_distribution<float> dist;
    rand_float_node(float min, float max):
      dist{min, max}
    {
      m_outlets.push_back(&value_out);
    }

    void run(ossia::token_request tk, ossia::execution_state& st) override
    {
      thread_local std::mt19937 gen;
      auto& out = *value_out.data.target<ossia::value_port>();
      out.add_value(dist(gen), tk.offset.impl);
    }
};


struct sine_node final : public ossia::nonowning_graph_node
{
    ossia::inlet freq_in{ossia::value_port{}};
    ossia::outlet audio_out{ossia::audio_port{}};
  public:
    double m_cos = 1.;
    double m_sin = 0;
    double freq = 440.;
    sine_node()
    {
      m_inlets.push_back(&freq_in);
      m_outlets.push_back(&audio_out);
    }
    void run(const ossia::token_request tk, ossia::execution_state& st) override
    {
      auto& vals = freq_in.data.target<ossia::value_port>()->get_data();
      if(!vals.empty())
        freq = ossia::clamp(ossia::convert<float>(vals.back().value), 0.f, 20000.f);

      auto& audio = audio_out.data.target<ossia::audio_port>()->samples;
      if(auto N = tk.date - m_prev_date; N > 0)
      {
        audio.resize(1);
        audio[0].resize(tk.offset.impl + N);

        // Uses the method in https://github.com/mbrucher/AudioTK/blob/master/ATK/Tools/SinusGeneratorFilter.cpp
        auto frequ_cos = std::cos(2. * M_PI * freq / st.sampleRate);
        auto frequ_sin = std::sin(2. * M_PI * freq / st.sampleRate);
        for(int64_t i = tk.offset.impl; i < tk.offset.impl + N; i++)
        {
          auto new_cos = m_cos * frequ_cos - m_sin * frequ_sin;
          auto new_sin = m_cos * frequ_sin + m_sin * frequ_cos;
          auto norm = (new_cos * new_cos + new_sin * new_sin);

          m_cos = new_cos / norm;
          m_sin = new_sin / norm;

          audio[0][i] = m_sin;
        }
      }
    }
};

struct gain_node final : public ossia::nonowning_graph_node
{
    double m_gain{0.005};
    ossia::inlet audio_in{ossia::audio_port{}};
    ossia::inlet gain_in{ossia::value_port{}};
    ossia::outlet audio_out{ossia::audio_port{}};
  public:
    gain_node()
    {
      m_inlets.push_back(&audio_in);
      m_inlets.push_back(&gain_in);
      gain_in.data.target<ossia::value_port>()->type = ossia::decibel_u{};
      m_outlets.push_back(&audio_out);
    }
    void run(ossia::token_request tk, ossia::execution_state& st) override
    {
      auto& vals = gain_in.data.target<ossia::value_port>()->get_data();
      if(!vals.empty())
        m_gain = ossia::clamp(ossia::linear{ossia::decibel{ossia::convert<float>(vals.back().value)}}.dataspace_value, 0.f, 1.f);

      auto& in = audio_in.data.target<ossia::audio_port>()->samples;
      auto& out = audio_out.data.target<ossia::audio_port>()->samples;

      const auto N = tk.date - m_prev_date;
      const std::size_t last_pos = tk.offset.impl + N;
      const auto channels = in.size();
      out.resize(channels);

      for(std::size_t i = 0; i < channels; i++)
      {
        const auto cur_chan_size = in[i].size();

        out[i].resize(tk.offset.impl + N);
        if(cur_chan_size < last_pos)
        {
          for(std::size_t j = tk.offset.impl; j < cur_chan_size; j++)
            out[i][j] = m_gain * in[i][j];

          for(std::size_t j = cur_chan_size; j < last_pos; j++)
            out[i][j] = 0.;
        }
        else
        {
          for(std::size_t j = tk.offset.impl; j < last_pos; j++)
            out[i][j] = m_gain * in[i][j];
        }
      }
    }
};
}
