#pragma once
#include <ossia/detail/config.hpp>
#include <ossia/detail/small_vector.hpp>

#include <memory>

namespace ossia
{
struct geometry
{
  struct buffer
  {
    std::shared_ptr<void> data{};
    int64_t size{};
    bool dirty{};
  };

  struct binding
  {
    uint32_t stride{};
    enum
    {
      per_vertex,
      per_instance
    } classification{};
    int step_rate{};
  };

  struct attribute
  {
    int binding = 0;
    int location = 0;

    enum
    {
      fp4,
      fp3,
      fp2,
      fp1,
      unsigned4,
      unsigned2,
      unsigned1
    } format
        = fp4;

    uint32_t offset = 0;
  };

  struct input
  {
    int buffer{};
    int64_t offset{};
  };

  ossia::small_vector<buffer, 2> buffers;
  ossia::small_vector<binding, 2> bindings;
  ossia::small_vector<attribute, 2> attributes;
  ossia::small_vector<input, 2> input;

  int vertices{}, indices{};
  enum
  {
    triangles,
    triangle_strip,
    triangle_fan,
    lines,
    line_strip,
    points
  } topology;
  enum
  {
    none,
    front,
    back
  } cull_mode;
  enum
  {
    counter_clockwise,
    clockwise
  } front_face;

  struct
  {
    int buffer{-1};
    int64_t offset{};
    enum
    {
      uint16,
      uint32
    } format{};
  } index;
};

struct mesh_list
{
  std::vector<geometry> meshes;
};

struct transform3d
{
  float matrix[16]{
                      1., 0., 0., 0.,
                      0., 1., 0., 0.,
                      0., 0., 1., 0.,
                      0., 0., 0., 1.,
  };
};

struct OSSIA_EXPORT geometry_port
{
  static const constexpr int which = 4;
  enum dirt_flags { dirty_transform = 0x1, dirty_meshes = 0x2 };

  void clear();

  mesh_list meshes;
  transform3d transform;
  uint8_t flags{};
};

struct geometry_delay_line
{
  std::vector<mesh_list> meshes;
};

}
