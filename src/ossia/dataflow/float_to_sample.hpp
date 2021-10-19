#pragma once

#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/detail/math.hpp>
#include <cstdint>
#include <limits>

namespace ossia
{
/** Generic versions found on KVR
static inline
int quantize(double x, int w)
{
  const double range = exp2((double) w);

  return (int) clamp(floor(x * range * 0.5), - range * 0.5, range * 0.5 - 1.0);
}

static inline
double dequantize(int n, int w)
{
  const double range = exp2((double) w);

  return ((2.0 * (double) n + range) / (range - 1.0)) - 1.0;
}
*/
template <typename SampleFormat, int N>
constexpr SampleFormat float_to_sample(ossia::audio_sample sample) noexcept;

template <>
constexpr uint8_t float_to_sample<uint8_t, 8>(ossia::audio_sample sample) noexcept
{
  // 0 -> 255 to -1 -> 1
  if constexpr (std::is_same_v<ossia::audio_sample, float>)
  {
    return (sample + 1.f) * 127.5f;
  }
  else
  {
    return (sample + 1.) * 127.5;
  }
}

template <>
constexpr int16_t float_to_sample<int16_t, 16>(ossia::audio_sample sample) noexcept
{
  // TODO division -> multiplication
  if constexpr (std::is_same_v<ossia::audio_sample, float>)
  {
    return sample * (0x7FFF + .5f) - 0.5f;
  }
  else
  {
    return sample * (0x7FFF + .5) - 0.5;
  }
}

// ALSA S24_LE: https://stackoverflow.com/a/40301874/1495627
template <>
constexpr int32_t float_to_sample<int32_t, 24>(ossia::audio_sample sample) noexcept
{
  const constexpr ossia::audio_sample int24_max = std::numeric_limits<int32_t>::max() / 256.;
  return int32_t(sample * int24_max);
}

template <>
constexpr int32_t float_to_sample<int32_t, 32>(ossia::audio_sample sample) noexcept
{
  const constexpr ossia::audio_sample int32_max = ossia::audio_sample(std::numeric_limits<int32_t>::max());
  return sample * int32_max;
}

template <>
constexpr float float_to_sample<float, 32>(float sample) noexcept
{
  return sample;
}

#if defined(_MSC_VER)
#define OSSIA_RESTRICT __restrict
#else
#define OSSIA_RESTRICT __restrict__
#endif

template<typename SampleFormat, int N>
inline void interleave(const float* const* OSSIA_RESTRICT in, SampleFormat* OSSIA_RESTRICT out, int channels, int bs)
{
  for(int c = 0; c < channels; c++)
  {
    auto* in_channel = in[c];
    for(int k = 0; k < bs; k++)
      out[k * channels + c] = float_to_sample<SampleFormat, N>(in_channel[k]);
  }
}

template<typename SampleFormat, int N>
inline void convert(const float* const* OSSIA_RESTRICT in, SampleFormat* OSSIA_RESTRICT out, int channels, int bs)
{
  for(int c = 0; c < channels; c++)
  {
    auto* in_channel = in[c];
    auto* out_channel = out + c * bs;
    for(int k = 0; k < bs; k++)
    {
      out_channel[k] = float_to_sample<SampleFormat, N>(in_channel[k]);
    }
  }
}
}
