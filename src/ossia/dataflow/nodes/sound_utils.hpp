#pragma once
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/detail/math.hpp>

#include <algorithm>

namespace ossia
{

// Reverse audio samples in-place for backward playback
template <typename T>
static void reverse_audio_buffer(T** audio_array, std::size_t channels, int64_t samples) noexcept
{
  for(std::size_t i = 0; i < channels; i++)
  {
    std::reverse(audio_array[i], audio_array[i] + samples);
  }
}

// Read audio from buffer going backwards (for reverse playback)
// 'start' is the position we're reading FROM (going towards 0)
// Returns samples in forward order (caller should reverse if needed for stretchers)
template <typename T>
static void read_audio_from_buffer_backward(
    const audio_span<float>& data, const int64_t start, const int64_t samples_to_write,
    const int64_t start_offset, const int64_t loop_duration, const bool loops,
    T** const audio_array) noexcept
{
  const auto channels = data.size();
  if(channels == 0)
    return;
  const int64_t file_duration = data[0].size();

  if(loops && loop_duration > 0)
  {
    // Looping backward: when we hit 0, we wrap to loop_duration
    for(std::size_t i = 0; i < channels; i++)
    {
      auto& src = data[i];
      T* dst = audio_array[i];

      for(int64_t k = 0; k < samples_to_write; k++)
      {
        // Going backwards: start - k, with wrapping
        int64_t raw_pos = start - k;

        // Wrap around using modulo (handle negative values)
        int64_t wrapped_pos = ((raw_pos % loop_duration) + loop_duration) % loop_duration;
        int64_t pos = start_offset + wrapped_pos;

        if(pos >= 0 && pos < file_duration)
          dst[k] = src[pos];
        else
          dst[k] = 0;
      }
    }
  }
  else
  {
    // No looping: just read backwards, zero-fill if we go past the beginning
    for(std::size_t i = 0; i < channels; i++)
    {
      const auto& src = data[i];
      T* dst = audio_array[i];

      for(int64_t k = 0; k < samples_to_write; k++)
      {
        int64_t pos = start_offset + start - k;

        if(pos >= 0 && pos < file_duration)
          dst[k] = src[pos];
        else
          dst[k] = 0;
      }
    }
  }
}

template <typename T>
static void read_audio_from_buffer(
    const audio_span<float>& data, const int64_t start, const int64_t samples_to_write,
    const int64_t start_offset, const int64_t loop_duration, const bool loops,
    T** const audio_array) noexcept
{
  const auto channels = data.size();
  if(channels == 0)
    return;
  const int64_t file_duration = data[0].size();

  if(loops)
  {
    // Case where we won't loop around at all in that buffer
    if(start + samples_to_write < loop_duration)
    {
      const int64_t read_start = start_offset + start;

      // Absolute best case where we can just copy directly the whole buffer.
      // read_start can be negative when the transport crosses zero, hence the
      // lower bound: without it this indexes before the start of the sample.
      if(read_start >= 0 && read_start + samples_to_write < file_duration)
      {
        for(std::size_t i = 0; i < channels; i++)
        {
          auto& src = data[i];
          T* dst = audio_array[i];

          for(int64_t k = 0; k < samples_to_write; k++)
          {
            dst[k] = src[read_start + k];
          }
        }
      }
      else
      {
        // Here we must check for both ends of the file
        for(std::size_t i = 0; i < channels; i++)
        {
          auto& src = data[i];
          T* dst = audio_array[i];

          for(int64_t k = 0; k < samples_to_write; k++)
          {
            int64_t pos = read_start + k;
            if(pos >= 0 && pos < file_duration)
              dst[k] = src[pos];
            else
              dst[k] = 0;
          }
        }
      }
    }
    else
    {
      // Here we may loop within that buffer
      for(std::size_t i = 0; i < channels; i++)
      {
        auto& src = data[i];
        T* dst = audio_array[i];

        for(int64_t k = 0; k < samples_to_write; k++)
        {
          // Floor-modulo: the built-in % keeps the sign of the dividend, so a
          // negative (start + k) would wrap to a negative index.
          const int64_t raw_pos = start + k;
          const int64_t wrapped_pos
              = ((raw_pos % loop_duration) + loop_duration) % loop_duration;
          const int64_t pos = start_offset + wrapped_pos;
          if(pos >= 0 && pos < file_duration)
            dst[k] = src[pos];
          else
            dst[k] = 0;
        }
      }
    }
  }
  else
  {
    for(std::size_t i = 0; i < channels; i++)
    {
      const auto& src = data[i];
      T* dst = audio_array[i];

      const int64_t read_start = start + start_offset;

      if(read_start >= 0 && file_duration >= read_start + samples_to_write)
      {
        // Absolute best case where we can copy the whole buffer
        for(int64_t k = 0, pos = read_start; k < samples_to_write; k++, pos++)
        {
          dst[k] = src[pos];
        }
      }
      else
      {
        // This buffer straddles one or both ends of the file: zero-fill the
        // part before sample 0 (the transport can be before the start when it
        // crosses zero) and the part past the end.
        const int64_t lead = ossia::clamp(-read_start, (int64_t)0, samples_to_write);
        const int64_t max
            = ossia::clamp(file_duration - read_start, lead, samples_to_write);
        for(int64_t k = 0; k < lead; k++)
        {
          dst[k] = 0;
        }
        for(int64_t k = lead, pos = read_start + lead; k < max; k++, pos++)
        {
          dst[k] = src[pos];
        }
        for(int64_t k = max; k < samples_to_write; k++)
        {
          dst[k] = 0;
        }
      }
    }
  }
}
}
