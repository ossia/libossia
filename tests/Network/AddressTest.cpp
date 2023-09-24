// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../Benchmarks/Random.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/detail/for_each.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/dataspace/detail/dataspace_parse.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include "include_catch.hpp"

#include <iostream>

using namespace ossia;
TEST_CASE("Nodes", "[Nodes]")
{
  ossia::net::generic_device device{"test"};
  auto cld = device.create_child("child");
  auto node = cld->create_parameter();
  REQUIRE(node != nullptr);
  if(node == nullptr)
    return;

  REQUIRE(&node->get_node() == device.children().front().get());
  REQUIRE(&node->get_node().get_device() == &device);

  REQUIRE(node->get_value_type() == val_type::IMPULSE);

  node->set_value_type(val_type::INT);
  REQUIRE(node->get_value_type() == val_type::INT);

  REQUIRE(node->get_access() == ossia::access_mode::BI);

  node->set_access(ossia::access_mode::SET);
  REQUIRE(node->get_access() == ossia::access_mode::SET);

  REQUIRE(!node->get_domain());

  node->set_domain(make_domain(0, 100));
  REQUIRE(node->get_domain() == make_domain(0, 100));

  REQUIRE(node->get_bounding() == ossia::bounding_mode::FREE);

  node->set_bounding(ossia::bounding_mode::CLIP);
  REQUIRE(node->get_bounding() == ossia::bounding_mode::CLIP);

  REQUIRE(node->get_repetition_filter() == repetition_filter::OFF);

  node->set_repetition_filter(repetition_filter::ON);
  REQUIRE(node->get_repetition_filter() == repetition_filter::ON);
}

// TODO add some checks
TEST_CASE("Units", "[Units]")
{
  ossia::for_each_tagged(dataspace_u_list{}, [](auto d_t) {
    using type = typename decltype(d_t)::type;
    ossia::for_each_tagged(type{}, [](auto u_t) {
      using utype = typename decltype(u_t)::type;
      std::cerr << ossia::get_pretty_unit_text(utype{}) << std::endl;
    });
  });
}
/*
// TODO this is a benchmark not a test
TEST_CASE( "Parameters", "[Parameters]")
{
  std::size_t k = 0;
  Random r;
  ossia::net::generic_device device{std::make_unique<ossia::oscquery::oscquery_server_protocol>(6677, 8899), "test"};
  using namespace std::chrono;
  for(int i = 0; i < 5000; i++)
  {
    auto& node = ossia::net::create_node(device.get_root_node(), "/blah");
    auto t0 = steady_clock::now();
    ossia::try_setup_parameter(r.getRandomString(), node);
    auto t1 = steady_clock::now();
    k += duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
  }

  std::cout << "average time: " << double(k) / double(5000.) << std::endl;
}
*/
