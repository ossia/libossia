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
  rubberband_stretcher(RubberBand::RubberBandStretcher::PresetOption opt, std::size_t channels, std::size_t sampleRate)
    : m_rubberBand{sampleRate, channels, RubberBand::RubberBandStretcher::OptionProcessRealTime | opt}
  {

  }

  RubberBand::RubberBandStretcher m_rubberBand;

  void run(
      ossia::audio_span& data,
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      const std::size_t chan,
      const std::size_t len,
      const int64_t samples_to_read,
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
      float** const input = (float**)alloca(sizeof(float*) * chan);
      float** const output = (float**)alloca(sizeof(float*) * chan);

      for(std::size_t i = 0; i < chan; i++)
      {
        input[i] = (float*) alloca(sizeof(float) * samples_to_read);
        for(int j = 0; j <samples_to_read; j++)
          input[i][j] = float(data[i][j + t.prev_date]);

        output[i] = (float*) alloca(sizeof(float) * samples_to_write);
        for(int j = 0; j <samples_to_write; j++)
          output[i][j] = 0.f;
      }

      int n = 1;

      m_rubberBand.process(input, samples_to_read, false);

      while (m_rubberBand.available() < samples_to_write || m_rubberBand.getSamplesRequired() > 0)
      {
        const auto new_start = n * samples_to_read + t.prev_date;
        const int max = len - new_start;
        if(max > 0)
        {
          for(std::size_t i = 0; i < chan; i++)
            for(int j = 0; j <samples_to_read; j++)
              input[i][j] = float(data[i][new_start + j]);
        }
        else if(samples_to_read + max > 0)
        {
          for(std::size_t i = 0; i < chan; i++)
          {
            for(int j = 0; j < samples_to_read + max; j++)
              input[i][j] = float(data[i][new_start + j]);
            for(int j = samples_to_read + max; j < samples_to_read; j++)
              input[i][j] = 0.f;
          }
        }
        else
        {
          for(std::size_t i = 0; i < chan; i++)
            for(int j = 0 ; j < samples_to_read; j++)
              input[i][j] = 0.f;
        }
        n++;

        m_rubberBand.process(input, samples_to_read, false);
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
