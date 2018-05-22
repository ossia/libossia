#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/audio/audio_parameter.hpp>

namespace ossia::nodes
{

struct execution_state;
class OSSIA_EXPORT sound final :
    public ossia::nonowning_graph_node
{
  public:
    sound()
    {
      m_outlets.push_back(&audio_out);
    }
    ~sound()
    {

    }

    void set_start(std::size_t v) { start = v; }
    void set_upmix(std::size_t v) { upmix = v; }
    void set_sound(const std::vector<std::vector<float>>& vec)
    {
      m_data.resize(vec.size());
      for(std::size_t i = 0; i < vec.size(); i++)
      {
        m_data[i].assign(vec[i].begin(), vec[i].end());
      }
    }
    void set_sound(std::vector<std::vector<double>> vec)
    {
      m_data = std::move(vec);
    }
    void run(ossia::token_request t, ossia::execution_state& e) override
    {
      if(m_data.empty())
        return;
      const std::size_t chan = m_data.size();
      const std::size_t len = m_data[0].size();

      ossia::audio_port& ap = *audio_out.data.target<ossia::audio_port>();
      ap.samples.resize(chan);
      int64_t max_N = std::min(t.date.impl, (int64_t)len);
      if(max_N <= 0)
        return;
      const auto samples = max_N - t.prev_date + t.offset.impl;
      if(samples <= 0)
        return;

      if(t.date > t.prev_date)
      {
        for(std::size_t i = 0; i < chan; i++)
        {
          ap.samples[i].resize(samples);
          for(int64_t j = t.prev_date; j < max_N; j++)
          {
            ap.samples[i][j - t.prev_date + t.offset.impl] = m_data[i][j];
          }
          do_fade(
                t.start_discontinuous,
                t.end_discontinuous,
                ap.samples[i],
                t.offset.impl,
                samples);
        }
      }
      else
      {
        // TODO rewind correctly and add rubberband
        for(std::size_t i = 0; i < chan; i++)
        {
          ap.samples[i].resize(samples);
          for(int64_t j = t.prev_date; j < max_N; j++)
          {
            ap.samples[i][max_N - (j - t.prev_date) + t.offset.impl] = m_data[i][j];
          }

          do_fade(
                t.start_discontinuous,
                t.end_discontinuous,
                ap.samples[i],
                max_N + t.offset.impl,
                t.prev_date + t.offset.impl);
        }
      }



      // Upmix
      if(upmix != 0)
      {
        if(upmix < chan)
        {
          /*
      // Downmix
      switch(upmix)
      {
        case 1:
        {
          for(std::size_t i = 1; i < chan; i++)
          {
            if(ap.samples[0].size() < ap.samples[i].size())
              ap.samples[0].resize(ap.samples[i].size());

            for(std::size_t j = 0; j < ap.samples[i].size(); j++)
              ap.samples[0][j] += ap.samples[i][j];
          }
        }
        default:
          // TODO
          break;
      }
      */
        }
        else if(upmix > chan)
        {
          switch(chan)
          {
            case 1:
            {
              ap.samples.resize(upmix);
              for(std::size_t i = 1; i < upmix; i++)
              {
                ap.samples[i] = ap.samples[0];
              }
              break;
            }
            default:
              // TODO
              break;
          }
        }
      }

      // Move channels
      if(start != 0)
      {
        ap.samples.insert(ap.samples.begin(), start, ossia::audio_channel{});
      }
    }
    std::size_t channels() const { return m_data.size(); }
    std::size_t duration() const { return m_data.empty() ? 0 : m_data[0].size(); }

  private:
    std::vector<std::vector<double>> m_data;
    std::size_t start{};
    std::size_t upmix{};
    ossia::outlet audio_out{ossia::audio_port{}};
};
}
