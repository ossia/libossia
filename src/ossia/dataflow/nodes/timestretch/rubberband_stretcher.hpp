#pragma once
#include <ossia/detail/config.hpp>

#if defined(OSSIA_ENABLE_RUBBERBAND)
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/audio_stretch_mode.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/dataflow/token_request.hpp>

#if __has_include(<RubberBandStretcher.h>)
#include <RubberBandStretcher.h>
#elif __has_include(<rubberband/RubberBandStretcher.h>)
#include <rubberband/RubberBandStretcher.h>
#endif

#include <algorithm>
#include <cmath>
#include <vector>

namespace ossia
{
static constexpr auto get_rubberband_preset(ossia::audio_stretch_mode mode)
{
  using opt_t = RubberBand::RubberBandStretcher::Option;
  using preset_t = RubberBand::RubberBandStretcher::PresetOption;
  uint32_t preset = opt_t::OptionProcessRealTime | opt_t::OptionThreadingNever;
  switch(mode)
  {
    case ossia::audio_stretch_mode::RubberBandStandard:
      break;

    case ossia::audio_stretch_mode::RubberBandPercussive:
      preset |= preset_t::PercussiveOptions;
      break;

    case ossia::audio_stretch_mode::RubberBandStandardHQ:
      preset |= RubberBand::RubberBandStretcher::OptionEngineFiner;
      preset |= RubberBand::RubberBandStretcher::OptionPitchHighConsistency;
      break;

    case ossia::audio_stretch_mode::RubberBandPercussiveHQ:
      preset |= preset_t::PercussiveOptions;
      preset |= RubberBand::RubberBandStretcher::OptionEngineFiner;
      preset |= RubberBand::RubberBandStretcher::OptionPitchHighConsistency;
      break;

    default:
      break;
  }

  return preset;
}

struct rubberband_stretcher
{
  // Priming variants exposed for SoundTest sweeps; production uses ZeroPadOnly.
  enum class prime_strategy : uint8_t
  {
    NoPrime,
    ZeroPadOnly,
    BareRecipe,
    ExtendedDrain,
    PreRollRealAudio,
  };
  static inline prime_strategy s_prime_strategy{prime_strategy::ZeroPadOnly};

  rubberband_stretcher(
      uint32_t opt, std::size_t channels, std::size_t sampleRate, int64_t pos)
      : m_rubberBand{std::make_unique<RubberBand::RubberBandStretcher>(
          sampleRate, channels, opt)}
      , next_sample_to_read{pos}
      , options{opt}
  {
    // Pre-size the prime() zero-pad here so the audio thread never allocates.
    // If a later setPitchScale() inflates the required size, prime() no-ops.
    const std::size_t pad
        = m_rubberBand ? m_rubberBand->getPreferredStartPad() : 0;
    if(channels > 0 && pad > 0)
    {
      m_prime_zero_storage.assign(channels * pad, 0.f);
      m_prime_zero_ptrs.resize(channels);
      for(std::size_t c = 0; c < channels; ++c)
      {
        m_prime_zero_ptrs[c] = m_prime_zero_storage.data() + c * pad;
      }
    }
  }

  rubberband_stretcher(const rubberband_stretcher&) = delete;
  rubberband_stretcher& operator=(const rubberband_stretcher&) = delete;
  rubberband_stretcher(rubberband_stretcher&&) = default;
  rubberband_stretcher& operator=(rubberband_stretcher&&) = default;

  std::unique_ptr<RubberBand::RubberBandStretcher> m_rubberBand;
  int64_t next_sample_to_read = 0;
  uint32_t options{};
  bool m_needs_prime{true};

  // Zero-pad scratch buffer for prime(), sized in the ctor.
  std::vector<float> m_prime_zero_storage;
  std::vector<float*> m_prime_zero_ptrs;

  [[nodiscard]] int64_t start_delay() const noexcept
  {
    return m_rubberBand ? int64_t(m_rubberBand->getStartDelay()) : 0;
  }

  void transport(int64_t date)
  {
    m_rubberBand->reset();
    next_sample_to_read = date;
    m_needs_prime = true;
  }

  template <typename T>
  void
  run(T& audio_fetcher, const ossia::token_request& t, ossia::exec_state_facade e,
      double tempo_ratio, const std::size_t chan, const std::size_t len,
      int64_t samples_to_read, const int64_t samples_to_write,
      const int64_t samples_offset, const ossia::mutable_audio_span<double>& ap) noexcept
  {
    const double abs_tempo_ratio = std::min(70., std::abs(tempo_ratio));
    if(abs_tempo_ratio != m_rubberBand->getTimeRatio())
    {
      m_rubberBand->setTimeRatio(abs_tempo_ratio);
    }

    // Lazy pre-roll on first run after ctor/transport(); see prime().
    if(m_needs_prime) [[unlikely]]
    {
      prime(audio_fetcher, chan, t.forward());
      m_needs_prime = false;
    }

    // TODO : if T::sample_type == float we could leverage it directly as
    // input
    const int max_chan = std::max(chan, m_rubberBand->getChannelCount());
    const int frames = std::max((int64_t)16, samples_to_read);
    float** const input = (float**)alloca(sizeof(float*) * max_chan);
    float** const output = (float**)alloca(sizeof(float*) * max_chan);
    for(std::size_t i = 0; i < chan; i++)
    {
      input[i] = (float*)alloca(sizeof(float) * frames);
      output[i] = (float*)alloca(sizeof(float) * samples_to_write);
    }
    for(std::size_t i = chan; i < m_rubberBand->getChannelCount(); i++)
    {
      input[i] = (float*)alloca(sizeof(float) * frames);
      std::fill_n(input[i], frames, 0.f);
      output[i] = (float*)alloca(sizeof(float) * samples_to_write);
    }

    if(t.forward())
    {
      while(m_rubberBand->available() < samples_to_write)
      {
        audio_fetcher.fetch_audio(next_sample_to_read, samples_to_read, input);

        m_rubberBand->process(input, samples_to_read, false);

        next_sample_to_read += samples_to_read;
        samples_to_read = 16;
      }

      m_rubberBand->retrieve(
          output, std::min((int)samples_to_write, m_rubberBand->available()));

      for(std::size_t i = 0; i < chan; i++)
      {
        for(int64_t j = 0; j < samples_to_write; j++)
        {
          ap[i][j + samples_offset] = double(output[i][j]);
        }
      }
    }
    else
    {
      // Backward playback:
      while(m_rubberBand->available() < samples_to_write)
      {
        audio_fetcher.fetch_audio_backward(next_sample_to_read, samples_to_read, input);

        m_rubberBand->process(input, samples_to_read, false);

        next_sample_to_read -= samples_to_read;
        samples_to_read = 16;
      }

      const int retrieved = m_rubberBand->retrieve(
          output, std::min((int)samples_to_write, m_rubberBand->available()));

      for(std::size_t i = 0; i < chan; i++)
      {
        for(int64_t j = 0; j < samples_to_write; j++)
        {
          ap[i][j + samples_offset] = double(output[i][j]);
        }
      }
    }
  }

private:
  // Run the s_prime_strategy variant before the first audible frame.
  template <typename T>
  void prime(T& audio_fetcher, std::size_t chan, bool forward) noexcept
  {
    if(!m_rubberBand || chan == 0)
      return;

    const auto strategy = s_prime_strategy;
    if(strategy == prime_strategy::NoPrime)
      return;

    if(strategy == prime_strategy::PreRollRealAudio)
    {
      // pad ≈ windowSize/2 per RubberBand::getPreferredStartPad docs.
      const int64_t window = 2 * int64_t(m_rubberBand->getPreferredStartPad());

      // Fall through to ZeroPadOnly if there isn't enough past content.
      if(window > 0 && (forward ? next_sample_to_read >= window : true))
      {
        constexpr int prime_fetch_chunk = 256;
        float** const input
            = (float**)alloca(sizeof(float*) * m_rubberBand->getChannelCount());
        float** const output
            = (float**)alloca(sizeof(float*) * m_rubberBand->getChannelCount());
        for(std::size_t i = 0; i < m_rubberBand->getChannelCount(); i++)
        {
          input[i] = (float*)alloca(sizeof(float) * prime_fetch_chunk);
          output[i] = (float*)alloca(sizeof(float) * prime_fetch_chunk);
        }

        if(forward)
          next_sample_to_read -= window;
        else
          next_sample_to_read += window;

        int64_t fed = 0;
        while(fed < window)
        {
          const int64_t chunk
              = std::min<int64_t>(prime_fetch_chunk, window - fed);
          if(forward)
          {
            audio_fetcher.fetch_audio(next_sample_to_read, chunk, input);
            next_sample_to_read += chunk;
          }
          else
          {
            audio_fetcher.fetch_audio_backward(next_sample_to_read, chunk, input);
            next_sample_to_read -= chunk;
          }
          m_rubberBand->process(input, int(chunk), false);
          fed += chunk;
        }

        // Drain pre-K outputs; feed zeros once real input is exhausted
        // to avoid consuming the file beyond K.
        const int64_t target_drain
            = std::max<int64_t>(0, window - int64_t(m_rubberBand->getStartDelay()));
        int64_t drained = 0;
        int safety = 0;
        while(drained < target_drain && safety++ < 4096)
        {
          while(m_rubberBand->available() <= 0)
          {
            std::fill_n(input[0], prime_fetch_chunk, 0.f);
            for(std::size_t i = 1; i < m_rubberBand->getChannelCount(); i++)
              std::fill_n(input[i], prime_fetch_chunk, 0.f);
            m_rubberBand->process(input, prime_fetch_chunk, false);
          }
          const int avail = m_rubberBand->available();
          const int to_take = int(std::min<int64_t>(
              target_drain - drained,
              std::min<int64_t>(avail, prime_fetch_chunk)));
          m_rubberBand->retrieve(output, to_take);
          drained += to_take;
        }
        return;
      }
    }

    const int64_t toPad = int64_t(m_rubberBand->getPreferredStartPad());
    if(toPad <= 0)
      return;

    // Bail without allocating if a later setPitchScale() outgrew the buffer.
    if(int64_t(m_prime_zero_storage.size()) < int64_t(chan) * toPad
       || int64_t(m_prime_zero_ptrs.size()) < int64_t(chan))
    {
      return;
    }

    m_rubberBand->process(
        m_prime_zero_ptrs.data(), int(toPad), /*final=*/false);

    if(strategy == prime_strategy::ZeroPadOnly
       || strategy == prime_strategy::PreRollRealAudio)
      return;

    const int64_t drain_target
        = (strategy == prime_strategy::ExtendedDrain) ? 2 * toPad
                                                      : int64_t(m_rubberBand->getStartDelay());
    if(drain_target <= 0)
      return;

    constexpr int prime_fetch_chunk = 256;
    float** const input
        = (float**)alloca(sizeof(float*) * m_rubberBand->getChannelCount());
    float** const output
        = (float**)alloca(sizeof(float*) * m_rubberBand->getChannelCount());
    for(std::size_t i = 0; i < m_rubberBand->getChannelCount(); i++)
    {
      input[i] = (float*)alloca(sizeof(float) * prime_fetch_chunk);
      output[i] = (float*)alloca(sizeof(float) * prime_fetch_chunk);
      std::fill_n(input[i], prime_fetch_chunk, 0.f);
    }

    int64_t drained = 0;
    int safety = 0;
    while(drained < drain_target && safety++ < 4096)
    {
      while(m_rubberBand->available() <= 0)
      {
        if(forward)
        {
          audio_fetcher.fetch_audio(
              next_sample_to_read, prime_fetch_chunk, input);
          next_sample_to_read += prime_fetch_chunk;
        }
        else
        {
          audio_fetcher.fetch_audio_backward(
              next_sample_to_read, prime_fetch_chunk, input);
          next_sample_to_read -= prime_fetch_chunk;
        }
        m_rubberBand->process(input, prime_fetch_chunk, false);
      }

      const int available = m_rubberBand->available();
      const int to_take = int(std::min<int64_t>(
          drain_target - drained, std::min<int64_t>(available, prime_fetch_chunk)));
      m_rubberBand->retrieve(output, to_take);
      drained += to_take;
    }
  }
};
}
#else
#include <ossia/dataflow/nodes/timestretch/raw_stretcher.hpp>

namespace ossia
{
static constexpr uint32_t get_rubberband_preset(ossia::audio_stretch_mode mode)
{
  return 0;
}
using rubberband_stretcher = raw_stretcher;
}
#endif
