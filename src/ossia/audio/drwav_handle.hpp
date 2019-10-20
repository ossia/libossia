#pragma once
#define DR_WAV_NO_STDIO
#include <dr_wav.h>

namespace ossia
{
struct drwav_handle final {
public:
  drwav_handle() noexcept = default;
  drwav_handle(const void* data, size_t dataSize) noexcept
    : impl{drwav_open_memory(data, dataSize)}
  {

  }

  drwav_handle(drwav_handle&& other) noexcept:
    impl{other.impl}
  {
    other.impl = nullptr;
  }

  drwav_handle(const drwav_handle& other) noexcept
  {
    if(other.impl && other.impl->memoryStream.data)
    {
      impl = drwav_open_memory(other.impl->memoryStream.data, other.impl->memoryStream.dataSize);
    }
  }

  drwav_handle& operator=(drwav_handle&& other) noexcept
  {
    if(impl)
    {
      drwav_close(impl);
      impl = nullptr;
    }

    impl = other.impl;
    other.impl = nullptr;
    return *this;
  }

  drwav_handle& operator=(const drwav_handle& other) noexcept
  {
    if(impl)
    {
      drwav_close(impl);
      impl = nullptr;
    }

    if(other.impl->memoryStream.data)
    {
      impl = drwav_open_memory(other.impl->memoryStream.data, other.impl->memoryStream.dataSize);
    }
    return *this;
  }

  ~drwav_handle()
  {
    if(impl)
    {
      drwav_close(impl);
    }
  }

  operator bool() const noexcept { return bool(impl); }
  auto open_memory(const void* data, size_t dataSize) noexcept
  {
    if(impl)
      drwav_close(impl);

    impl = drwav_open_memory(data, dataSize);
  }

  auto seek_to_pcm_frame(drwav_uint64 targetFrameIndex) noexcept
  {
    return drwav_seek_to_pcm_frame(impl, targetFrameIndex);
  }

  auto read_pcm_frames(drwav_uint64 framesToRead, void* buffer) noexcept
  {
    return drwav_read_pcm_frames(impl, framesToRead, buffer);
  }

  auto read_pcm_frames_f32(drwav_uint64 framesToRead, float* buffer) noexcept
  {
    return drwav_read_pcm_frames_f32(impl, framesToRead, buffer);
  }

  auto channels() const noexcept { return impl->channels; }
  auto translatedFormatTag() const noexcept { return impl->translatedFormatTag; }
  auto bitsPerSample() const noexcept { return impl->bitsPerSample; }
  auto sampleRate() const noexcept { return impl->sampleRate; }
  auto totalPCMFrameCount() const noexcept { return impl->totalPCMFrameCount; }

  ::drwav* wav() const noexcept { return impl; }
private:

  ::drwav* impl{};
};
}
