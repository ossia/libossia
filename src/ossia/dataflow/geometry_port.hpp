#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/variant.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

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
  rotation_extra    = morph_position + 1,  // vec4.  Post-orient rotation.
  scale             = morph_position + 2,  // vec3.  Non-uniform scale. 
  uniform_scale     = morph_position + 3,  // float. Uniform scale. 
  up                = morph_position + 4,  // vec3.  Up vector for LookAt.
  pivot             = morph_position + 5,  // vec3.  Local pivot point.
  transform_matrix  = morph_position + 6,  // mat4.  Full transform, overrides TRS. (note: remember that mat4 takes 4 lanes of attributes)
  translation       = morph_position + 7,  // vec3.  Additional translation offset.

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
    } format
        = float4;

    uint32_t byte_offset = 0;

    // Semantic identification for this attribute.
    // For well-known semantics, name is empty (derivable from the enum).
    // For custom semantics, name holds the user-defined attribute name.
    attribute_semantic semantic = attribute_semantic::custom;
    std::string name;
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

  // Axis-aligned bounding box. All zeros = not computed.
  struct
  {
    float min[3]{};
    float max[3]{};
  } bounds;

  // Optional GPU buffer holding a uint32 element count for indirect dispatch/draw.
  // When set, renderers can use drawIndirect/dispatchIndirect.
  gpu_buffer indirect_count;

  // Auxiliary structured buffers that travel with the geometry.
  // These are NOT per-vertex attributes; they are opaque buffers with
  // application-defined internal layouts (e.g. particle bookkeeping structs,
  // indirect dispatch/draw args, index lists).
  // The buffer data lives in the `buffers` array; this struct references it.
  // Consumers match by name against their shader storage declarations.
  struct auxiliary_buffer
  {
    std::string name;      // Shader-visible name for matching (e.g. "particle_aux")
    int buffer{-1};        // Index into the buffers array
    int64_t byte_offset{}; // Offset within the buffer
    int64_t byte_size{};   // Size of the auxiliary data region
  };
  ossia::small_vector<auxiliary_buffer, 1> auxiliary;

  // Find an auxiliary buffer by name. Returns nullptr if not found.
  const auxiliary_buffer* find_auxiliary(std::string_view name) const noexcept
  {
    for(auto& a : auxiliary)
      if(a.name == name)
        return &a;
    return nullptr;
  }

  // Find an attribute by semantic enum. Returns nullptr if not found.
  const attribute* find(attribute_semantic sem) const noexcept
  {
    for(auto& a : attributes)
      if(a.semantic == sem)
        return &a;
    return nullptr;
  }

  // Find a custom attribute by name. Returns nullptr if not found.
  const attribute* find(std::string_view attr_name) const noexcept
  {
    for(auto& a : attributes)
      if(a.semantic == attribute_semantic::custom && a.name == attr_name)
        return &a;
    return nullptr;
  }

  // Returns the display name for an attribute (semantic name or custom name).
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
};
}
