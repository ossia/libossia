#pragma once
#include <cinttypes>
#include <optional>
namespace ossia
{
enum transport_mode
{
  none,
  client,
  master
};

enum transport_status
{
  stopped,
  starting,
  playing
};

struct audio_tick_state
{
  // Audio samples. Array of arrays pointing to individual channels
  float* const* inputs{};
  float** outputs{};

  // Number of input and output channels
  int32_t n_in{};
  int32_t n_out{};

  // Number of frames to playback
  uint64_t frames{};

  // Absolute transport position in seconds
  double seconds{};

  // Absolute transport position in frames
  std::optional<uint64_t> position_in_frames{};

  // Current transport status
  std::optional<transport_status> status{};
};
}
