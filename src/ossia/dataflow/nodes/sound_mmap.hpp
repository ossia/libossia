#pragma once
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/audio/drwav_handle.hpp>
#include <ossia/dataflow/audio_stretch_mode.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/dataflow/nodes/sound.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/pod_vector.hpp>

#include <type_traits>

namespace ossia::nodes
{

class sound_mmap final : public ossia::sound_node
{
public:
  sound_mmap() { m_outlets.push_back(&audio_out); }

  ~sound_mmap() = default;

  std::string label() const noexcept override { return "sound_mmap"; }

  void set_start(std::size_t v) { start = v; }

  void set_upmix(std::size_t v) { upmix = v; }

  void set_sound(drwav_handle hdl)
  {
    using namespace snd;
    m_handle = std::move(hdl);
    if(m_handle)
    {
      switch(m_handle.translatedFormatTag())
      {
        case DR_WAVE_FORMAT_PCM: {
          switch(m_handle.bitsPerSample())
          {
            case 8:
              m_converter = read_u8;
              break;
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
        case DR_WAVE_FORMAT_IEEE_FLOAT: {
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
  }

  void transport(time_value date) override
  {
    if(m_handle)
      m_resampler.transport(to_sample(date, m_handle.sampleRate()));
  }

  void fetch_audio(
      int64_t start, int64_t samples_to_write, double** audio_array_base) noexcept
  {
    const int channels = this->channels();
    const int file_duration = this->duration();

    m_resampleBuffer.resize(channels);
    for(auto& buf : m_resampleBuffer)
      buf.resize(samples_to_write);

    float** audio_array = (float**)alloca(sizeof(float*) * channels);
    for(int i = 0; i < channels; i++)
    {
      m_resampleBuffer[i].resize(samples_to_write);
      audio_array[i] = m_resampleBuffer[i].data();
    }

    ossia::mutable_audio_span<float> source(channels);

    double* frame_data{};
    if(samples_to_write * channels > 10000)
    {
      m_safetyBuffer.resize(samples_to_write * channels);
      frame_data = m_safetyBuffer.data();
      // TODO detect if we happen to be in this case often, and if so, garbage
      // collect at some point
    }
    else
    {
      frame_data = (double*)alloca(sizeof(double) * samples_to_write * channels);
    }

    if(m_loops)
    {
      for(int k = 0; k < samples_to_write; k++)
      {
        // TODO add a special case if [0; samples_to_write] don't loop around
        int pos = this->m_start_offset_samples
                  + ((start + k) % this->m_loop_duration_samples);
        if(pos >= file_duration)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const bool ok = this->m_handle.seek_to_pcm_frame(pos);
        if(!ok)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const int max = 1;
        const auto count = this->m_handle.read_pcm_frames(max, frame_data);
        if(count >= 0)
        {
          for(int i = 0; i < channels; i++)
            source[i] = std::span(audio_array[i] + k, count);
          m_converter(source, frame_data, count);
        }
        else
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
        }
      }
    }
    else
    {
      for(int i = 0; i < channels; i++)
      {
        source[i] = std::span(audio_array[i], samples_to_write);
      }

      bool ok = start + m_start_offset_samples < file_duration;
      if(ok)
        ok = ok && this->m_handle.seek_to_pcm_frame(start + m_start_offset_samples);

      if(ok)
      {
        const auto count = this->m_handle.read_pcm_frames(samples_to_write, frame_data);
        m_converter(source, frame_data, count);
        for(int i = 0; i < channels; i++)
          for(int k = count; k < samples_to_write; k++)
            audio_array[i][k] = 0;
      }
      else
      {
        for(int i = 0; i < channels; i++)
          for(int k = 0; k < samples_to_write; k++)
            audio_array[i][k] = 0;
        return;
      }
    }

    for(int i = 0; i < channels; i++)
      std::copy_n(audio_array[i], samples_to_write, audio_array_base[i]);
  }

  void fetch_audio(int64_t start, int64_t samples_to_write, float** audio_array) noexcept
  {
    const int channels = this->channels();
    const int file_duration = this->duration();

    ossia::mutable_audio_span<float> source(channels);

    double* frame_data{};
    if(samples_to_write * channels > 10000)
    {
      m_safetyBuffer.resize(samples_to_write * channels);
      frame_data = m_safetyBuffer.data();
      // TODO detect if we happen to be in this case often, and if so, garbage
      // collect at some point
    }
    else
    {
      frame_data = (double*)alloca(sizeof(double) * samples_to_write * channels);
    }

    if(m_loops)
    {
      for(int k = 0; k < samples_to_write; k++)
      {
        // TODO add a special case if [0; samples_to_write] don't loop around
        int pos = this->m_start_offset_samples
                  + ((start + k) % this->m_loop_duration_samples);
        if(pos >= file_duration)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const bool ok = this->m_handle.seek_to_pcm_frame(pos);
        if(!ok)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const int max = 1;
        const auto count = this->m_handle.read_pcm_frames(max, frame_data);
        if(count >= 0)
        {
          for(int i = 0; i < channels; i++)
            source[i] = std::span(audio_array[i] + k, count);
          m_converter(source, frame_data, count);
        }
        else
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
        }
      }
    }
    else
    {
      for(int i = 0; i < channels; i++)
      {
        source[i] = std::span(audio_array[i], samples_to_write);
      }

      const bool ok = this->m_handle.seek_to_pcm_frame(start + m_start_offset_samples);
      if(!ok)
      {
        for(int i = 0; i < channels; i++)
          for(int k = 0; k < samples_to_write; k++)
            audio_array[i][k] = 0;
        return;
      }

      const auto count = this->m_handle.read_pcm_frames(samples_to_write, frame_data);
      m_converter(source, frame_data, count);
      for(int i = 0; i < channels; i++)
        for(int k = count; k < samples_to_write; k++)
          audio_array[i][k] = 0;
    }
  }

  // Backward audio fetching - reads samples going backwards from 'start'
  void fetch_audio_backward(
      int64_t start, int64_t samples_to_write, double** audio_array_base) noexcept
  {
    const int channels = this->channels();
    const int file_duration = this->duration();

    m_resampleBuffer.resize(channels);
    for(auto& buf : m_resampleBuffer)
      buf.resize(samples_to_write);

    float** audio_array = (float**)alloca(sizeof(float*) * channels);
    for(int i = 0; i < channels; i++)
    {
      m_resampleBuffer[i].resize(samples_to_write);
      audio_array[i] = m_resampleBuffer[i].data();
    }

    ossia::mutable_audio_span<float> source(channels);

    double* frame_data{};
    if(samples_to_write * channels > 10000)
    {
      m_safetyBuffer.resize(samples_to_write * channels);
      frame_data = m_safetyBuffer.data();
    }
    else
    {
      frame_data = (double*)alloca(sizeof(double) * samples_to_write * channels);
    }

    if(m_loops && m_loop_duration_samples > 0)
    {
      // Looping backward: when we hit 0, wrap to loop_duration
      for(int k = 0; k < samples_to_write; k++)
      {
        int64_t raw_pos = start - k;
        int64_t wrapped_pos
            = ((raw_pos % m_loop_duration_samples) + m_loop_duration_samples)
              % m_loop_duration_samples;
        int64_t pos = m_start_offset_samples + wrapped_pos;

        if(pos < 0 || pos >= file_duration)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const bool ok = this->m_handle.seek_to_pcm_frame(pos);
        if(!ok)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const auto count = this->m_handle.read_pcm_frames(1, frame_data);
        if(count >= 0)
        {
          for(int i = 0; i < channels; i++)
            source[i] = std::span(audio_array[i] + k, count);
          m_converter(source, frame_data, count);
        }
        else
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
        }
      }
    }
    else
    {
      // Non-looping backward: read sample by sample going backwards
      for(int k = 0; k < samples_to_write; k++)
      {
        int64_t pos = m_start_offset_samples + start - k;

        if(pos < 0 || pos >= file_duration)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const bool ok = this->m_handle.seek_to_pcm_frame(pos);
        if(!ok)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const auto count = this->m_handle.read_pcm_frames(1, frame_data);
        if(count >= 0)
        {
          for(int i = 0; i < channels; i++)
            source[i] = std::span(audio_array[i] + k, count);
          m_converter(source, frame_data, count);
        }
        else
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
        }
      }
    }

    for(int i = 0; i < channels; i++)
      std::copy_n(audio_array[i], samples_to_write, audio_array_base[i]);
  }

  void
  fetch_audio_backward(int64_t start, int64_t samples_to_write, float** audio_array) noexcept
  {
    const int channels = this->channels();
    const int file_duration = this->duration();

    ossia::mutable_audio_span<float> source(channels);

    double* frame_data{};
    if(samples_to_write * channels > 10000)
    {
      m_safetyBuffer.resize(samples_to_write * channels);
      frame_data = m_safetyBuffer.data();
    }
    else
    {
      frame_data = (double*)alloca(sizeof(double) * samples_to_write * channels);
    }

    if(m_loops && m_loop_duration_samples > 0)
    {
      // Looping backward
      for(int k = 0; k < samples_to_write; k++)
      {
        int64_t raw_pos = start - k;
        int64_t wrapped_pos
            = ((raw_pos % m_loop_duration_samples) + m_loop_duration_samples)
              % m_loop_duration_samples;
        int64_t pos = m_start_offset_samples + wrapped_pos;

        if(pos < 0 || pos >= file_duration)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const bool ok = this->m_handle.seek_to_pcm_frame(pos);
        if(!ok)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const auto count = this->m_handle.read_pcm_frames(1, frame_data);
        if(count >= 0)
        {
          for(int i = 0; i < channels; i++)
            source[i] = std::span(audio_array[i] + k, count);
          m_converter(source, frame_data, count);
        }
        else
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
        }
      }
    }
    else
    {
      // Non-looping backward
      for(int k = 0; k < samples_to_write; k++)
      {
        int64_t pos = m_start_offset_samples + start - k;

        if(pos < 0 || pos >= file_duration)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const bool ok = this->m_handle.seek_to_pcm_frame(pos);
        if(!ok)
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
          continue;
        }

        const auto count = this->m_handle.read_pcm_frames(1, frame_data);
        if(count >= 0)
        {
          for(int i = 0; i < channels; i++)
            source[i] = std::span(audio_array[i] + k, count);
          m_converter(source, frame_data, count);
        }
        else
        {
          for(int i = 0; i < channels; i++)
            audio_array[i][k] = 0;
        }
      }
    }
  }

  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    if(!m_handle || !m_converter)
      return;

    const auto channels = m_handle.channels();
    const auto len = m_handle.totalPCMFrameCount();

    ossia::audio_port& ap = *audio_out;
    ap.set_channels(std::max((std::size_t)upmix, (std::size_t)channels));

    const auto [samples_to_read, samples_to_write]
        = snd::sample_info(e.bufferSize(), e.modelToSamples(), t);
    if(samples_to_write <= 0)
      return;

    assert(samples_to_write > 0);

    const auto samples_offset = t.physical_start(e.modelToSamples());

    // Handle transport for both forward and backward playback
    if(t.forward())
    {
      if(t.prev_date < m_prev_date)
      {
        // Sentinel: we never played.
        if(m_prev_date == ossia::time_value{ossia::time_value::infinite_min})
        {
          if(t.prev_date != 0_tv)
          {
            transport(t.prev_date);
          }
          else
          {
            // Otherwise we don't need transport, everything is already at 0
            m_prev_date = 0_tv;
          }
        }
        else
        {
          transport(t.prev_date);
        }
      }
    }
    else
    {
      // Backward playback transport handling
      if(t.prev_date > m_prev_date)
      {
        // Sentinel: we never played.
        if(m_prev_date == ossia::time_value{ossia::time_value::infinite_min})
        {
          transport(t.prev_date);
        }
        else
        {
          transport(t.prev_date);
        }
      }
    }

    for(std::size_t chan = 0; chan < channels; chan++)
    {
      ap.channel(chan).resize(e.bufferSize());
    }

    const double stretch_ratio = update_stretch(t, e);
    const double abs_stretch_ratio = std::abs(stretch_ratio);

    // Resample (handles both forward and backward internally)
    m_resampler.run(
        *this, t, e, stretch_ratio, channels, len, samples_to_read, samples_to_write,
        samples_offset, ap);

    const bool start_discontinuous = t.start_discontinuous || (m_last_stretch > 70.);
    const bool end_discontinuous = t.end_discontinuous || (abs_stretch_ratio > 70.);
    if(abs_stretch_ratio > 70. && m_last_stretch > 70.)
    {
      [[unlikely]];
      for(std::size_t i = 0; i < channels; i++)
      {
        ossia::snd::do_zero(ap.channel(i), samples_offset, samples_to_write);
      }
    }
    else
    {
      [[likely]];
      for(std::size_t chan = 0; chan < channels; chan++)
      {
        // fade
        snd::do_fade(
            start_discontinuous, end_discontinuous, ap.channel(chan), samples_offset,
            samples_to_write);
      }
    }

    ossia::snd::perform_upmix(this->upmix, channels, ap);
    ossia::snd::perform_start_offset(this->start, ap);

    m_prev_date = t.date;
    m_last_stretch = abs_stretch_ratio;
  }

  [[nodiscard]] std::size_t channels() const
  {
    return m_handle ? m_handle.channels() : 0;
  }
  [[nodiscard]] std::size_t duration() const
  {
    return m_handle ? m_handle.totalPCMFrameCount() : 0;
  }

private:
  drwav_handle m_handle{};

  ossia::audio_outlet audio_out;

  std::size_t start{};
  std::size_t upmix{};

  using read_fn_t
      = void (*)(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples);
  read_fn_t m_converter{};
  std::vector<double> m_safetyBuffer;
  std::vector<std::vector<float>> m_resampleBuffer;
};

}
