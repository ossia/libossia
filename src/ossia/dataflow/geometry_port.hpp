#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/dataflow/texture_port.hpp>
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/variant.hpp>
#include <ossia/network/value/value.hpp>

#include <cstdint>
#include <limits>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace ossia
{

// clang-format off
// Semantic identification for geometry attributes.
// Custom attributes use attribute_semantic::custom + a string name.
enum class attribute_semantic : uint16_t
{
  // Core geometry
  position          = 0,   // vec3.  Object-space position.
  normal            = 1,   // vec3.  Surface normal.
  tangent           = 2,   // vec4.  xyz=tangent, w=handedness (±1). [glTF TANGENT]
  bitangent         = 3,   // vec3.  cross(N, T.xyz) * T.w.

  // Basic materials
  texcoord0         = 100,   // vec2/3. Primary UV. [glTF TEXCOORD_0]
  texcoord1         = texcoord0 + 1,   // vec2. Secondary UV (lightmaps). [glTF TEXCOORD_1]
  texcoord2         = texcoord0 + 2,   // vec2. Secondary UV. [glTF TEXCOORD_2]
  texcoord3         = texcoord0 + 3,   // vec2. Secondary UV. [glTF TEXCOORD_3]
  texcoord4         = texcoord0 + 4,   // vec2. Secondary UV. [glTF TEXCOORD_4]
  texcoord5         = texcoord0 + 5,   // vec2. Secondary UV. 
  texcoord6         = texcoord0 + 6,   // vec2. Secondary UV. 
  texcoord7         = texcoord0 + 7,   // vec2. Secondary UV.

  color0            = 200,   // vec4.  Vertex color RGBA. [glTF COLOR_0]
  color1            = color0 + 1,   // vec4.  Secondary vertex color. [glTF COLOR_1]
  color2            = color0 + 2,   // vec4.  Secondary vertex color. 
  color3            = color0 + 3,   // vec4.  Secondary vertex color.

  // Skinning / skeletal animation 
  joints0           = 300,  // uvec4. Bone indices, set 0. [glTF JOINTS_0]
  joints1           = joints0 + 1,  // uvec4. Bone indices, set 1. [glTF JOINTS_1]

  weights0          = 400,  // vec4.  Bone weights, set 0. [glTF WEIGHTS_0]
  weights1          = weights0 + 1,  // vec4.  Bone weights, set 1. [glTF WEIGHTS_1]

  // Morph targets / blend shapes
  morph_position    = 500,  // vec3.  Position delta for morph target.
  morph_normal      = morph_position + 1,  // vec3.  Normal delta for morph target.
  morph_tangent     = morph_position + 2,  // vec3.  Tangent delta (no w). [glTF morph TANGENT]
  morph_texcoord    = morph_position + 3,  // vec2.  UV delta for morph target.
  morph_color       = morph_position + 4,  // vec3/4. Color delta for morph target.

  // Transform / instancing
  rotation          = 600,  // vec4.  Quaternion (x,y,z,w). 
  rotation_extra    = rotation + 1,  // vec4.  Post-orient rotation.
  scale             = rotation + 2,  // vec3.  Non-uniform scale. 
  uniform_scale     = rotation + 3,  // float. Uniform scale. 
  up                = rotation + 4,  // vec3.  Up vector for LookAt.
  pivot             = rotation + 5,  // vec3.  Local pivot point.
  transform_matrix  = rotation + 6,  // mat4.  Full transform, overrides TRS. (note: remember that mat4 takes 4 lanes of attributes)
  translation       = rotation + 7,  // vec3.  Additional translation offset.

  // Particle dynamics
  velocity          = 1000,  // vec3.  Velocity in units/sec.
  acceleration      = velocity + 1,  // vec3.  Current acceleration.
  force             = velocity + 2,  // vec3.  Accumulated force this frame.
  mass              = velocity + 3,  // float. 
  age               = velocity + 4,  // float. Time since birth, seconds. 
  lifetime          = velocity + 5,  // float. Max age before death.
  birth_time        = velocity + 6,  // float. Absolute time of birth.
  particle_id       = velocity + 7,  // int.   Stable unique ID.
  drag              = velocity + 8,  // float. Per-particle drag coefficient.
  angular_velocity  = velocity + 9,  // vec3.  Rotation speed rad/sec. 
  previous_position = velocity + 10,  // vec3.  For Verlet / motion blur.
  rest_position     = velocity + 11,  // vec3.  Undeformed position.
  target_position   = velocity + 12,  // vec3.  Goal position for constraints.
  previous_velocity = velocity + 13,  // vec3.  Velocity at previous frame.
  state             = velocity + 14,  // int.   alive/dying/dead/collided enum.
  collision_count   = velocity + 15,  // int.   Number of collisions.
  collision_normal  = velocity + 16,  // vec3.  Normal at last collision.
  sleep             = velocity + 17,  // int.   Dormant flag (skip simulation).

  // Rendering hints
  sprite_size       = 1100,  // vec2.  Billboard width/height.
  sprite_rotation   = sprite_size + 1,  // float. Billboard screen-space rotation.
  sprite_facing     = sprite_size + 2,  // vec3.  Custom billboard facing direction.
  sprite_index      = sprite_size + 3,  // int/float. Sub-image index for sprite sheets.
  width             = sprite_size + 4,  // float. Curve/ribbon thickness.
  opacity           = sprite_size + 5,  // float. Separate from color alpha.
  emissive          = sprite_size + 6,  // vec3.  Self-illumination color.
  emissive_strength = sprite_size + 7,  // float. Emissive intensity multiplier.

  // Material / PBR
  roughness            = 1200, // float. PBR roughness [0-1].
  metallic             = roughness + 1, // float. PBR metalness [0-1].
  ambient_occlusion    = roughness + 2, // float. Baked AO [0-1].
  specular             = roughness + 3, // float. Specular factor.
  subsurface           = roughness + 4, // float. SSS intensity.
  clearcoat            = roughness + 5, // float. Clearcoat factor.
  clearcoat_roughness  = roughness + 6, // float. Clearcoat roughness.
  anisotropy           = roughness + 7, // float. Anisotropic reflection.
  anisotropy_direction = roughness + 8, // vec3.  Anisotropy tangent direction.
  ior                  = roughness + 9, // float. Index of refraction.
  transmission         = roughness + 10, // float. Transmission factor (glass-like).
  thickness            = roughness + 11, // float. Volume thickness for transmission.
  material_id          = roughness + 22, // int.   Index into material array.

  // Gaussian splatting
  sh_dc             = 1300, // vec3.  SH degree-0 (DC) color.
  sh_coeffs         = sh_dc + 1, // float[N]. SH coefficients for higher degrees.
  covariance_3d     = sh_dc + 2, // vec6 or mat3. 3D covariance (6 unique floats).
  sh_degree         = sh_dc + 3, // int.   Active SH degree for this splat (0-3).

  // Volumetric / field data
  density           = 1400, // float. Scalar density. 
  temperature       = density + 1, // float.
  fuel              = density + 2, // float.
  pressure          = density + 3, // float.
  divergence        = density + 4, // float.
  sdf_distance      = density + 5, // float. Signed distance field value.
  voxel_color       = density + 6, // vec4. Per-voxel RGBA.

  // Topology / connectivity
  name              = 1600, // string. Piece/group identifier.
  piece_id          = name + 1, // int.   Numeric piece/group index.
  line_id           = name + 2, // int.   Which line strip this point belongs to.
  prim_id           = name + 3, // int.   Source primitive index.
  point_id          = name + 4, // int.   Stable point ID (distinct from array index).
  group_mask        = name + 5, // int.   Bitfield for group membership.
  instance_id       = name + 6, // int.   Which instance this element belongs to.
  
  // UI
  selection         = 1700, // float. Soft selection weight [0-1].

  instance_color0           = 1800, // vec4. Per-instance broadcast color, set 0.
  instance_color1           = instance_color0 + 1, // vec4.
  instance_color2           = instance_color0 + 2, // vec4.
  instance_color3           = instance_color0 + 3, // vec4.
  instance_custom0          = instance_color0 + 4, // vec4. Per-instance user data, set 0.
  instance_custom1          = instance_color0 + 5, // vec4.
  instance_custom2          = instance_color0 + 6, // vec4.
  instance_custom3          = instance_color0 + 7, // vec4.
  instance_draw_id          = instance_color0 + 8, // uint.
  // User / general purpose
  fx0               = 2000, // float. General-purpose effect control.
  fx1               = fx0 + 1, // float. General-purpose effect control.
  fx2               = fx0 + 2, // float. General-purpose effect control.
  fx3               = fx0 + 3, // float. General-purpose effect control.
  fx4               = fx0 + 4, // float. General-purpose effect control.
  fx5               = fx0 + 5, // float. General-purpose effect control.
  fx6               = fx0 + 6, // float. General-purpose effect control.
  fx7               = fx0 + 7, // float. General-purpose effect control.

  // Custom (string name lookup) 
  custom            = 0xFFFF
};

// Returns a display name for well-known semantics, empty for custom.
OSSIA_EXPORT std::string_view semantic_to_name(attribute_semantic s) noexcept;

// Returns the semantic for a well-known name, or custom if not recognized.
OSSIA_EXPORT attribute_semantic name_to_semantic(std::string_view name) noexcept;

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
    int64_t active_element_count{-1}; // -1 = use full buffer; else first N elements valid
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
      user_struct,
    } format
        = float4;

    uint32_t byte_offset = 0;
    // Semantic identification for this attribute.
    // For well-known semantics, name is empty (derivable from the enum).
    // For custom semantics, name holds the user-defined attribute name.
    attribute_semantic semantic = attribute_semantic::custom;
    std::string name;

    // Used only when format == user_struct: byte size of one element
    // (sizeof of the user-defined struct) and the GLSL type name the
    // consumer CSF declares in its TYPES section. element_byte_size
    // doubles as the per-vertex stride when classification is
    // per_vertex with no padding.
    uint32_t element_byte_size = 0;
    std::string user_type_name;
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

  enum blend_mode : uint8_t
  {
    blend_opaque,
    blend_premultiplied_alpha
  } blend{blend_opaque};
  bool depth_write{true};
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
  struct
  {
    float min[3]{};
    float max[3]{};
  } bounds;
  uint32_t filter_tag{};
  uint32_t filter_material_index{};
  gpu_buffer indirect_count;
  struct draw_command
  {
    uint32_t index_or_vertex_count{};
    uint32_t instance_count{1};
    uint32_t first_index_or_vertex{};
    int32_t base_vertex{};
    uint32_t first_instance{};
  };
  ossia::small_vector<draw_command, 0> cpu_draw_commands;
  struct auxiliary_buffer
  {
    std::string name;      // Shader-visible name for matching (e.g. "particle_aux")
    int buffer{-1};        // Index into the buffers array
    int64_t byte_offset{}; // Offset within the buffer
    int64_t byte_size{};   // Size of the auxiliary data region
  };
  ossia::small_vector<auxiliary_buffer, 1> auxiliary;
  struct auxiliary_texture
  {
    std::string name;       // Shader-visible name (e.g. "base_color_array", "skybox")
    void* native_handle{};  // Backend-owned texture handle
    // Optional sampler handle to bind alongside the texture. When non-
    // null, the renderer uses this sampler instead of the one created
    // from the shader's INPUTS sampler config — used by the
    // ScenePreprocessor's per-bucket sampler split (per-glTF-texture
    // wrap/filter modes). Null = renderer falls back to its own
    // sampler (back-compat with existing producers).
    void* sampler_handle{};
  };
  ossia::small_vector<auxiliary_texture, 2> auxiliary_textures;
  const auxiliary_buffer* find_auxiliary(std::string_view name) const noexcept
  {
    for(auto& a : auxiliary)
      if(a.name == name)
        return &a;
    return nullptr;
  }
  const auxiliary_texture* find_auxiliary_texture(std::string_view name) const noexcept
  {
    for(auto& a : auxiliary_textures)
      if(a.name == name)
        return &a;
    return nullptr;
  }
  const attribute* find(attribute_semantic sem) const noexcept
  {
    for(auto& a : attributes)
      if(a.semantic == sem)
        return &a;
    return nullptr;
  }

  const attribute* find(std::string_view attr_name) const noexcept
  {
    for(auto& a : attributes)
      if(a.semantic == attribute_semantic::custom && a.name == attr_name)
        return &a;
    return nullptr;
  }
  static std::string_view display_name(const attribute& a) noexcept
  {
    if(a.semantic != attribute_semantic::custom)
      return semantic_to_name(a.semantic);
    return a.name;
  }
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

struct gpu_slot_ref
{
  uint32_t arena{};
  uint32_t offset{};
  uint32_t size{};
  uint32_t internal_index{}; // registry-private slot identity for isLive()
  uint32_t generation{};     // bumped on allocate/free — compared by isLive()

  bool valid() const noexcept { return size != 0; }
};
OSSIA_EXPORT uint64_t mint_stable_id() noexcept;
struct scene_transform
{
  float translation[3]{0.0f, 0.0f, 0.0f};
  float rotation[4]{0.0f, 0.0f, 0.0f, 1.0f}; // quaternion (x,y,z,w)
  float scale[3]{1.0f, 1.0f, 1.0f};

  // RawTransform arena slot stamped by the producer (Transform3D,
  // scene-node TRS emitter, loader). See gpu_slot_ref definition for
  // the liveness contract. Opaque to CPU paths.
  gpu_slot_ref raw_slot;

  // Stable id across producer rebuilds. 0 = unset (anonymous transform).
  uint64_t stable_id{};
};

/// Handle to a GPU buffer (vertex, index, uniform, storage)
struct gpu_buffer_handle
{
  void* native_handle{};
  int64_t byte_size{};
  int64_t byte_offset{};
  uint32_t binding_index{};

  bool valid() const noexcept { return native_handle != nullptr; }
  bool operator==(const gpu_buffer_handle&) const noexcept = default;
};
/// Handle to a GPU texture with sampler state
struct gpu_texture_handle
{
  void* native_handle{};
  uint32_t bindless_index{};

  struct sampler_state
  {
    enum class filter : uint8_t { nearest, linear };
    enum class address : uint8_t { repeat, clamp_to_edge, mirror };
    enum class mipmap : uint8_t { none, nearest, linear };

    filter mag : 2 = filter::linear;
    filter min : 2 = filter::linear;
    mipmap mip : 2 = mipmap::linear;
    address u : 2 = address::repeat;
    address v : 2 = address::repeat;
    address w : 2 = address::repeat;

    bool operator==(const sampler_state&) const noexcept = default;
  } sampler{};

  bool valid() const noexcept { return native_handle != nullptr; }
  bool operator==(const gpu_texture_handle&) const noexcept = default;
};
struct buffer_data
{
  std::shared_ptr<const void> data;
  int64_t byte_size{};

  enum class usage : uint8_t
  {
    vertex_buffer,
    index_buffer,
    uniform_buffer,
    storage_buffer,
    indirect_buffer
  } usage_hint{usage::vertex_buffer};
};

struct buffer_resource
{
  ossia::variant<buffer_data, gpu_buffer_handle> resource;
  uint64_t content_hash{};
  int64_t dirty_index{};

  bool is_gpu_resident() const noexcept
  {
    return ossia::get_if<gpu_buffer_handle>(&resource) != nullptr;
  }
};
using buffer_resource_ptr = std::shared_ptr<const buffer_resource>;

enum class vertex_format : uint8_t
{
  float1, float2, float3, float4,
  half1, half2, half3, half4,
  unorm8x1, unorm8x2, unorm8x4,
  snorm8x1, snorm8x2, snorm8x4,
  uint8x1, uint8x2, uint8x4,
  uint16x1, uint16x2, uint16x4,
  uint32x1, uint32x2, uint32x3, uint32x4,
  sint8x1, sint8x2, sint8x4,
  sint16x1, sint16x2, sint16x4,
  sint32x1, sint32x2, sint32x3, sint32x4,
  rgb10a2_unorm, rg11b10_float,
  user_struct
};

struct vertex_attribute
{
  attribute_semantic semantic{}; // reuses the existing attribute_semantic enum
  vertex_format format{vertex_format::float3};
  uint32_t buffer_index{};
  uint32_t byte_offset{};
  uint32_t byte_stride{};
  enum class input_rate : uint8_t { per_vertex, per_instance } rate{};

  // Used only when format == vertex_format::user_struct. element_byte_size
  // equals sizeof of the user-defined struct (typically == byte_stride
  // for tightly-packed per-vertex data); user_type_name is the GLSL
  // type name the consumer CSF declares in its TYPES section.
  uint32_t element_byte_size = 0;
  std::string user_type_name;
};

enum class primitive_topology : uint8_t
{
  points, lines, line_strip, triangles, triangle_strip, triangle_fan,
  patches, meshlets
};

enum class index_format : uint8_t { none, uint16, uint32 };


struct aabb
{
  float min[3]{};
  float max[3]{};
  bool operator==(const aabb&) const noexcept = default;

  // Empty/uninitialised sentinel: an inverted box (min > max on every axis).
  // Used as the default so renderers can detect "no bounds computed" and
  // fall back to an infinite AABB (never-culled) instead of a degenerate
  // point at origin.
  constexpr bool empty() const noexcept
  {
    return min[0] > max[0] || min[1] > max[1] || min[2] > max[2];
  }

  constexpr void expand(float x, float y, float z) noexcept
  {
    if(empty())
    {
      min[0] = max[0] = x;
      min[1] = max[1] = y;
      min[2] = max[2] = z;
      return;
    }
    if(x < min[0]) min[0] = x; else if(x > max[0]) max[0] = x;
    if(y < min[1]) min[1] = y; else if(y > max[1]) max[1] = y;
    if(z < min[2]) min[2] = z; else if(z > max[2]) max[2] = z;
  }
};

inline aabb compute_aabb_from_positions(
    const float* positions, std::size_t vertex_count) noexcept
{
  aabb out{};
  // Mark empty: inverted box. expand() bootstraps on the first vertex.
  out.min[0] = out.min[1] = out.min[2] = 1.f;
  out.max[0] = out.max[1] = out.max[2] = -1.f;
  if(!positions || vertex_count == 0)
    return out;
  for(std::size_t i = 0; i < vertex_count; ++i)
  {
    out.expand(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
  }
  return out;
}

struct material_component;
using material_component_ptr = std::shared_ptr<const material_component>;

struct skeleton_component;
using skeleton_component_ptr = std::shared_ptr<const skeleton_component>;

struct mesh_primitive
{
  ossia::small_vector<buffer_resource_ptr, 4> vertex_buffers;
  buffer_resource_ptr index_buffer;
  ossia::small_vector<vertex_attribute, 8> attributes;

  primitive_topology topology{primitive_topology::triangles};
  index_format index_type{index_format::none};

  uint32_t vertex_count{};
  uint32_t index_count{};
  uint32_t first_vertex{};
  uint32_t first_index{};
  int32_t vertex_offset{};

  material_component_ptr material;
  aabb bounds{};
  float line_width{1.0f};
  gpu_slot_ref raw_slot;
  uint64_t stable_id{};
  ossia::small_vector<material_component_ptr, 0> material_variants;
};

struct morph_target
{
  buffer_resource_ptr position_deltas;
  buffer_resource_ptr normal_deltas;
  buffer_resource_ptr tangent_deltas;
  float default_weight{0.0f};
};

using scene_property_map = ossia::flat_map<std::string, ossia::value>;

struct mesh_component
{
  ossia::small_vector<mesh_primitive, 4> primitives;
  ossia::small_vector<morph_target, 8> morph_targets;
  ossia::small_vector<float, 8> morph_weights;

  skeleton_component_ptr skin;
  aabb bounds{};

  uint8_t lod_level{0};
  float lod_screen_coverage{0.0f};

  scene_property_map properties;
  geometry_spec legacy_geometry;

  int64_t dirty_index{};
};
using mesh_component_ptr = std::shared_ptr<const mesh_component>;
struct texture_source
{
  std::string file_path;                                      // Filesystem path (empty = use embedded_data)
  std::shared_ptr<const std::vector<uint8_t>> embedded_data;  // Inline image bytes (glTF bufferView, FBX blob)
  std::string mime_type;                                      // "image/png", "image/jpeg", "image/ktx2", etc.

  uint64_t content_hash{};
};
struct texture_sampler_config
{
  texture_address_mode wrap_s : 2 = texture_address_mode::REPEAT;
  texture_address_mode wrap_t : 2 = texture_address_mode::REPEAT;
  texture_filter mag_filter : 2 = texture_filter::LINEAR;
  texture_filter min_filter : 2 = texture_filter::LINEAR;
  texture_filter mipmap_mode : 2 = texture_filter::LINEAR;

  constexpr uint32_t hash() const noexcept
  {
    return uint32_t(wrap_s)
        | (uint32_t(wrap_t) << 2)
        | (uint32_t(mag_filter) << 4)
        | (uint32_t(min_filter) << 6)
        | (uint32_t(mipmap_mode) << 8);
  }
  bool operator==(const texture_sampler_config&) const noexcept = default;
};
struct texture_ref
{
  gpu_texture_handle texture{};   // GPU-resident handle (filled by renderer after upload)
  uint32_t texcoord_set{0};

  std::shared_ptr<const texture_source> source;

  struct transform
  {
    float offset[2]{0.0f, 0.0f};
    float scale[2]{1.0f, 1.0f};
    float rotation{0.0f};
  } uv_transform{};
  texture_sampler_config sampler{};

  bool valid() const noexcept { return texture.valid() || source; }
};
enum class alpha_mode : uint8_t { opaque_, mask, blend };

struct material_component
{
  float base_color_factor[4]{1.0f, 1.0f, 1.0f, 1.0f};
  texture_ref base_color_texture;

  float metallic_factor{1.0f};
  float roughness_factor{1.0f};
  texture_ref metallic_roughness_texture;

  texture_ref normal_texture;
  float normal_scale{1.0f};

  texture_ref occlusion_texture;
  float occlusion_strength{1.0f};

  float emissive_factor[3]{0.0f, 0.0f, 0.0f};
  texture_ref emissive_texture;
  float emissive_strength{1.0f};

  alpha_mode alpha{alpha_mode::opaque_};
  float alpha_cutoff{0.5f};
  bool double_sided{false};

  struct { float factor{0.0f}; texture_ref texture; } transmission{};

  struct {
    float thickness_factor{0.0f};
    texture_ref thickness_texture;
    float attenuation_distance{std::numeric_limits<float>::infinity()};
    float attenuation_color[3]{1.0f, 1.0f, 1.0f};
  } volume{};

  float ior{1.5f};

  struct {
    float factor{1.0f};
    texture_ref texture;
    float color_factor[3]{1.0f, 1.0f, 1.0f};
    texture_ref color_texture;
  } specular{};

  struct {
    float factor{0.0f};
    texture_ref texture;
    float roughness_factor{0.0f};
    texture_ref roughness_texture;
    texture_ref normal_texture;
  } clearcoat{};

  struct {
    float color_factor[3]{0.0f, 0.0f, 0.0f};
    texture_ref color_texture;
    float roughness_factor{0.0f};
    texture_ref roughness_texture;
  } sheen{};

  struct {
    float factor{0.0f};
    texture_ref texture;
    float ior{1.3f};
    float thickness_min{100.0f};
    float thickness_max{400.0f};
    texture_ref thickness_texture;
  } iridescence{};

  struct {
    float strength{0.0f};
    float rotation{0.0f};
    texture_ref texture;
  } anisotropy{};

  struct {
    float factor{0.0f};
    float color_factor[3]{1.0f, 1.0f, 1.0f};
    texture_ref texture;
    texture_ref color_texture;
  } diffuse_transmission{};

  bool unlit{false};

  bool shadow_caster{true};       // false → skipped by shadow-map passes.
  bool shadow_receiver{true};     // false → lit as if no shadow hits it.
  bool reflection_caster{true};   // false → skipped by planar / cubemap reflection passes.

  std::string tag;

  scene_property_map properties;

  int64_t dirty_index{};
  gpu_slot_ref raw_slot;
  uint64_t stable_id{};
};
using material_component_ptr = std::shared_ptr<const material_component>;

enum class light_type : uint8_t
{
  directional, point, spot,
  rect_area, disk_area, sphere_area, cylinder_area,
  dome
};

enum class light_decay : uint8_t
{
  none,       // constant intensity regardless of distance
  linear,     // 1/d
  quadratic,  // 1/d² (physically correct)
  cubic       // 1/d³
};
struct light_component
{
  light_type type{light_type::point};
  light_decay decay{light_decay::quadratic};

  float color[3]{1.0f, 1.0f, 1.0f};
  float intensity{1.0f};
  float range{0.0f}; // 0 = infinite (or renderer default)

  float inner_cone_angle{0.0f};
  float outer_cone_angle{0.7853981f}; // pi/4

  // Area light dimensions.
  float width{1.0f};
  float height{1.0f};
  float radius{0.5f};

  buffer_resource_ptr ies_profile;
  std::shared_ptr<const texture_source> environment_texture;

  struct {
    bool enabled{true};
    float bias{0.001f};
    float normal_bias{0.01f};
    uint32_t map_resolution{1024};
    float near_plane{0.1f};
    float far_plane{100.0f};
  } shadow{};

  int64_t dirty_index{};
  gpu_slot_ref raw_slot;
  uint64_t stable_id{};
};
using light_component_ptr = std::shared_ptr<const light_component>;

enum class camera_projection : uint8_t { perspective, orthographic, fulldome };

struct camera_component
{
  camera_projection projection{camera_projection::perspective};

  float yfov{0.7853981f}; // 45° in radians
  float aspect_ratio{1.0f};

  float xmag{1.0f};
  float ymag{1.0f};

  float znear{0.1f};
  float zfar{1000.0f};

  struct {
    float focal_length{50.0f};      // mm
    float focus_distance{10.0f};    // m
    float fstop{5.6f};
    float horizontal_aperture{36.0f}; // mm
    float vertical_aperture{24.0f};   // mm
  } physical{};

  bool enable_dof{false};

  int64_t dirty_index{};
  gpu_slot_ref raw_slot;
  uint64_t stable_id{};
};
using camera_component_ptr = std::shared_ptr<const camera_component>;

struct scene_node_id
{
  uint64_t value{};

  bool operator==(const scene_node_id&) const noexcept = default;
  bool operator<(const scene_node_id& rhs) const noexcept { return value < rhs.value; }

  static scene_node_id from_path(std::string_view path) noexcept;
  static scene_node_id from_parent(scene_node_id parent, std::string_view name) noexcept;
};

struct skeleton_joint
{
  std::string name;            // Bone name from source file (for retargeting)
  int32_t parent_index{-1};
  float inverse_bind_matrix[16]{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  float translation[3]{0, 0, 0};
  float rotation[4]{0, 0, 0, 1};  // quaternion (x, y, z, w)
  float scale[3]{1, 1, 1};
};

enum class skinning_method : uint8_t
{
  linear_blend,       // Standard LBS
  dual_quaternion     // DQ skinning (avoids candy-wrapper artifacts)
};

struct skeleton_component
{
  ossia::small_vector<skeleton_joint, 64> joints;
  buffer_resource_ptr joint_matrices_buffer;

  ossia::small_vector<scene_node_id, 64> joint_node_ids;

  skinning_method method{skinning_method::linear_blend};

  int64_t dirty_index{};

  int32_t find_joint(std::string_view name) const noexcept
  {
    for(int32_t i = 0; i < (int32_t)joints.size(); i++)
      if(joints[i].name == name)
        return i;
    return -1;
  }
};
using skeleton_component_ptr = std::shared_ptr<const skeleton_component>;

enum class animation_interpolation : uint8_t { step, linear, cubic_spline };
enum class animation_target : uint8_t { translation, rotation, scale, weights, custom };

struct animation_channel
{
  uint64_t target_node_id{};
  animation_target target_path{};
  std::string custom_path;

  animation_interpolation interpolation{animation_interpolation::linear};

  std::shared_ptr<const std::vector<float>> times;
  std::shared_ptr<const std::vector<float>> values;
};

struct animation_component
{
  ossia::small_vector<animation_channel, 16> channels;

  float duration{0.0f};
  bool loop{false};

  int64_t dirty_index{};
};
using animation_component_ptr = std::shared_ptr<const animation_component>;


struct gaussian_splat_component
{
  buffer_resource_ptr positions;
  buffer_resource_ptr covariances;
  buffer_resource_ptr sh_coefficients;
  buffer_resource_ptr opacities;

  uint32_t splat_count{};
  uint8_t sh_degree{3};

  struct {
    buffer_resource_ptr cluster_bounds;
    buffer_resource_ptr cluster_indices;
    uint32_t cluster_count{};
    float lod_bias{0.0f};
  } clustering{};

  float splat_scale{1.0f};
  bool use_sh{true};

  int64_t dirty_index{};
};
using gaussian_splat_component_ptr = std::shared_ptr<const gaussian_splat_component>;


struct voxel_field_component
{
  buffer_resource_ptr morton_codes;
  buffer_resource_ptr densities;
  buffer_resource_ptr colors;
  buffer_resource_ptr emissions;

  uint32_t voxel_count{};
  uint32_t resolution[3]{256, 256, 256};

  float bounds_min[3]{};
  float bounds_max[3]{};

  float density_scale{1.0f};
  float step_size{0.01f};

  int64_t dirty_index{};
};
using voxel_field_component_ptr = std::shared_ptr<const voxel_field_component>;

// Generic, format-agnostic point-cloud / splat container.
// Carries N primitives whose schema lives entirely in the consumer
// (a CSF chain that names columns via AUXILIARY LAYOUT) — no fixed
// fields like positions / colors / SH coefficients live here. The
// parser hands us the raw row data verbatim; the format's CSF tells
// the GPU how to read it. Coexists with the typed
// gaussian_splat_component / point_cloud_component / voxel_field_component
// scaffolds; ScenePreprocessor's primitive-cloud branch buckets these
// by format_id and emits one CSF chain per bucket.
struct primitive_cloud_component
{
  // Primary raw payload. Convention: this is the verbatim PLY-row data
  // (header stripped) for a .ply, or the post-decode bytes for binary
  // formats. ScenePreprocessor uploads it to the GPU and re-exposes it
  // to the format's first CSF as AUXILIARY by name "raw_splats".
  buffer_resource_ptr raw_data;

  // Stride in bytes between consecutive primitives in raw_data. 0 means
  // raw_data isn't a flat row buffer (e.g. format with separate indexes).
  uint32_t row_stride{};

  // Optional named extra buffers for formats that need more than one
  // CPU-side input array (e.g. quantized SH codebook + per-primitive
  // indices). Each entry's name becomes the AUXILIARY name on the
  // bucket geometry downstream.
  struct named_buffer
  {
    std::string name;
    buffer_resource_ptr data;
  };
  ossia::small_vector<named_buffer, 2> extra_buffers;

  uint64_t primitive_count{};
  primitive_topology topology{primitive_topology::points};

  // Format identity. Empty = unrouted / wired by hand by the user in
  // the editor. When set (e.g. "3dgs.classic"), ScenePreprocessor
  // buckets clouds with matching format_id so they share one CSF
  // chain and one indirect draw — same idea as MDI for meshes.
  std::string format_id;
  scene_property_map format_params;

  // GLSL type name for the per-primitive row payload. When non-empty
  // (e.g. "Splat3DGS"), ScenePreprocessor exposes raw_data as a per-
  // vertex ATTRIBUTE of format=user_struct + this name, so the CSF
  // can declare a matching `TYPES.{NAME=struct_type_name}` and read
  // the row directly with `ISF_READ(geoIn, splat)[idx].field`. When
  // empty, raw_data is exposed as an AUXILIARY block named
  // "raw_splats" — legacy behaviour for formats that haven't been
  // migrated to TYPES yet.
  std::string struct_type_name;

  aabb bounds{};

  int64_t dirty_index{};
  gpu_slot_ref raw_slot;
};
using primitive_cloud_component_ptr = std::shared_ptr<const primitive_cloud_component>;

struct point_cloud_component
{
  buffer_resource_ptr positions;
  buffer_resource_ptr colors;
  buffer_resource_ptr normals;
  buffer_resource_ptr intensities;

  uint64_t point_count{};

  struct {
    buffer_resource_ptr hilbert_keys;
    buffer_resource_ptr range_bounds;
    uint32_t range_count{};
  } spatial_index{};

  float point_size{1.0f};
  enum class size_mode : uint8_t { fixed, adaptive } point_size_mode{};
  bool enable_edl{true};

  int64_t dirty_index{};
};
using point_cloud_component_ptr = std::shared_ptr<const point_cloud_component>;

struct volume_channel
{
  buffer_resource_ptr data;
  enum class type : uint8_t { scalar_float, vector_float, scalar_half } data_type{};
};

struct volume_component
{
  ossia::small_vector<volume_channel, 4> channels;
  buffer_resource_ptr active_voxel_indices;

  uint32_t resolution[3]{};
  float voxel_size{0.1f};
  float bounds_min[3]{};
  float bounds_max[3]{};

  int64_t dirty_index{};
};
using volume_component_ptr = std::shared_ptr<const volume_component>;

struct instance_component
{
  mesh_component_ptr prototype;  // Mesh to draw for each instance

  // Per-instance data buffers, interpreted according to transform_type.
  buffer_resource_ptr instance_transforms;  // Per-instance transforms
  buffer_resource_ptr instance_colors;      // Optional per-instance color (vec4)
  buffer_resource_ptr instance_custom;      // Optional per-instance application data

  uint32_t instance_count{};

  enum class transform_format : uint8_t
  {
    mat4,         // 16 floats per instance (full 4x4 matrix)
    trs,          // 10 floats: vec3 translation + vec4 rotation(quat) + vec3 scale
    translation   // 3 floats: vec3 position only
  } transform_type{transform_format::mat4};

  int64_t dirty_index{};

  gpu_slot_ref raw_slot;
};
using instance_component_ptr = std::shared_ptr<const instance_component>;
struct scene_data
{
  std::string name;                  // Shader-visible name for AUXILIARY matching.
  buffer_resource_ptr data;          // CPU data or GPU handle.
  int64_t dirty_index{};
};
using scene_data_ptr = std::shared_ptr<const scene_data>;

struct scene_node;
using scene_node_ptr = std::shared_ptr<const scene_node>;

using scene_payload = ossia::variant<
    scene_node_ptr,              // Complete subtree
    mesh_component_ptr,          // Mesh
    material_component_ptr,      // Standalone material
    light_component_ptr,         // Light
    camera_component_ptr,        // Camera
    skeleton_component_ptr,      // Skeleton
    animation_component_ptr,     // Animation clip
    instance_component_ptr,      // GPU-instanced mesh
    scene_data_ptr,              // Generic named scene-level buffer
    gaussian_splat_component_ptr,
    voxel_field_component_ptr,
    point_cloud_component_ptr,
    volume_component_ptr,
    primitive_cloud_component_ptr, // Format-agnostic point-cloud / splat
    scene_transform              // Just a transform
    >;
enum class scene_purpose : uint8_t
{
  default_ = 0,  // Always visible regardless of purpose filter.
  render   = 1,  // Final-quality render; skipped by proxy-only passes.
  proxy    = 2,  // Viewport-quality fallback; skipped by render passes.
  guide    = 3,  // UI helpers / gizmos; usually hidden outside editors.
};

struct scene_node
{
  scene_node_id id;
  std::string name;       // Human-readable label (for UI / debugging)
  bool visible{true};     // Toggle rendering without removing from graph
  bool active{true};      // USD-style: false → prim + subtree skipped by
  scene_purpose purpose{scene_purpose::default_};
  std::shared_ptr<const std::vector<scene_payload>> children;

  scene_property_map properties;

  int64_t dirty_index{};

  bool has_children() const noexcept { return children && !children->empty(); }

  template <typename T>
  const T* get_component() const noexcept;
  template <typename T>
  bool is() const noexcept;
};
struct scene_environment
{
  std::shared_ptr<const texture_source> skybox_source;

  gpu_texture_handle skybox_texture;
  gpu_texture_handle irradiance_map;
  gpu_texture_handle prefiltered_map;
  gpu_texture_handle brdf_lut;

  uint32_t params_set{0};
  enum params_bits : uint32_t
  {
    params_exposure_gamma     = 1u << 0,
    params_ambient            = 1u << 1,
    params_fog                = 1u << 2,
    params_render_target_size = 1u << 3,
  };

  float exposure{1.0f};
  float gamma{2.2f};

  struct {
    bool enabled{false};
    float color[3]{0.8f, 0.8f, 0.8f};
    float density{0.01f};
    float start{10.0f};
    float end{100.0f};
    enum class type : uint8_t { linear, exponential, exponential_squared } mode{};
  } fog{};
  float ambient_color[3]{0.03f, 0.03f, 0.03f};
  float ambient_intensity{1.0f};
  uint32_t render_target_size[2]{0, 0};
  gpu_slot_ref raw_slot;
};
struct shadow_cascades_info
{
  static constexpr uint32_t max_cascades = 8;
  float split_view_depths[max_cascades + 1]{};
  float light_view_proj[max_cascades][16]{};
  gpu_texture_handle shadow_map_array{};
  uint32_t cascade_count{0};
  float shadow_distance{100.f};
  float light_direction[3]{0.f, -1.f, 0.f};
};

struct aux_inject_buffer
{
  std::string name;
  void* native_handle{};  // QRhiBuffer*
  int64_t byte_size{};
};
struct aux_inject_texture
{
  std::string name;
  void* native_handle{};  // QRhiTexture*
};
struct scene_collection
{
  std::string name;
  ossia::small_vector<std::string, 8> paths;
  ossia::small_vector<std::string, 4> tags; // optional, free-form
};
using scene_collection_ptr = std::shared_ptr<const scene_collection>;

struct scene_state
{
  std::shared_ptr<const std::vector<scene_node_ptr>> roots;
  std::shared_ptr<const std::vector<material_component_ptr>> materials;
  std::shared_ptr<const std::vector<animation_component_ptr>> animations;
  std::shared_ptr<const std::vector<skeleton_component_ptr>> skeletons;
  std::shared_ptr<const std::vector<camera_component_ptr>> cameras;

  std::shared_ptr<const std::vector<scene_collection_ptr>> collections;

  scene_environment environment{};
  shadow_cascades_info shadow_cascades{};

  ossia::small_vector<aux_inject_buffer, 4>  inject_buffers;
  ossia::small_vector<aux_inject_texture, 4> inject_textures;

  scene_node_id active_camera_id{};
  double time_seconds{0.0};

  int32_t active_variant_index{-1};
  ossia::small_vector<std::string, 0> variant_names;

  int64_t version{};
  int64_t dirty_index{};

  struct {
    uint64_t total_nodes{};
    uint64_t total_triangles{};
    uint64_t total_vertices{};
    uint64_t total_lights{};
    uint64_t total_materials{};
  } statistics{};

  bool empty() const noexcept { return !roots || roots->empty(); }
};
using scene_state_ptr = std::shared_ptr<const scene_state>;


struct scene_spec
{
  scene_state_ptr state;

  struct scene_delta
  {
    ossia::small_vector<scene_node_id, 16> added_nodes;
    ossia::small_vector<scene_node_id, 16> removed_nodes;
    ossia::small_vector<scene_node_id, 16> modified_nodes;
  };
  std::shared_ptr<const scene_delta> delta;

  explicit operator bool() const noexcept { return state != nullptr; }
  bool operator==(const scene_spec& rhs) const noexcept { return state == rhs.state; }
  bool operator<(const scene_spec& rhs) const noexcept { return state < rhs.state; }
};

struct OSSIA_EXPORT scene_port
{
  static const constexpr int which = 4;

  enum dirt_flags : uint8_t
  {
    dirty_transform   = 0x01,
    dirty_geometry    = 0x02,
    dirty_materials   = 0x04,
    dirty_lights      = 0x08,
    dirty_animation   = 0x10,
    dirty_environment = 0x20,
    dirty_structure   = 0x40,
    dirty_all         = 0xFF
  };

  void clear();

  scene_spec scene;
  scene_transform transform{};
  uint8_t flags{};
};

struct scene_delay_line
{
  std::vector<scene_spec> scenes;
  size_t max_frames{60};
  size_t write_index{0};
};
OSSIA_EXPORT scene_spec wrap_geometry_as_scene(const geometry_spec& geom);
OSSIA_EXPORT geometry_spec extract_first_geometry(const scene_spec& scene);
OSSIA_EXPORT scene_spec merge_scenes(std::span<const scene_spec> scenes);
// clang-format on
}
