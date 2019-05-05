#pragma once

#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <ossia/detail/pod_vector.hpp>
#include <ossia/detail/small_vector.hpp>
#include <dr_wav.h>
namespace ossia
{
using audio_sample = double;
// TODO we should move to pector / uninit'ed vector.
using audio_array = ossia::small_vector<ossia::pod_vector<audio_sample>, 2>;
struct audio_data
{
  audio_array data;
};

struct mmap_audio_handle
{
  drwav* data{};
};

struct video_data
{
};

using audio_handle = std::shared_ptr<audio_data>;

struct media_data
{
  std::string file_path;
  std::variant<audio_handle, mmap_audio_handle> media;
};
}
