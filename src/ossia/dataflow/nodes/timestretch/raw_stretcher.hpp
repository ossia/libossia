#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/dataflow/audio_port.hpp>

namespace ossia
{

struct raw_stretcher
{
  int64_t cur_pos{};
  raw_stretcher() noexcept = default;
  raw_stretcher(const raw_stretcher&) noexcept = default;
  raw_stretcher(raw_stretcher&&) noexcept = default;
  raw_stretcher& operator=(const raw_stretcher&) noexcept = default;
  raw_stretcher& operator=(raw_stretcher&&) noexcept = default;
  raw_stretcher(int64_t pos) noexcept
    : cur_pos{pos}
  {

  }

  template<typename T>
  void run(
      T& audio_fetcher,
      const ossia::token_request& t,
      const ossia::exec_state_facade e,
      const std::size_t chan,
      const int64_t len,
      const int64_t samples_to_read,
      const int64_t samples_to_write,
      ossia::audio_port& ap
      ) noexcept
  {
    if (t.date > t.prev_date)
    {
      auto data = audio_fetcher.fetch_audio(cur_pos, samples_to_write);
      if(data.empty())
        return;
      cur_pos += samples_to_write;

      for (std::size_t i = 0; i < chan; i++)
      {
        const auto* input = data[i].data();
        double* output = ap.samples[i].data();
        for (int64_t j = 0; j < samples_to_write; j++)
        {
          output[j + t.offset.impl] = input[j];
        }
      }
    }
    else
    {
      /*
      // TODO rewind correctly and add rubberband
      for (std::size_t i = 0; i < chan; i++)
      {
        ap.samples[i].resize(samples_to_read);
        for (int64_t j = t.prev_date; j < max_N; j++)
        {
          ap.samples[i][max_N - (j - t.prev_date) + t.offset.impl]
              = m_data[i][j];
        }

        do_fade(
            t.start_discontinuous, t.end_discontinuous, ap.samples[i],
            max_N + t.offset.impl, t.prev_date + t.offset.impl);
      }
      */
    }
  }
};


}
