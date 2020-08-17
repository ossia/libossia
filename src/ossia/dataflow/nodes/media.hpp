#pragma once

#include <memory>
#include <ossia/detail/small_vector.hpp>
#include <gsl/span>

namespace ossia
{
// Used in nodes
using audio_channel = ossia::small_pod_vector<double, 256>;
using audio_vector = ossia::small_vector<audio_channel, 2>;


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
  std::variant<audio_handle, mmap_audio_handle> media;
};*/
}
