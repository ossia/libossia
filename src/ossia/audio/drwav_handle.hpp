#pragma once
#include <ossia/dataflow/float_to_sample.hpp>

#include <dr_wav.h>

#include <ossia-config.hpp>

namespace ossia
{
#pragma pack(push, 1)
struct acid_chunk
{
  // https://github.com/erikd/libsndfile/blob/master/src/wav.c#L1427
  int32_t file_type{};
  int16_t root_note{};
  int16_t fill_0{};
  float fill_1{};
  int32_t num_beats{};
  int16_t meter_denom{};
  int16_t meter_num{};
  float tempo{};
};
#pragma pack(pop)

struct drwav_handle final
{
public:
  static const constexpr drwav_allocation_callbacks drwav_allocs{
      nullptr, [](size_t sz, void* pUserData) { return malloc(sz); },
      [](void* p, size_t sz, void* pUserData) { return realloc(p, sz); },
      [](void* p, void* pUserData) { return free(p); }};
  drwav_handle() noexcept = default;
  drwav_handle(const void* data, size_t dataSize) noexcept
      : impl{new drwav}
  {
    drwav_init_memory_ex(impl, data, dataSize, on_chunk, this, 0, &drwav_allocs);
  }

  drwav_handle(drwav_handle&& other) noexcept
      : impl{other.impl}
      , m_acid{other.m_acid}
  {
    other.impl = nullptr;
    other.m_acid = {};
  }

  drwav_handle(const drwav_handle& other) noexcept
  {
    if(other.impl && other.impl->memoryStream.data)
    {
      impl = new drwav;
      drwav_init_memory_ex(
          impl, other.impl->memoryStream.data, other.impl->memoryStream.dataSize,
          on_chunk, this, 0, &drwav_allocs);
    }
  }

  static drwav_uint64 on_chunk(
      void* pChunkUserData, drwav_read_proc onRead, drwav_seek_proc onSeek,
      void* pReadSeekUserData, const drwav_chunk_header* pChunkHeader,
      drwav_container container, const drwav_fmt* pFMT) noexcept
  {
    drwav_handle& self = *(drwav_handle*)pChunkUserData;
    auto& cc = pChunkHeader->id.fourcc;
    static const constexpr drwav_uint8 acid[4] = {'a', 'c', 'i', 'd'};
    if(std::equal(cc, cc + 4, acid, acid + 4))
    {
      onRead(pReadSeekUserData, &self.m_acid, sizeof(acid_chunk));
      return sizeof(acid_chunk);
    }
    return 0;
  }

  drwav_handle& operator=(drwav_handle&& other) noexcept
  {
    if(impl)
    {
      drwav_uninit(impl);
      delete impl;
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
      drwav_uninit(impl);
      delete impl;
      impl = nullptr;
    }

    if(other.impl->memoryStream.data)
    {
      impl = new drwav;
      drwav_init_memory_ex(
          impl, other.impl->memoryStream.data, other.impl->memoryStream.dataSize,
          on_chunk, this, 0, &drwav_allocs);
    }
    return *this;
  }

  ~drwav_handle()
  {
    if(impl)
    {
      drwav_uninit(impl);
      delete impl;
    }
  }

  operator bool() const noexcept { return bool(impl); }
  auto open_memory(const void* data, size_t dataSize) noexcept
  {
    if(impl)
    {
      drwav_uninit(impl);
      delete impl;
    }

    impl = new drwav;
    drwav_init_memory_ex(impl, data, dataSize, on_chunk, this, 0, &drwav_allocs);
  }

  auto seek_to_pcm_frame(drwav_uint64 targetFrameIndex) noexcept
  {
    return drwav_seek_to_pcm_frame(impl, targetFrameIndex);
  }

  auto read_pcm_frames(drwav_uint64 framesToRead, void* buffer) noexcept
  {
    return drwav_read_pcm_frames(impl, framesToRead, buffer);
  }

  auto read_pcm_frames_s16(drwav_uint64 framesToRead, int16_t* buffer) noexcept
  {
    return drwav_read_pcm_frames_s16(impl, framesToRead, buffer);
  }

  auto read_pcm_frames_f32(drwav_uint64 framesToRead, float* buffer) noexcept
  {
    return drwav_read_pcm_frames_f32(impl, framesToRead, buffer);
  }

  [[nodiscard]] auto channels() const noexcept { return impl->channels; }
  [[nodiscard]] auto translatedFormatTag() const noexcept
  {
    return impl->translatedFormatTag;
  }
  [[nodiscard]] auto bitsPerSample() const noexcept { return impl->bitsPerSample; }
  [[nodiscard]] auto sampleRate() const noexcept { return impl->sampleRate; }
  [[nodiscard]] auto totalPCMFrameCount() const noexcept
  {
    return impl->totalPCMFrameCount;
  }

  [[nodiscard]] ::drwav* wav() const noexcept { return impl; }

  [[nodiscard]] acid_chunk acid() const noexcept { return m_acid; }

private:
  ::drwav* impl{};
  acid_chunk m_acid{};
};
}
