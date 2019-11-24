#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/dataflow/audio_port.hpp>

namespace ossia
{

struct raw_stretcher
{
  int64_t next_sample_to_read{};
  raw_stretcher() noexcept = default;
  raw_stretcher(const raw_stretcher&) noexcept = default;
  raw_stretcher(raw_stretcher&&) noexcept = default;
  raw_stretcher& operator=(const raw_stretcher&) noexcept = default;
  raw_stretcher& operator=(raw_stretcher&&) noexcept = default;
  raw_stretcher(int64_t pos) noexcept
    : next_sample_to_read{pos}
  {

  }

  template<typename T>
  void run(
      T& audio_fetcher,
      const ossia::token_request& t,
      const ossia::exec_state_facade e,
      double tempo_ratio,
      const std::size_t chan,
      const int64_t len,
      const int64_t samples_to_read,
      const int64_t samples_to_write,
      const int64_t samples_offset,
      ossia::audio_port& ap
      ) noexcept
  {
    if (t.forward())
    {
      double** output = (double**)alloca(sizeof(double*) * chan);
      for (std::size_t i = 0; i < chan; i++)
        output[i] = ap.samples[i].data() + samples_offset;

      audio_fetcher.fetch_audio(next_sample_to_read, samples_to_write, output);
      next_sample_to_read += samples_to_write;
    }
    else
    {
      // TODO
    }
  }
};


}
