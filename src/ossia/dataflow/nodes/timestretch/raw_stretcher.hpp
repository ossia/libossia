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
      ossia::audio_span& data,
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      std::size_t chan,
      std::size_t len,
      int64_t samples_to_read,
      int64_t samples_to_write,
      ossia::audio_port& ap
      ) noexcept
  {
    if (t.date > t.prev_date)
    {
      for (std::size_t i = 0; i < chan; i++)
      {
        ap.samples[i].resize(samples_to_write + t.offset.impl);
        for (int64_t j = t.prev_date; j < t.prev_date + samples_to_write && j < len; j++)
        {
          ap.samples[i][j - t.prev_date + t.offset.impl]
              = data[i][j];
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
