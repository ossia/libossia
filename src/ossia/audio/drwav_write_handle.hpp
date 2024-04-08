#pragma once
#include <ossia/dataflow/float_to_sample.hpp>

#include <boost/container/vector.hpp>

#include <dr_wav.h>

#include <ossia-config.hpp>
namespace ossia
{

struct drwav_write_handle
{
  drwav_data_format format;
  static constexpr auto static_cutoff = 4096 * 2;

public:
  drwav_write_handle()
      : impl{new drwav}
  {
  }
  void open(std::string_view path, int channels, int rate, int bits)
  {
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = channels;
    format.sampleRate = rate;
    format.bitsPerSample = bits;

    bool ok = drwav_init_file_write(impl, path.data(), &format, nullptr);

    buffer.reserve(channels * 8192 * (bits / 8));
    started = ok;
  }

  void close()
  {
    if(impl)
    {
      if(started)
      {
        drwav_uninit(impl);
      }
    }
    started = false;
  }

  drwav_uint64 write_pcm_frames(drwav_uint64 frames, const double* const* in)
  {
    this->buffer.clear();
    this->buffer.resize(
        frames * format.channels * (format.bitsPerSample / 8),
        boost::container::default_init);

    switch(format.bitsPerSample)
    {
      case 16:
        interleave<int16_t, 16, 2>(
            in, reinterpret_cast<int16_t*>(buffer.data()), format.channels, frames);

        return drwav_write_raw(impl, frames * 2 * format.channels, buffer.data());
      case 24:
        interleave<int32_t, 24, 3>(
            in, reinterpret_cast<int32_t*>(buffer.data()), format.channels, frames);

        return drwav_write_raw(impl, frames * 3 * format.channels, buffer.data());
      case 32:
        interleave<int32_t, 32, 4>(
            in, reinterpret_cast<int32_t*>(buffer.data()), format.channels, frames);

        return drwav_write_raw(impl, frames * 4 * format.channels, buffer.data());
        break;
    }
    return 0;
  }

  bool is_open() const noexcept { return started; }

private:
  ::drwav* impl{};
  ossia::pod_vector<char> buffer;
  bool started{};
};
}
