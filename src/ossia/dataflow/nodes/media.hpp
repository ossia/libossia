#pragma once

#include <memory>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/small_vector.hpp>
#include <gsl/span>

namespace ossia
{
using audio_sample = double;
using audio_array = ossia::small_vector<ossia::pod_vector<audio_sample>, 2>;
using audio_channel = ossia::small_vector<double, 256>;
using audio_vector = ossia::small_vector<audio_channel, 2>;
using audio_span = ossia::small_vector<gsl::span<const double>, 8>;

struct audio_data
{
  audio_array data;
};

OSSIA_EXPORT
void do_fade(
    bool start_discontinuous, bool end_discontinuous,
    audio_channel& ap,
    std::size_t start, std::size_t end);


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
