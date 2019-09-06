#pragma once
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <RubberBandStretcher.h>

using namespace RubberBand;
namespace ossia::nodes
{

class sound_ref final : public ossia::nonowning_graph_node
{
public:
  sound_ref()
    :m_rubberBand(44100, 2, RubberBandStretcher::OptionProcessRealTime)
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

  void set_sound(const audio_handle& hdl)
  {
    m_handle = hdl;
    m_data.clear();
    if (hdl)
    {
      m_data.assign(m_handle->data.begin(), m_handle->data.end());
    }
  }

  void run_same_tempo(
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      std::size_t chan,
      std::size_t len,
      int64_t max_N,
      int64_t samples,
      ossia::audio_port& ap
      ) noexcept
  {
    if (t.date > t.prev_date)
    {
      for (std::size_t i = 0; i < chan; i++)
      {
        ap.samples[i].resize(samples);
        for (int64_t j = t.prev_date; j < max_N; j++)
        {
          ap.samples[i][j - t.prev_date + t.offset.impl]
              = m_data[i][j];
        }
        do_fade(
            t.start_discontinuous, t.end_discontinuous, ap.samples[i],
            t.offset.impl, samples);
      }
    }
    else
    {
      // TODO rewind correctly and add rubberband
      for (std::size_t i = 0; i < chan; i++)
      {
        ap.samples[i].resize(samples);
        for (int64_t j = t.prev_date; j < max_N; j++)
        {
          ap.samples[i][max_N - (j - t.prev_date) + t.offset.impl]
              = m_data[i][j];
        }

        do_fade(
            t.start_discontinuous, t.end_discontinuous, ap.samples[i],
            max_N + t.offset.impl, t.prev_date + t.offset.impl);
      }
    }
  }


  void run_stretch(
      const ossia::token_request& t,
      ossia::exec_state_facade e,
      std::size_t chan,
      std::size_t len,
      int64_t max_N,
      int64_t samples,
      ossia::audio_port& ap) noexcept
  {
    if (t.date > t.prev_date)
    {/*
      float** input = (float**)alloca(sizeof(float*) * chan);
      float** output = (float**)alloca(sizeof(float*) * chan);
      for(std::size_t i = 0; i < chan; i++)
      {
        input[i] = (float*) alloca(sizeof(float) * samples);

        output[i] = (float*) alloca(sizeof(float) * samples);

        for(int j = 0; j <samples; j++)
        {
          input[i][j] = float(m_data[i][j + t.prev_date]);
          output[i][j] = 0.f;
        }
      }

      if(m_rubberBand.available() < samples)
      {

      }

      while (m_rubberBand.available() < samples) {
          int needFrames = std::min((int)samples, (int)m_rubberBand.getSamplesRequired());
          if (needFrames > 0) {
              m_rubberBand.process(input, needFrames, false);
          }
      }

      m_rubberBand.retrieve(output, std::min(samples_to_fill, m_rubberBand.available()));

      m_stretcher(input, output, samples);

      for (std::size_t i = 0; i < chan; i++)
      {
        ap.samples[i].resize(samples);
        for (int64_t j = t.prev_date; j < max_N; j++)
        {
          ap.samples[i][j - t.prev_date + t.offset.impl]
              = double(output[i][j - t.prev_date]);
        }
        do_fade(
            t.start_discontinuous, t.end_discontinuous, ap.samples[i],
            t.offset.impl, samples);
      }*/
    }
    else
    {
      // TODO
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
    const int64_t samples = max_N - t.prev_date + t.offset.impl;
    if (samples <= 0)
      return;

    std::cout << "needed: " << samples << std::endl;

    if(t.speed == 1.)
    {
      run_same_tempo(t, e, chan, len, max_N, samples, ap);
    }
    else
    {
      m_rubberBand.setTimeRatio(1.0 / t.speed);
      run_stretch(t, e, chan, len, max_N, samples, ap);
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
  ossia::outlet audio_out{ossia::audio_port{}};
  audio_handle m_handle;
  RubberBand::RubberBandStretcher m_rubberBand;
};
}

