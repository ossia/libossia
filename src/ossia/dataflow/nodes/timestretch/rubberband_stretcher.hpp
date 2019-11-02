#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <RubberBandStretcher.h>

#include <iostream>
namespace ossia
{

struct rubberband_stretcher
{
  rubberband_stretcher(
      RubberBand::RubberBandStretcher::PresetOption opt,
      std::size_t channels,
      std::size_t sampleRate,
      int64_t pos)
    : m_rubberBand{sampleRate, channels, RubberBand::RubberBandStretcher::OptionProcessRealTime | opt}
    , next_sample_to_read{pos}

  {

  }

  int64_t next_sample_to_read = 0;
  RubberBand::RubberBandStretcher m_rubberBand;

  template<typename T>
  void run(
      T& audio_fetcher,
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      const std::size_t chan,
      const std::size_t len,
      int64_t samples_to_read,
      const int64_t samples_to_write,
      ossia::audio_port& ap) noexcept
  {
    const double r = 1.0 / t.speed;
    if(r != m_rubberBand.getTimeRatio())
    {
      m_rubberBand.setTimeRatio(r);
    }

    if (t.date > t.prev_date)
    {
      // TODO : if T::sample_type == float we could leverage it directly as input
      float** const input = (float**)alloca(sizeof(float*) * chan);
      float** const output = (float**)alloca(sizeof(float*) * chan);
      for(int i = 0; i < chan; i++)
      {
        input[i] =  (float*) alloca(sizeof(float) * std::max(16L, samples_to_read));
        output[i] = (float*) alloca(sizeof(float) * samples_to_write);
      }

      while (m_rubberBand.available() < samples_to_write)
      {
        audio_fetcher.fetch_audio(next_sample_to_read, samples_to_read, input);

        m_rubberBand.process(input, samples_to_read, false);

        next_sample_to_read += samples_to_read;
        samples_to_read = 16;
      }

      m_rubberBand.retrieve(output, std::min((int)samples_to_write, m_rubberBand.available()));

      for (std::size_t i = 0; i < chan; i++)
      {
        for (int64_t j = 0; j < samples_to_write; j++)
        {
          ap.samples[i][j + t.offset.impl] = double(output[i][j]);
        }
      }
    }
    else
    {
      // TODO
    }
  }
};
}
