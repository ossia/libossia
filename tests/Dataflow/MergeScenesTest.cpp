#include <ossia/detail/config.hpp>

#include <ossia/dataflow/geometry_port.hpp>

#include "include_catch.hpp"

#include <array>
#include <memory>

namespace
{
std::shared_ptr<ossia::scene_state> make_state()
{
  auto s = std::make_shared<ossia::scene_state>();
  s->roots = std::make_shared<std::vector<ossia::scene_node_ptr>>();
  return s;
}

ossia::scene_spec merge(const std::shared_ptr<ossia::scene_state>& a,
                        const std::shared_ptr<ossia::scene_state>& b)
{
  std::array<ossia::scene_spec, 2> specs{ossia::scene_spec{a}, ossia::scene_spec{b}};
  return ossia::merge_scenes(specs);
}
}

TEST_CASE("merge_scenes_skeletons", "merge_scenes")
{
  auto shared = std::make_shared<const ossia::skeleton_component>();
  auto own_a = std::make_shared<const ossia::skeleton_component>();
  auto own_b = std::make_shared<const ossia::skeleton_component>();

  auto a = make_state();
  auto b = make_state();

  SECTION("one contributor: its list is reused as is")
  {
    auto list = std::make_shared<const std::vector<ossia::skeleton_component_ptr>>(
        std::vector<ossia::skeleton_component_ptr>{own_a});
    a->skeletons = list;
    auto m = merge(a, b);
    REQUIRE(m.state);
    CHECK(m.state->skeletons == list);
  }

  SECTION("several contributors: concatenated, shared ones once")
  {
    a->skeletons = std::make_shared<const std::vector<ossia::skeleton_component_ptr>>(
        std::vector<ossia::skeleton_component_ptr>{shared, own_a});
    b->skeletons = std::make_shared<const std::vector<ossia::skeleton_component_ptr>>(
        std::vector<ossia::skeleton_component_ptr>{own_b, shared});
    auto m = merge(a, b);
    REQUIRE(m.state->skeletons);
    CHECK(
        *m.state->skeletons
        == std::vector<ossia::skeleton_component_ptr>{shared, own_a, own_b});
  }

  SECTION("none")
  {
    CHECK_FALSE(merge(a, b).state->skeletons);
  }
}

TEST_CASE("merge_scenes_shadow_cascades", "merge_scenes")
{
  int texture{};
  auto a = make_state();
  auto b = make_state();

  a->shadow_cascades.shadow_map_array.native_handle = &texture;
  b->shadow_cascades.cascade_count = 3;
  b->shadow_cascades.shadow_distance = 42.f;

  auto m = merge(a, b);
  CHECK(m.state->shadow_cascades.cascade_count == 3);
  CHECK(m.state->shadow_cascades.shadow_distance == 42.f);
  // The cascades of b render into the array a provides.
  CHECK(m.state->shadow_cascades.shadow_map_array.native_handle == &texture);
}

TEST_CASE("merge_scenes_injections", "merge_scenes")
{
  int x{}, y{}, z{};
  auto a = make_state();
  auto b = make_state();
  a->inject_buffers.push_back({"lights", &x, 16});
  a->inject_buffers.push_back({"extra", &y, 8});
  b->inject_buffers.push_back({"lights", &z, 32});
  a->inject_textures.push_back({"noise", &x});
  b->inject_textures.push_back({"noise", &y});

  auto m = merge(a, b);
  const auto& buffers = m.state->inject_buffers;
  REQUIRE(buffers.size() == 2);
  CHECK(buffers[0].name == "extra");
  CHECK(buffers[1].name == "lights");
  CHECK(buffers[1].native_handle == &z);
  CHECK(buffers[1].byte_size == 32);

  REQUIRE(m.state->inject_textures.size() == 1);
  CHECK(m.state->inject_textures[0].native_handle == &y);
}

TEST_CASE("merge_scenes_variants_and_indices", "merge_scenes")
{
  auto a = make_state();
  auto b = make_state();
  a->version = 4;
  b->version = 9;
  a->dirty_index = 12;
  b->dirty_index = 3;
  b->active_variant_index = 1;
  b->variant_names = {"day", "night"};

  auto m = merge(a, b);
  CHECK(m.state->version == 10);
  CHECK(m.state->dirty_index == 13);
  CHECK(m.state->active_variant_index == 1);
  REQUIRE(m.state->variant_names.size() == 2);
  CHECK(m.state->variant_names[1] == "night");

  // The first contributor that picked a variant wins.
  a->active_variant_index = 0;
  CHECK(merge(a, b).state->active_variant_index == 0);
}
