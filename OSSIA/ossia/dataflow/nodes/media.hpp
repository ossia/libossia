#pragma once

#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/small_vector.hpp>

namespace ossia
{
using audio_sample = double;
using audio_array = ossia::small_vector<ossia::pod_vector<audio_sample>, 2>;
struct audio_data
{
  audio_array data;
};


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
