#pragma once

#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/span.hpp>

#include <memory>
#include <string>

namespace ossia
{
// Used in nodes
using audio_channel = ossia::pod_vector<double>;
using audio_vector = std::vector<audio_channel>;


// Used for audio files
using audio_sample = float;
using audio_array = ossia::small_vector<ossia::pod_vector<audio_sample>, 2>;
template<typename T>
using audio_span = ossia::small_vector<gsl::span<const T>, 8>;
template<typename T>
using mutable_audio_span = ossia::small_vector<gsl::span<T>, 8>;

struct audio_data
{
  audio_array data;
  std::string path;
};

#if BOOST_VERSION >= 107200
static_assert(noexcept(audio_data{}));
#endif

using audio_handle = std::shared_ptr<audio_data>;
struct drwav_handle;
/*
struct video_data
{
};

struct mmap_audio_handle
{
  drwav* data{};
};

struct media_data
{
  std::string file_path;
  ossia::variant<audio_handle, mmap_audio_handle> media;
};*/
}
