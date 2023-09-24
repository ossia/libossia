// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/common/value_bounding.hpp>
#include <ossia/network/domain/domain_base.hpp>

#include "include_catch.hpp"

using namespace ossia;

////////////////////////////////
// float domain, float value //
///////////////////////////////

TEST_CASE("Float domain, float value")
{
  auto domain = make_domain_from_minmax({-100.0f}, {0.0f}, ossia::val_type::FLOAT);
  ossia::value v = -12.0f;
  auto filtered = ossia::bound_value(domain, v, ossia::bounding_mode::CLIP);

  REQUIRE(filtered.valid());
  REQUIRE(filtered == -12);
  REQUIRE(filtered.get_type() == v.get_type());
}

////////////////////////////////
// int domain, int value //
///////////////////////////////

TEST_CASE("int domain, int value")
{
  auto domain = make_domain_from_minmax({-100}, {0}, ossia::val_type::INT);
  ossia::value v = -12;
  auto filtered = ossia::bound_value(domain, v, ossia::bounding_mode::CLIP);

  REQUIRE(filtered.valid());
  REQUIRE(filtered == -12);
  REQUIRE(filtered.get_type() == v.get_type());
}

////////////////////////////////
// float domain, int value //
///////////////////////////////

TEST_CASE("Float domain, int value")
{
  auto domain = make_domain_from_minmax({-100.0f}, {0.0f}, ossia::val_type::FLOAT);
  ossia::value v = -12;
  auto filtered = ossia::bound_value(domain, v, ossia::bounding_mode::CLIP);

  REQUIRE(filtered.valid());
  REQUIRE(filtered == -12);
  REQUIRE(filtered.get_type() == v.get_type());
}

TEST_CASE("Float domain with int, int value")
{
  auto domain = make_domain_from_minmax({-100}, {0}, ossia::val_type::FLOAT);
  ossia::value v = -12;
  auto filtered = ossia::bound_value(domain, v, ossia::bounding_mode::CLIP);

  REQUIRE(filtered.valid());
  REQUIRE(filtered == -12);
  REQUIRE(filtered.get_type() == v.get_type());
}

TEST_CASE("Float domain with int&float, int value")
{
  auto domain = make_domain_from_minmax({-100}, {0.0f}, ossia::val_type::FLOAT);
  ossia::value v = -12;
  auto filtered = ossia::bound_value(domain, v, ossia::bounding_mode::CLIP);

  REQUIRE(filtered.valid());
  REQUIRE(filtered == -12);
  REQUIRE(filtered.get_type() == v.get_type());
}

TEST_CASE("Float domain with float&int, int value")
{
  auto domain = make_domain_from_minmax({-100.0}, {0}, ossia::val_type::FLOAT);
  ossia::value v = -12;
  auto filtered = ossia::bound_value(domain, v, ossia::bounding_mode::CLIP);

  REQUIRE(filtered.valid());
  REQUIRE(filtered == -12);
  REQUIRE(filtered.get_type() == v.get_type());
}

////////////////////////////////
// int domain, float value //
///////////////////////////////

TEST_CASE("int domain, float value")
{
  auto domain = make_domain_from_minmax({-100}, {0}, ossia::val_type::INT);
  ossia::value v = -12.0f;
  auto filtered = ossia::bound_value(domain, v, ossia::bounding_mode::CLIP);

  REQUIRE(filtered.valid());
  REQUIRE(filtered == -12);
  REQUIRE(filtered.get_type() == v.get_type());
}

TEST_CASE("int domain with float, float value")
{
  auto domain = make_domain_from_minmax({-100.0f}, {0.0f}, ossia::val_type::INT);
  ossia::value v = -12.0f;
  auto filtered = ossia::bound_value(domain, v, ossia::bounding_mode::CLIP);

  REQUIRE(filtered.valid());
  REQUIRE(filtered == -12);
  REQUIRE(filtered.get_type() == v.get_type());
}

TEST_CASE("int domain with int&float, Float value")
{
  auto domain = make_domain_from_minmax({-100}, {0.0f}, ossia::val_type::INT);
  ossia::value v = -12.0f;
  auto filtered = ossia::bound_value(domain, v, ossia::bounding_mode::CLIP);

  REQUIRE(filtered.valid());
  REQUIRE(filtered == -12);
  REQUIRE(filtered.get_type() == v.get_type());
}

TEST_CASE("Int domain with float&int, Float value")
{
  auto domain = make_domain_from_minmax({-100.0}, {0}, ossia::val_type::INT);
  ossia::value v = -12.0f;
  auto filtered = ossia::bound_value(domain, v, ossia::bounding_mode::CLIP);

  REQUIRE(filtered.valid());
  REQUIRE(filtered == -12);
  REQUIRE(filtered.get_type() == v.get_type());
}
