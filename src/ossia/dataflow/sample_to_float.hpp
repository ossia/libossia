#pragma once

#include <ossia/dataflow/nodes/media.hpp>

#include <cstdint>
#include <limits>

namespace ossia
{
template <typename SampleFormat, int N>
constexpr audio_sample sample_to_float(SampleFormat i);

template <>
constexpr audio_sample sample_to_float<uint8_t, 8>(uint8_t i)
{
  // 0 -> 255 to -1 -> 1
  if constexpr(std::is_same_v<ossia::audio_sample, float>)
    return i / 127.f - 1.f;
  else
    return i / 127. - 1.;
}

template <>
constexpr audio_sample sample_to_float<int16_t, 16>(int16_t i)
{
  // TODO division -> multiplication
  if constexpr(std::is_same_v<ossia::audio_sample, float>)
    return (i + .5f) / (0x7FFF + .5f);
  else
    return (i + .5) / (0x7FFF + .5);
}

template <>
constexpr audio_sample sample_to_float<int32_t, 24>(int32_t i)
{
  if constexpr(std::is_same_v<ossia::audio_sample, float>)
    return ((int32_t)i >> 8)
           / ((audio_sample)std::numeric_limits<int32_t>::max() / 256.f);
  else
    return ((int32_t)i >> 8)
           / ((audio_sample)std::numeric_limits<int32_t>::max() / 256.);
}

template <>
constexpr audio_sample sample_to_float<int32_t, 32>(int32_t i)
{
  return i / (audio_sample)(std::numeric_limits<int32_t>::max());
}

template <>
constexpr audio_sample sample_to_float<float, 32>(float i)
{
  return i;
}

template <>
constexpr audio_sample sample_to_float<double, 64>(double i)
{
  return i;
}

inline void read_u8(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  auto d = reinterpret_cast<int8_t*>(data);

  for(int64_t j = 0; j < samples; j++)
  {
    for(std::size_t i = 0; i < channels; i++)
    {
      ap[i][j] = sample_to_float<uint8_t, 8>(d[j * channels + i]);
    }
  }
}

inline void read_s16(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  auto d = reinterpret_cast<int16_t*>(data);

  for(int64_t j = 0; j < samples; j++)
  {
    for(std::size_t i = 0; i < channels; i++)
    {
      ap[i][j] = sample_to_float<int16_t, 16>(d[j * channels + i]);
    }
  }
}

inline void read_s24(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  const auto frame_bytes = channels * 3;

  auto bytes = reinterpret_cast<uint8_t*>(data);
  for(int64_t j = 0; j < samples; j++)
  {
    for(std::size_t i = 0; i < channels; i++)
    {
      int32_t sample = 0;
      sample += (bytes[3 * i] << 8);
      sample += (bytes[3 * i + 1] << 16);
      sample += (bytes[3 * i + 2] << 24);

      ap[i][j] = sample_to_float<int32_t, 24>(sample);
    }
    bytes += frame_bytes;
  }
}

inline void read_s32(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  auto d = reinterpret_cast<int32_t*>(data);

  for(int64_t j = 0; j < samples; j++)
  {
    for(std::size_t i = 0; i < channels; i++)
    {
      ap[i][j] = sample_to_float<int32_t, 32>(d[j * channels + i]);
    }
  }
}

inline void read_f32(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  auto d = reinterpret_cast<float*>(data);

  for(int64_t j = 0; j < samples; j++)
  {
    for(std::size_t i = 0; i < channels; i++)
    {
      ap[i][j] = d[j * channels + i];
    }
  }
}

inline void read_f64(ossia::mutable_audio_span<float>& ap, void* data, int64_t samples)
{
  const auto channels = ap.size();
  auto d = reinterpret_cast<double*>(data);

  for(int64_t j = 0; j < samples; j++)
  {
    for(std::size_t i = 0; i < channels; i++)
    {
      ap[i][j] = d[j * channels + i];
    }
  }
}

}
