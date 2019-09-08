#pragma once
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/dataflow/audio_stretch_mode.hpp>
#include <RubberBandStretcher.h>
#if __has_include(<CDSPResampler.h>)
#include <CDSPResampler.h>
#include <r8bbase.cpp>
#else
#include <r8brain-free-src/CDSPResampler.h>
#endif
using namespace RubberBand;
namespace ossia::nodes
{


class sound_ref final : public ossia::nonowning_graph_node
{
public:
  sound_ref()
    : m_rubberBand(0, 0, RubberBandStretcher::OptionProcessRealTime)
  {
    m_outlets.push_back(&audio_out);
  }

  ~sound_ref()
  {
  }

  std::string label() const noexcept override
  {
    return "sound_ref";
  }

  void set_start(std::size_t v)
  {
    start = v;
  }
  void set_upmix(std::size_t v)
  {
    upmix = v;
  }
  void set_native_tempo(double v)
  {
    tempo = v;
  }

  void set_sound(const audio_handle& hdl, int channels, int sampleRate)
  {
    m_handle = hdl;
    m_data.clear();
    if (hdl)
    {
      m_data.assign(m_handle->data.begin(), m_handle->data.end());
      m_rubberBand.~RubberBandStretcher();
      new (&m_rubberBand) RubberBandStretcher(sampleRate, channels, RubberBandStretcher::OptionProcessRealTime);
    }
  }

  void run_same_tempo(
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
              = m_data[i][j];
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


  void run_rubberband(
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      const std::size_t chan,
      const std::size_t len,
      const int64_t samples_to_read,
      const int64_t samples_to_write,
      ossia::audio_port& ap) noexcept
  {
    if (t.date > t.prev_date)
    {
      float** input = (float**)alloca(sizeof(float*) * chan);
      float** output = (float**)alloca(sizeof(float*) * chan);

      for(std::size_t i = 0; i < chan; i++)
      {
        input[i] = (float*) alloca(sizeof(float) * samples_to_read);
        for(int j = 0; j <samples_to_read; j++)
          input[i][j] = float(m_data[i][j + t.prev_date]);

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
              input[i][j] = float(m_data[i][new_start + j]);
        }
        else if(samples_to_read + max > 0)
        {
          for(std::size_t i = 0; i < chan; i++)
          {
            for(int j = 0; j < samples_to_read + max; j++)
              input[i][j] = float(m_data[i][new_start + j]);
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
        ap.samples[i].resize(t.offset.impl + samples_to_write);
        for (int64_t j = 0; j < samples_to_write; j++)
        {
          ap.samples[i][j + t.offset.impl]
              = double(output[i][j]);
        }
        do_fade(
            t.start_discontinuous, t.end_discontinuous, ap.samples[i],
            t.offset.impl, samples_to_read);
      }
    }
    else
    {
      // TODO
    }
  }

  void run_repitch(
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      const std::size_t chan,
      const std::size_t len,
      const int64_t samples_to_read,
      const int64_t samples_to_write,
      ossia::audio_port& ap) noexcept
  {
    // speed == 0.5 -> 2 times more samples
    double target_rate = this->m_fileSampleRate / t.speed;

    std::vector<std::unique_ptr<r8b::CDSPResampler24>> resamplers;
    std::vector<std::pair<double*, int>> lastResampled;
    for (std::size_t i = 0; i < channels(); ++i)
    {
      /* Create the resampler objects. */
      ap.samples[i].resize(t.offset.impl + samples_to_write);

      if(resamplers.size() < channels())
      {
        lastResampled.push_back({});
        resamplers.emplace_back(std::make_unique<r8b::CDSPResampler24>(this->m_fileSampleRate, target_rate, std::max(int64_t(16384), samples_to_read)));

      }
      double* input = const_cast<double*>(&m_data[i][t.prev_date]);
      double* output = nullptr;
      resamplers[i]->process(input, samples_to_read, output);
      for(int j = 0; j < samples_to_write; j++)
      {
        ap.samples[i][j + t.offset] = output[j];
      }
      do_fade(
          t.start_discontinuous, t.end_discontinuous, ap.samples[i],
          t.offset.impl, samples_to_write);
    }
  }


  void
  run(ossia::token_request t, ossia::exec_state_facade e) noexcept override
  {
    if (m_data.empty())
    {
      return;
    }

    const std::size_t chan = m_data.size();
    const std::size_t len = m_data[0].size();
    ossia::audio_port& ap = *audio_out.data.target<ossia::audio_port>();
    ap.samples.resize(chan);
    int64_t max_N = std::min(t.date.impl, (int64_t)(len));
    if (max_N <= 0)
      return;
    const int64_t samples_to_read = max_N - t.prev_date + t.offset.impl;

    const int64_t samples_to_write = std::min(int64_t(samples_to_read / t.speed), e.bufferSize() - t.offset);

    if (samples_to_read <= 0)
      return;

    m_mode = audio_stretch_mode::Repitch;
    switch(m_mode)
    {
      case audio_stretch_mode::None:
      {
        run_same_tempo(t, e, chan, len, samples_to_read, samples_to_write, ap);
        break;
      }
      case audio_stretch_mode::Repitch:
      {
        run_repitch(t, e, chan, len, samples_to_read, samples_to_write, ap);
        break;
      }
      case audio_stretch_mode::RubberBandStandard:
      case audio_stretch_mode::RubberBandPercussive:
      {
        double r = 1.0/t.speed;
        if(r != m_rubberBand.getTimeRatio())
        {
          m_rubberBand.setTimeRatio(1.0 / t.speed);
        }
        run_rubberband(t, e, chan, len, samples_to_read, samples_to_write, ap);
        break;
      }
    }


    // Upmix
    if (upmix != 0)
    {
      if (upmix < chan)
      {
        /* TODO
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
      else if (upmix > chan)
      {
        switch (chan)
        {
          case 1:
          {
            ap.samples.resize(upmix);
            for (std::size_t i = 1; i < upmix; i++)
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
    if (start != 0)
    {
      ap.samples.insert(ap.samples.begin(), start, ossia::audio_channel{});
    }

  }
  std::size_t channels() const
  {
    return m_data.size();
  }
  std::size_t duration() const
  {
    return m_data.empty() ? 0 : m_data[0].size();
  }

private:
  ossia::small_vector<gsl::span<const double>, 8> m_data;
  std::size_t start{};
  std::size_t upmix{};
  double tempo{};
  int64_t m_fileSampleRate{};
  ossia::outlet audio_out{ossia::audio_port{}};
  audio_handle m_handle{};
  audio_stretch_mode m_mode{};
  RubberBand::RubberBandStretcher m_rubberBand;
};
}

