#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/dataflow/audio_port.hpp>

namespace ossia
{

struct raw_stretcher
{
  void run(
      const ossia::audio_span& data,
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
      const int64_t read_max = std::min(t.prev_date.impl + samples_to_write, len);
      for (std::size_t i = 0; i < chan; i++)
      {
        ap.samples[i].resize(samples_to_write + t.offset.impl);
        for (int64_t j = t.prev_date; j < read_max; j++)
        {
          ap.samples[i][j - t.prev_date + t.offset.impl] = data[i][j];
        }
        do_fade(
            t.start_discontinuous, t.end_discontinuous, ap.samples[i],
            t.offset.impl, samples_to_read);
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
