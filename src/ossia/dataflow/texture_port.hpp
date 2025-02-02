#pragma once
#include <ossia/detail/config.hpp>

namespace ossia
{

enum texture_filter
{
  NONE,
  NEAREST,
  LINEAR
};

enum texture_address_mode
{
  REPEAT,
  CLAMP_TO_EDGE,
  MIRROR,
};

enum texture_format
{
  RGBA8,
  BGRA8,
  R8,
  RG8,
  R16,
  RG16,
  RED_OR_ALPHA8,

  RGBA16F,
  RGBA32F,
  R16F,
  R32F,

  RGB10A2,

  D16,
  D24,
  D24S8,
  D32F,
};
struct texture_size
{
  int32_t width{};
  int32_t height{};

  bool operator==(const texture_size& other) const noexcept = default;
  bool operator!=(const texture_size& other) const noexcept = default;
};

struct render_target_spec
{
  std::optional<texture_size> size;

  texture_format format = RGBA8;

  texture_filter mag_filter : 2 = texture_filter::LINEAR;
  texture_filter min_filter : 2 = texture_filter::LINEAR;
  texture_filter mipmap_mode : 2 = texture_filter::NONE;

  texture_address_mode address_u : 2 = texture_address_mode::REPEAT;
  texture_address_mode address_v : 2 = texture_address_mode::REPEAT;
  texture_address_mode address_w : 2 = texture_address_mode::REPEAT;

  bool operator==(const render_target_spec& other) const noexcept = default;
  bool operator!=(const render_target_spec& other) const noexcept = default;
};

}
