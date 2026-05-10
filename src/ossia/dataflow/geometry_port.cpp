#include <ossia/dataflow/geometry_port.hpp>

#include <ossia/detail/ptr_set.hpp>

#include <array>
#include <atomic>
#include <cctype>

namespace ossia
{

uint64_t mint_stable_id() noexcept
{
  static std::atomic<uint64_t> s_next{1};
  return s_next.fetch_add(1, std::memory_order_relaxed);
}

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
    semantic_entry{attribute_semantic::texcoord0, "texcoord"}, // alias
    semantic_entry{attribute_semantic::texcoord1, "texcoord1"},
    semantic_entry{attribute_semantic::texcoord2, "texcoord2"},
    semantic_entry{attribute_semantic::texcoord3, "texcoord3"},
    semantic_entry{attribute_semantic::texcoord4, "texcoord4"},
    semantic_entry{attribute_semantic::texcoord5, "texcoord5"},
    semantic_entry{attribute_semantic::texcoord6, "texcoord6"},
    semantic_entry{attribute_semantic::texcoord7, "texcoord7"},
    semantic_entry{attribute_semantic::texcoord0, "texcoord_0"},
    semantic_entry{attribute_semantic::texcoord1, "texcoord_1"},
    semantic_entry{attribute_semantic::texcoord2, "texcoord_2"},
    semantic_entry{attribute_semantic::texcoord3, "texcoord_3"},
    semantic_entry{attribute_semantic::texcoord4, "texcoord_4"},
    semantic_entry{attribute_semantic::texcoord5, "texcoord_5"},
    semantic_entry{attribute_semantic::texcoord6, "texcoord_6"},
    semantic_entry{attribute_semantic::texcoord7, "texcoord_7"},
    semantic_entry{attribute_semantic::texcoord0, "uv0"},
    semantic_entry{attribute_semantic::texcoord0, "uv"}, // alias
    semantic_entry{attribute_semantic::texcoord1, "uv1"},
    semantic_entry{attribute_semantic::texcoord2, "uv2"},
    semantic_entry{attribute_semantic::texcoord3, "uv3"},
    semantic_entry{attribute_semantic::texcoord4, "uv4"},
    semantic_entry{attribute_semantic::texcoord5, "uv5"},
    semantic_entry{attribute_semantic::texcoord6, "uv6"},
    semantic_entry{attribute_semantic::texcoord7, "uv7"},
    semantic_entry{attribute_semantic::texcoord0, "uv_0"},
    semantic_entry{attribute_semantic::texcoord1, "uv_1"},
    semantic_entry{attribute_semantic::texcoord2, "uv_2"},
    semantic_entry{attribute_semantic::texcoord3, "uv_3"},
    semantic_entry{attribute_semantic::texcoord4, "uv_4"},
    semantic_entry{attribute_semantic::texcoord5, "uv_5"},
    semantic_entry{attribute_semantic::texcoord6, "uv_6"},
    semantic_entry{attribute_semantic::texcoord7, "uv_7"},

    semantic_entry{attribute_semantic::color0, "color0"},
    semantic_entry{attribute_semantic::color0, "color"}, // alias
    semantic_entry{attribute_semantic::color1, "color1"},
    semantic_entry{attribute_semantic::color2, "color2"},
    semantic_entry{attribute_semantic::color3, "color3"},
    semantic_entry{attribute_semantic::color0, "color_0"},
    semantic_entry{attribute_semantic::color1, "color_1"},
    semantic_entry{attribute_semantic::color2, "color_2"},
    semantic_entry{attribute_semantic::color3, "color_3"},

    // Skinning / skeletal animation
    semantic_entry{attribute_semantic::joints0, "joints0"},
    semantic_entry{attribute_semantic::joints1, "joints1"},
    semantic_entry{attribute_semantic::weights0, "weights0"},
    semantic_entry{attribute_semantic::weights1, "weights1"},
    // glTF-style underscored aliases.
    semantic_entry{attribute_semantic::joints0, "joints_0"},
    semantic_entry{attribute_semantic::joints1, "joints_1"},
    semantic_entry{attribute_semantic::weights0, "weights_0"},
    semantic_entry{attribute_semantic::weights1, "weights_1"},

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

    semantic_entry{attribute_semantic::instance_color0, "instance_color0"},
    semantic_entry{attribute_semantic::instance_color1, "instance_color1"},
    semantic_entry{attribute_semantic::instance_color2, "instance_color2"},
    semantic_entry{attribute_semantic::instance_color3, "instance_color3"},
    semantic_entry{attribute_semantic::instance_color0, "instance_color_0"},
    semantic_entry{attribute_semantic::instance_color1, "instance_color_1"},
    semantic_entry{attribute_semantic::instance_color2, "instance_color_2"},
    semantic_entry{attribute_semantic::instance_color3, "instance_color_3"},
    semantic_entry{attribute_semantic::instance_custom0, "instance_custom0"},
    semantic_entry{attribute_semantic::instance_custom1, "instance_custom1"},
    semantic_entry{attribute_semantic::instance_custom2, "instance_custom2"},
    semantic_entry{attribute_semantic::instance_custom3, "instance_custom3"},
    semantic_entry{attribute_semantic::instance_draw_id, "instance_draw_id"},
    // User / general purpose
    semantic_entry{attribute_semantic::fx0, "fx0"},
    semantic_entry{attribute_semantic::fx1, "fx1"},
    semantic_entry{attribute_semantic::fx2, "fx2"},
    semantic_entry{attribute_semantic::fx3, "fx3"},
    semantic_entry{attribute_semantic::fx4, "fx4"},
    semantic_entry{attribute_semantic::fx5, "fx5"},
    semantic_entry{attribute_semantic::fx6, "fx6"},
    semantic_entry{attribute_semantic::fx7, "fx7"},

    // TODO use reflection to build the table whenever we can
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

void geometry_port::clear()
{
  geometry = {};
  transform = {};
  flags = 0;
}

void scene_port::clear()
{
  scene = {};
  transform = {};
  flags = 0;
}

OSSIA_EXPORT
scene_node_id scene_node_id::from_path(std::string_view path) noexcept
{
  // FNV-1a 64-bit hash
  uint64_t h = 14695981039346656037ULL;
  for(auto c : path)
  {
    h ^= static_cast<uint64_t>(c);
    h *= 1099511628211ULL;
  }
  return {h};
}

OSSIA_EXPORT
scene_node_id scene_node_id::from_parent(scene_node_id parent, std::string_view name) noexcept
{
  uint64_t h = parent.value;
  h ^= 0x9e3779b97f4a7c15ULL; // separator
  for(auto c : name)
  {
    h ^= static_cast<uint64_t>(c);
    h *= 1099511628211ULL;
  }
  return {h};
}

scene_spec wrap_geometry_as_scene(const geometry_spec& geom)
{
  if(!geom.meshes || geom.meshes->meshes.empty())
    return {};

  auto mesh = std::make_shared<mesh_component>();
  mesh->legacy_geometry = geom;

  auto children = std::make_shared<std::vector<scene_payload>>();
  children->push_back(mesh_component_ptr(std::move(mesh)));

  auto node = std::make_shared<scene_node>();
  node->children = std::move(children);

  auto roots = std::make_shared<std::vector<scene_node_ptr>>();
  roots->push_back(scene_node_ptr(std::move(node)));

  auto state = std::make_shared<scene_state>();
  state->roots = std::move(roots);

  return scene_spec{std::move(state)};
}

// Walk scene_payload children looking for mesh_components
static geometry_spec find_first_geometry_in_payloads(
    const std::vector<scene_payload>& payloads)
{
  for(auto& payload : payloads)
  {
    if(auto* mesh = ossia::get_if<mesh_component_ptr>(&payload))
    {
      if(*mesh && (*mesh)->legacy_geometry.meshes)
        return (*mesh)->legacy_geometry;
    }
    else if(auto* subnode = ossia::get_if<scene_node_ptr>(&payload))
    {
      if(*subnode && (*subnode)->has_children())
      {
        auto result = find_first_geometry_in_payloads(*(*subnode)->children);
        if(result.meshes)
          return result;
      }
    }
  }
  return {};
}

geometry_spec extract_first_geometry(const scene_spec& scene)
{
  if(!scene.state || scene.state->empty())
    return {};

  for(auto& root : *scene.state->roots)
  {
    if(!root || !root->has_children())
      continue;
    auto result = find_first_geometry_in_payloads(*root->children);
    if(result.meshes)
      return result;
  }
  return {};
}

scene_spec merge_scenes(std::span<const scene_spec> scenes)
{
  int count = 0;
  const scene_spec* single = nullptr;
  for(auto& s : scenes)
  {
    if(s.state)
    {
      count++;
      single = &s;
    }
  }

  if(count == 0)
    return {};
  if(count == 1)
    return *single;

  auto merged_roots = std::make_shared<std::vector<scene_node_ptr>>();

  const std::shared_ptr<const std::vector<material_component_ptr>>*
      single_materials = nullptr;
  int materials_contributors = 0;
  const std::shared_ptr<const std::vector<animation_component_ptr>>*
      single_animations = nullptr;
  int animations_contributors = 0;
  const std::shared_ptr<const std::vector<camera_component_ptr>>*
      single_cameras = nullptr;
  int cameras_contributors = 0;
  const std::shared_ptr<const std::vector<scene_collection_ptr>>* single_collections
      = nullptr;
  int collections_contributors = 0;

  int64_t max_version = 0;
  double max_time = 0.0;
  scene_node_id active_camera_id{};

  scene_environment merged_env{};  // start from defaults

  // Pointer-keyed dedup sets shared across contributors. Identity-based
  // (raw pointer) — two distinct shared_ptrs that wrap the same object
  // collapse to one entry; structurally-equal but separately-allocated
  // payloads keep their distinct identities (which is the right call for
  // independent producers that just happen to publish similar data).
  ossia::ptr_set<const scene_node*> seen_roots;
  ossia::ptr_set<const material_component*> seen_materials;
  ossia::ptr_set<const animation_component*> seen_animations;
  ossia::ptr_set<const camera_component*> seen_cameras;
  ossia::ptr_set<const scene_collection*> seen_collections;

  for(auto& s : scenes)
  {
    if(!s.state)
      continue;
    const auto& e = s.state->environment;

    // Skybox texture: non-null native_handle takes precedence.
    if(e.skybox_texture.native_handle)
      merged_env.skybox_texture = e.skybox_texture;
    if(e.skybox_source)
      merged_env.skybox_source = e.skybox_source;
    if(e.irradiance_map.native_handle)
      merged_env.irradiance_map = e.irradiance_map;
    if(e.prefiltered_map.native_handle)
      merged_env.prefiltered_map = e.prefiltered_map;
    if(e.brdf_lut.native_handle)
      merged_env.brdf_lut = e.brdf_lut;

    // Numeric parameter groups overlay only when the producer set the
    // corresponding bit, otherwise the merged value (default or an
    // earlier contributor's) stays.
    if(e.params_set & scene_environment::params_exposure_gamma)
    {
      merged_env.exposure = e.exposure;
      merged_env.gamma = e.gamma;
      merged_env.params_set |= scene_environment::params_exposure_gamma;
    }
    if(e.params_set & scene_environment::params_ambient)
    {
      merged_env.ambient_color[0] = e.ambient_color[0];
      merged_env.ambient_color[1] = e.ambient_color[1];
      merged_env.ambient_color[2] = e.ambient_color[2];
      merged_env.ambient_intensity = e.ambient_intensity;
      merged_env.params_set |= scene_environment::params_ambient;
    }
    if(e.params_set & scene_environment::params_fog)
    {
      merged_env.fog = e.fog;
      merged_env.params_set |= scene_environment::params_fog;
    }
    if(e.params_set & scene_environment::params_render_target_size)
    {
      merged_env.render_target_size[0] = e.render_target_size[0];
      merged_env.render_target_size[1] = e.render_target_size[1];
      merged_env.params_set |= scene_environment::params_render_target_size;
    }

    // Dedup roots by shared_ptr identity. Two contributors that share a
    // root (Y-shaped wiring of the same upstream into multiple cables,
    // or a SceneGroup that received the same scene on more than one of
    // its input slots) would otherwise push the same root twice — and
    // every primitive_cloud / mesh / light reachable through it would
    // be visited and uploaded N times by the downstream preprocessor.
    if(s.state->roots)
      for(auto& root : *s.state->roots)
        if(root && seen_roots.insert(root.get()).second)
          merged_roots->push_back(root);

    if(s.state->materials && !s.state->materials->empty())
    {
      materials_contributors++;
      single_materials = &s.state->materials;
    }

    if(s.state->animations && !s.state->animations->empty())
    {
      animations_contributors++;
      single_animations = &s.state->animations;
    }

    if(s.state->cameras && !s.state->cameras->empty())
    {
      cameras_contributors++;
      single_cameras = &s.state->cameras;
    }

    if(s.state->collections && !s.state->collections->empty())
    {
      collections_contributors++;
      single_collections = &s.state->collections;
    }

    if(active_camera_id.value == 0 && s.state->active_camera_id.value != 0)
      active_camera_id = s.state->active_camera_id;

    max_version = std::max(max_version, s.state->version);
    max_time = std::max(max_time, s.state->time_seconds);
  }

  auto merged = std::make_shared<scene_state>();
  merged->roots = std::move(merged_roots);

  if(materials_contributors == 0)
  {
    merged->materials = nullptr;
  }
  else if(materials_contributors == 1)
  {
    merged->materials = *single_materials;
  }
  else
  {
    auto merged_materials
        = std::make_shared<std::vector<material_component_ptr>>();
    for(auto& s : scenes)
      if(s.state && s.state->materials)
        for(auto& mat : *s.state->materials)
          if(mat && seen_materials.insert(mat.get()).second)
            merged_materials->push_back(mat);
    merged->materials = std::move(merged_materials);
  }

  if(animations_contributors == 0)
  {
    merged->animations = nullptr;
  }
  else if(animations_contributors == 1)
  {
    merged->animations = *single_animations;
  }
  else
  {
    auto merged_animations
        = std::make_shared<std::vector<animation_component_ptr>>();
    for(auto& s : scenes)
      if(s.state && s.state->animations)
        for(auto& anim : *s.state->animations)
          if(anim && seen_animations.insert(anim.get()).second)
            merged_animations->push_back(anim);
    merged->animations = std::move(merged_animations);
  }

  if(cameras_contributors == 0)
  {
    merged->cameras = nullptr;
  }
  else if(cameras_contributors == 1)
  {
    merged->cameras = *single_cameras;
  }
  else
  {
    auto merged_cameras = std::make_shared<std::vector<camera_component_ptr>>();
    for(auto& s : scenes)
      if(s.state && s.state->cameras)
        for(auto& cam : *s.state->cameras)
          if(cam && seen_cameras.insert(cam.get()).second)
            merged_cameras->push_back(cam);
    merged->cameras = std::move(merged_cameras);
  }

  if(collections_contributors == 0)
  {
    merged->collections = nullptr;
  }
  else if(collections_contributors == 1)
  {
    merged->collections = *single_collections;
  }
  else
  {
    auto merged_collections
        = std::make_shared<std::vector<scene_collection_ptr>>();
    for(auto& s : scenes)
      if(s.state && s.state->collections)
        for(auto& c : *s.state->collections)
          if(c && seen_collections.insert(c.get()).second)
            merged_collections->push_back(c);
    merged->collections = std::move(merged_collections);
  }

  merged->active_camera_id = active_camera_id;
  merged->environment = std::move(merged_env);
  merged->version = max_version + 1;
  merged->time_seconds = max_time;

  return scene_spec{std::move(merged)};
}
}
