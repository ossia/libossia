#pragma once

#include <dr_wav.h>

#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace ossia
{
using audio_sample = double;
using audio_array = std::vector<std::vector<audio_sample>>;
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
