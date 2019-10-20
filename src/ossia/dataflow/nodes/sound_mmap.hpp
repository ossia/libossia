#pragma once
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/audio/drwav_handle.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/nodes/sound.hpp>
#include <ossia/dataflow/audio_stretch_mode.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/dataflow/nodes/media.hpp>

#include <type_traits>

namespace ossia::nodes
{

class sound_mmap final : public ossia::nonowning_graph_node
{
public:
  sound_mmap()
  {
    m_outlets.push_back(&audio_out);
  }

  ~sound_mmap()
  {
  }

  void set_start(std::size_t v)
  {
    start = v;
  }

  void set_upmix(std::size_t v)
  {
    upmix = v;
  }

  void set_sound(drwav_handle hdl)
  {
    using namespace snd;
    m_handle = std::move(hdl);
    if(m_handle)
    {
      switch(m_handle.translatedFormatTag())
      {
        case DR_WAVE_FORMAT_PCM:
        {
          switch(m_handle.bitsPerSample())
          {
            case 16:
              m_converter = read_s16;
              break;
            case 24:
              m_converter = read_s24;
              break;
            case 32:
              m_converter = read_s32;
              break;
          }
          break;
        }
        case DR_WAVE_FORMAT_IEEE_FLOAT:
        {
          switch(m_handle.bitsPerSample())
          {
            case 32:
              m_converter = read_f32;
              break;
            case 64:
              m_converter = read_f64;
              break;
          }
          break;
        }
        default:
          m_converter = nullptr;
          break;
      }
    }
    m_resampler.reset(0_tv, m_mode, m_handle.channels(), m_handle.sampleRate());
  }

  void set_native_tempo(double v)
  {
    tempo = v;
  }

  void set_stretch_mode(ossia::audio_stretch_mode mode)
  {
    if(m_mode != mode)
    {
      m_mode = mode;
      m_resampler.reset(0_tv, m_mode, channels(), m_handle.sampleRate());
    }
  }

  void reset_resampler(time_value date)
  {
    m_resampler.reset(date, m_mode, channels(), m_handle.sampleRate());
  }

  ossia::audio_span<float> fetch_audio(int64_t start, int64_t dur) noexcept
  {
    // Read data into the allocated space
    // TODO if(current_pcm_frame != start)

    const auto N = channels();
    ossia::audio_span<float> ret(N);

    m_handle.seek_to_pcm_frame(start);

    const auto bytes_to_allocate = sizeof(double) * dur * N;
    if(bytes_to_allocate < 300000)
    {
      frame_data = alloca(bytes_to_allocate);
    }
    else
    {
      m_safetyBuffer.resize(bytes_to_allocate); // TODO pector
      frame_data = m_safetyBuffer.data();
    }

    auto count = m_handle.read_pcm_frames(dur, frame_data);
    if(count == 0)
    {
      ret.clear();
      return ret;
    }
    ossia::mutable_audio_span<float> source(N);

    m_resampleBuffer.resize(N);
    for(int i = 0; i < N; i++)
    {
      m_resampleBuffer[i].resize(count);
      source[i] = gsl::span(m_resampleBuffer[i].data(), count);
      ret[i] = gsl::span(m_resampleBuffer[i].data(), count);
    }

    // go from the native wav format to float or double
    m_converter(source, frame_data, count);

    return ret;
  }


  void
  run(ossia::token_request t, ossia::exec_state_facade e) noexcept override
  {
    if(!m_handle)
      return;

    const auto channels = m_handle.channels();
    const auto len = m_handle.totalPCMFrameCount();

    ossia::audio_port& ap = *audio_out.data.target<ossia::audio_port>();
    ap.samples.resize(channels);
    const auto [samples_to_read, samples_to_write] = ossia::snd::sample_info(len, e.bufferSize(), t);

    if (samples_to_read <= 0)
      return;


    if (t.date > t.prev_date)
    {
      const bool ok = m_handle.seek_to_pcm_frame(t.prev_date.impl);
      if(!ok)
        return;

      // Allocate some space
      frame_data = nullptr;

      for (std::size_t chan = 0; chan < channels; chan++)
      {
        ap.samples[chan].resize(t.offset.impl + samples_to_write);
      }

      // resample
      m_resampler.run(*this, t, e, channels, len, samples_to_read, samples_to_write, ap);

      for (std::size_t chan = 0; chan < channels; chan++)
      {
        // fade
        snd::do_fade(t.start_discontinuous, t.end_discontinuous, ap.samples[chan],
                     t.offset.impl, samples_to_read);
      }
    }
    else
    {
      /* TODO */

    }


    ossia::snd::perform_upmix(this->upmix, channels, ap);
    ossia::snd::perform_start_offset(this->start, ap);
  }
  std::size_t channels() const
  {
    return m_handle ? m_handle.channels() : 0;
  }
  std::size_t duration() const
  {
    return m_handle ? m_handle.totalPCMFrameCount(): 0;
  }

private:
  drwav_handle m_handle{};

  ossia::outlet audio_out{ossia::audio_port{}};

  std::size_t start{};
  std::size_t upmix{};
  double tempo{};

  using read_fn_t = void(*)(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples);
  read_fn_t m_converter{};
  std::vector<char> m_safetyBuffer;
  std::vector<std::vector<float>> m_resampleBuffer;
  void* frame_data{};

  resampler m_resampler;
  audio_stretch_mode m_mode{};
};

}
