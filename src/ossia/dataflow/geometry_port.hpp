#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/variant.hpp>

#include <memory>
#include <string>

namespace ossia
{
struct geometry
{
  struct cpu_buffer
  {
    std::shared_ptr<void> raw_data;
    int64_t byte_size{};
  };

  struct gpu_buffer
  {
    void* handle{}; // Can be casted to e.g. a QRhiBuffer
    int64_t byte_size{};
  };

  struct buffer
  {
    ossia::variant<cpu_buffer, gpu_buffer> data;
    bool dirty{};
  };

  struct binding
  {
    uint32_t byte_stride{};
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
      float4,
      float3,
      float2,
      float1,
      unormbyte4,
      unormbyte2,
      unormbyte1,
      uint4,
      uint3,
      uint2,
      uint1,
      sint4,
      sint3,
      sint2,
      sint1,
      half4,
      half3,
      half2,
      half1,
      ushort4,
      ushort3,
      ushort2,
      ushort1,
      sshort4,
      sshort3,
      sshort2,
      sshort1,
    } format
        = float4;

    uint32_t byte_offset = 0;
  };

  struct input
  {
    int buffer{};
    int64_t byte_offset{};
  };

  ossia::small_vector<buffer, 2> buffers;
  ossia::small_vector<binding, 2> bindings;
  ossia::small_vector<attribute, 2> attributes;
  ossia::small_vector<input, 2> input;

  int vertices{}, indices{}, instances{1};
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
    int64_t byte_offset{};
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
  int64_t dirty_index{};
};
using mesh_list_ptr = std::shared_ptr<mesh_list>;
struct transform3d
{
  float matrix[16]{
      1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1., 0., 0., 0., 0., 1.,
  };
};

struct geometry_filter
{
  int64_t node_id{}; // which node is responsible for initalizing the UBO
  int64_t filter_id{}; // unique index for this filter instance
  /**
   * @brief shader
   *
   * ```
   * layout(std140, binding = %next%) uniform filter_t {
   *   float v;
   * } filter;
   * void process_vertex(inout vec3 position, inout vec3 normal, inout vec2 uv, inout vec3 tangent, inout vec4 color)
   * {
   *   position.xyz += v; 
   * }
   * ```
   */
  std::string shader;
  int64_t dirty_index{};
};

struct geometry_filter_list
{
  std::vector<geometry_filter> filters;
  int64_t dirty_index{};
};
using geometry_filter_list_ptr = std::shared_ptr<geometry_filter_list>;

struct geometry_spec
{
  mesh_list_ptr meshes;
  geometry_filter_list_ptr filters;

  operator bool() const noexcept { return meshes && filters; }
  bool operator==(const geometry_spec&) const noexcept = default;
  bool operator<(const geometry_spec& rhs) const noexcept
  {
    return (meshes < rhs.meshes) || (meshes == rhs.meshes && filters < rhs.filters);
  }
};

struct OSSIA_EXPORT geometry_port
{
  static const constexpr int which = 4;
  enum dirt_flags
  {
    dirty_transform = 0x1,
    dirty_meshes = 0x2
  };

  void clear();

  geometry_spec geometry;
  transform3d transform;
  uint8_t flags{};
};

struct geometry_delay_line
{
  std::vector<geometry_spec> geometries;
};

}
