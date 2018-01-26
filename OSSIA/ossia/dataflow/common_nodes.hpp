#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <random>
namespace ossia
{
struct rand_float_node final : public ossia::graph_node
{
  public:
    std::uniform_real_distribution<float> dist;
    rand_float_node(float min, float max):
      dist{min, max}
    {
      m_outlets.push_back(ossia::make_outlet<ossia::value_port>());
    }

    void run(ossia::token_request tk, ossia::execution_state& st) override
    {
      thread_local std::mt19937 gen;
      auto& out = *m_outlets[0]->data.target<ossia::value_port>();
      out.add_value(dist(gen), tk.offset.impl);
    }
};


struct sine_node final : public ossia::graph_node
{
  public:
    int64_t pos = 0;
    double freq = 440.;
    sine_node()
    {
      m_inlets.push_back(ossia::make_inlet<ossia::value_port>());
      m_outlets.push_back(ossia::make_outlet<ossia::audio_port>());
    }
    void run(ossia::token_request tk, ossia::execution_state& st) override
    {
      auto& vals = m_inlets[0]->data.target<ossia::value_port>()->get_data();
      if(!vals.empty())
        freq = ossia::clamp(ossia::convert<float>(vals.back().value), 20.f, 2000.f);

      auto& audio = m_outlets[0]->data.target<ossia::audio_port>()->samples;
      auto N = tk.date - m_prev_date;
      audio.resize(1);
      audio[0].resize(tk.offset.impl + N);

      for(int64_t i = tk.offset.impl; i < tk.offset.impl + N; i++)
      {
        audio[0][i] = std::sin(2. * M_PI * freq * pos++ / st.sampleRate);
      }
    }
};

struct gain_node final : public ossia::graph_node
{
  public:
    double gain{0.005};
    gain_node()
    {
      m_inlets.push_back(ossia::make_inlet<ossia::audio_port>());
      m_inlets.push_back(ossia::make_inlet<ossia::value_port>());
      m_outlets.push_back(ossia::make_outlet<ossia::audio_port>());
    }
    void run(ossia::token_request tk, ossia::execution_state& st) override
    {
      auto& vals = m_inlets[1]->data.target<ossia::value_port>()->get_data();
      if(!vals.empty())
        gain = ossia::clamp(ossia::convert<float>(vals.back().value), 0.f, 1.f);

      auto& audio_in = m_inlets[0]->data.target<ossia::audio_port>()->samples;
      auto& audio_out = m_outlets[0]->data.target<ossia::audio_port>()->samples;

      const auto N = tk.date - m_prev_date;
      const std::size_t last_pos = tk.offset.impl + N;
      const auto channels = audio_in.size();
      audio_out.resize(channels);

      for(std::size_t i = 0; i < channels; i++)
      {
        const auto cur_chan_size = audio_in[i].size();

        audio_out[i].resize(tk.offset.impl + N);
        if(cur_chan_size < last_pos)
        {
          for(std::size_t j = tk.offset.impl; j < cur_chan_size; j++)
            audio_out[i][j] = gain * audio_in[i][j];

          for(std::size_t j = cur_chan_size; j < last_pos; j++)
            audio_out[i][j] = 0.;
        }
        else
        {
          for(std::size_t j = tk.offset.impl; j < last_pos; j++)
            audio_out[i][j] = gain * audio_in[i][j];
        }
      }
    }
};
}
