#include <ossia/dataflow/geometry_port.hpp>

#include <array>
#include <cctype>

namespace ossia
{

struct semantic_entry
{
  attribute_semantic semantic;
  std::string_view name;
};

static bool icase_equal(std::string_view a, std::string_view b) noexcept
{
  if(a.size() != b.size())
    return false;
  for(std::size_t i = 0; i < a.size(); ++i)
    if(std::tolower(static_cast<unsigned char>(a[i]))
       != std::tolower(static_cast<unsigned char>(b[i])))
      return false;
  return true;
}

// Sorted by enum value for binary search in semantic_to_name.
// Sorted by name for binary search in name_to_semantic (separate array below).
static constexpr std::array semantic_table = {
    // Core geometry
    semantic_entry{attribute_semantic::position, "position"},
    semantic_entry{attribute_semantic::normal, "normal"},
    semantic_entry{attribute_semantic::tangent, "tangent"},
    semantic_entry{attribute_semantic::bitangent, "bitangent"},

    // Basic materials
    semantic_entry{attribute_semantic::texcoord0, "texcoord0"},
    semantic_entry{attribute_semantic::texcoord1, "texcoord1"},
    semantic_entry{attribute_semantic::texcoord2, "texcoord2"},
    semantic_entry{attribute_semantic::texcoord3, "texcoord3"},
    semantic_entry{attribute_semantic::texcoord4, "texcoord4"},
    semantic_entry{attribute_semantic::texcoord5, "texcoord5"},
    semantic_entry{attribute_semantic::texcoord6, "texcoord6"},
    semantic_entry{attribute_semantic::texcoord7, "texcoord7"},

    semantic_entry{attribute_semantic::color0, "color0"},
    semantic_entry{attribute_semantic::color1, "color1"},
    semantic_entry{attribute_semantic::color2, "color2"},
    semantic_entry{attribute_semantic::color3, "color3"},

    // Skinning / skeletal animation
    semantic_entry{attribute_semantic::joints0, "joints0"},
    semantic_entry{attribute_semantic::joints1, "joints1"},
    semantic_entry{attribute_semantic::weights0, "weights0"},
    semantic_entry{attribute_semantic::weights1, "weights1"},

    // Morph targets / blend shapes
    semantic_entry{attribute_semantic::morph_position, "morph_position"},
    semantic_entry{attribute_semantic::morph_normal, "morph_normal"},
    semantic_entry{attribute_semantic::morph_tangent, "morph_tangent"},
    semantic_entry{attribute_semantic::morph_texcoord, "morph_texcoord"},
    semantic_entry{attribute_semantic::morph_color, "morph_color"},

    // Transform / instancing
    semantic_entry{attribute_semantic::rotation, "rotation"},
    semantic_entry{attribute_semantic::rotation_extra, "rotation_extra"},
    semantic_entry{attribute_semantic::scale, "scale"},
    semantic_entry{attribute_semantic::uniform_scale, "uniform_scale"},
    semantic_entry{attribute_semantic::up, "up"},
    semantic_entry{attribute_semantic::pivot, "pivot"},
    semantic_entry{attribute_semantic::transform_matrix, "transform_matrix"},
    semantic_entry{attribute_semantic::translation, "translation"},

    // Particle dynamics
    semantic_entry{attribute_semantic::velocity, "velocity"},
    semantic_entry{attribute_semantic::acceleration, "acceleration"},
    semantic_entry{attribute_semantic::force, "force"},
    semantic_entry{attribute_semantic::mass, "mass"},
    semantic_entry{attribute_semantic::age, "age"},
    semantic_entry{attribute_semantic::lifetime, "lifetime"},
    semantic_entry{attribute_semantic::birth_time, "birth_time"},
    semantic_entry{attribute_semantic::particle_id, "particle_id"},
    semantic_entry{attribute_semantic::drag, "drag"},
    semantic_entry{attribute_semantic::angular_velocity, "angular_velocity"},
    semantic_entry{attribute_semantic::previous_position, "previous_position"},
    semantic_entry{attribute_semantic::rest_position, "rest_position"},
    semantic_entry{attribute_semantic::target_position, "target_position"},
    semantic_entry{attribute_semantic::previous_velocity, "previous_velocity"},
    semantic_entry{attribute_semantic::state, "state"},
    semantic_entry{attribute_semantic::collision_count, "collision_count"},
    semantic_entry{attribute_semantic::collision_normal, "collision_normal"},
    semantic_entry{attribute_semantic::sleep, "sleep"},

    // Rendering hints
    semantic_entry{attribute_semantic::sprite_size, "sprite_size"},
    semantic_entry{attribute_semantic::sprite_rotation, "sprite_rotation"},
    semantic_entry{attribute_semantic::sprite_facing, "sprite_facing"},
    semantic_entry{attribute_semantic::sprite_index, "sprite_index"},
    semantic_entry{attribute_semantic::width, "width"},
    semantic_entry{attribute_semantic::opacity, "opacity"},
    semantic_entry{attribute_semantic::emissive, "emissive"},
    semantic_entry{attribute_semantic::emissive_strength, "emissive_strength"},

    // Material / PBR
    semantic_entry{attribute_semantic::roughness, "roughness"},
    semantic_entry{attribute_semantic::metallic, "metallic"},
    semantic_entry{attribute_semantic::ambient_occlusion, "ambient_occlusion"},
    semantic_entry{attribute_semantic::specular, "specular"},
    semantic_entry{attribute_semantic::subsurface, "subsurface"},
    semantic_entry{attribute_semantic::clearcoat, "clearcoat"},
    semantic_entry{attribute_semantic::clearcoat_roughness, "clearcoat_roughness"},
    semantic_entry{attribute_semantic::anisotropy, "anisotropy"},
    semantic_entry{attribute_semantic::anisotropy_direction, "anisotropy_direction"},
    semantic_entry{attribute_semantic::ior, "ior"},
    semantic_entry{attribute_semantic::transmission, "transmission"},
    semantic_entry{attribute_semantic::thickness, "thickness"},
    semantic_entry{attribute_semantic::material_id, "material_id"},

    // Gaussian splatting
    semantic_entry{attribute_semantic::sh_dc, "sh_dc"},
    semantic_entry{attribute_semantic::sh_coeffs, "sh_coeffs"},
    semantic_entry{attribute_semantic::covariance_3d, "covariance_3d"},
    semantic_entry{attribute_semantic::sh_degree, "sh_degree"},

    // Volumetric / field data
    semantic_entry{attribute_semantic::density, "density"},
    semantic_entry{attribute_semantic::temperature, "temperature"},
    semantic_entry{attribute_semantic::fuel, "fuel"},
    semantic_entry{attribute_semantic::pressure, "pressure"},
    semantic_entry{attribute_semantic::divergence, "divergence"},
    semantic_entry{attribute_semantic::sdf_distance, "sdf_distance"},
    semantic_entry{attribute_semantic::voxel_color, "voxel_color"},

    // Topology / connectivity
    semantic_entry{attribute_semantic::name, "name"},
    semantic_entry{attribute_semantic::piece_id, "piece_id"},
    semantic_entry{attribute_semantic::line_id, "line_id"},
    semantic_entry{attribute_semantic::prim_id, "prim_id"},
    semantic_entry{attribute_semantic::point_id, "point_id"},
    semantic_entry{attribute_semantic::group_mask, "group_mask"},
    semantic_entry{attribute_semantic::instance_id, "instance_id"},

    // UI
    semantic_entry{attribute_semantic::selection, "selection"},

    // User / general purpose
    semantic_entry{attribute_semantic::fx0, "fx0"},
    semantic_entry{attribute_semantic::fx1, "fx1"},
    semantic_entry{attribute_semantic::fx2, "fx2"},
    semantic_entry{attribute_semantic::fx3, "fx3"},
    semantic_entry{attribute_semantic::fx4, "fx4"},
    semantic_entry{attribute_semantic::fx5, "fx5"},
    semantic_entry{attribute_semantic::fx6, "fx6"},
    semantic_entry{attribute_semantic::fx7, "fx7"},
};

std::string_view semantic_to_name(attribute_semantic s) noexcept
{
  // FIXME lower_bound or boost::bimap if there's a constexpr one
  for(auto& e : semantic_table)
    if(e.semantic == s)
      return e.name;
  return {};
}

attribute_semantic name_to_semantic(std::string_view name) noexcept
{
  for(auto& e : semantic_table)
    if(icase_equal(e.name, name))
      return e.semantic;
  return attribute_semantic::custom;
}

}
