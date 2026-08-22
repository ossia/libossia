// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/generic/wrapped_parameter.hpp>

using namespace ossia;
using namespace ossia::net;

namespace
{
// Mimics the shape of serial_parameter_data / http_parameter_data /
// ws_generic_client_parameter_data / mapper_parameter_data: a parameter_data
// with an extra payload that the protocol reads back through
// wrapped_parameter::data().
struct test_parameter_data_base
{
  test_parameter_data_base() = default;
  test_parameter_data_base(const test_parameter_data_base&) = default;
  test_parameter_data_base(test_parameter_data_base&&) = default;
  test_parameter_data_base& operator=(const test_parameter_data_base&) = default;
  test_parameter_data_base& operator=(test_parameter_data_base&&) = default;

  std::string request;
};

struct test_parameter_data final
    : public parameter_data
    , public test_parameter_data_base
{
  using base_data_type = test_parameter_data_base;
  test_parameter_data() = default;
  test_parameter_data(const test_parameter_data&) = default;
  test_parameter_data(test_parameter_data&&) = default;
  test_parameter_data& operator=(const test_parameter_data&) = default;
  test_parameter_data& operator=(test_parameter_data&&) = default;

  test_parameter_data(const std::string& name)
      : parameter_data{name}
  {
  }

  bool valid() const noexcept { return !request.empty() || bool(type); }
};

using test_parameter = wrapped_parameter<test_parameter_data>;
using test_node = wrapped_node<test_parameter_data, test_parameter>;

struct test_protocol final : public protocol_base
{
  test_protocol()
      : protocol_base{flags{}}
  {
  }

  bool pull(parameter_base&) override { return false; }
  bool push(const parameter_base& p, const ossia::value&) override
  {
    // This is what all the wrapped_* protocols do with the parameters of their
    // tree; it is only valid if every parameter really is a test_parameter.
    pushed_request = static_cast<const test_parameter&>(p).data().request;
    return true;
  }
  bool push_raw(const full_parameter_data&) override { return false; }
  bool observe(parameter_base&, bool) override { return false; }
  bool update(node_base&) override { return true; }

  std::string pushed_request;
};

using test_device = wrapped_device<test_node, test_protocol>;
}

TEST_CASE("test_wrapped_node_make_child", "test_wrapped_node")
{
  test_device dev{std::make_unique<test_protocol>(), "test"};

  // This is the path taken by the `Device.addNode(address, type)` binding
  // exposed to the QML scripts of the serial / http / websocket / mapper
  // protocols.
  auto param
      = ossia::net::find_or_create_parameter(dev.get_root_node(), "/foo", "float");
  REQUIRE(param != nullptr);
  REQUIRE(param->get_value_type() == ossia::val_type::FLOAT);
  REQUIRE(param->value() == ossia::value{0.f});

  auto node = ossia::net::find_node(dev.get_root_node(), "/foo");
  REQUIRE(node != nullptr);

  // Both the node and its parameter must have the protocol's own type: the
  // protocols static_cast<> them without checking.
  REQUIRE(dynamic_cast<test_node*>(node) != nullptr);
  REQUIRE(dynamic_cast<test_parameter*>(param) != nullptr);
}

TEST_CASE("test_wrapped_node_make_child_recursive", "test_wrapped_node")
{
  test_device dev{std::make_unique<test_protocol>(), "test"};

  auto param
      = ossia::net::find_or_create_parameter(dev.get_root_node(), "/a/b/c", "int");
  REQUIRE(param != nullptr);
  REQUIRE(dynamic_cast<test_parameter*>(param) != nullptr);

  // Intermediate nodes are created through make_child too
  for(auto addr : {"/a", "/a/b", "/a/b/c"})
  {
    auto node = ossia::net::find_node(dev.get_root_node(), addr);
    REQUIRE(node != nullptr);
    REQUIRE(dynamic_cast<test_node*>(node) != nullptr);
  }
}

TEST_CASE("test_wrapped_node_push_created_parameter", "test_wrapped_node")
{
  test_device dev{std::make_unique<test_protocol>(), "test"};
  auto& proto = static_cast<test_protocol&>(dev.get_protocol());

  auto param
      = ossia::net::find_or_create_parameter(dev.get_root_node(), "/foo", "float");
  REQUIRE(param != nullptr);

  // Before the fix this reinterpreted a generic_parameter as a test_parameter.
  param->push_value(1.5f);
  REQUIRE(proto.pushed_request.empty());
}

namespace
{
struct param_counter
{
  int created{};
  void on_parameter_created(const parameter_base&) { created++; }
};
}

TEST_CASE("test_wrapped_node_parameter_created_notified", "test_wrapped_node")
{
  param_counter counter;
  test_device dev{std::make_unique<test_protocol>(), "test"};

  dev.on_parameter_created.connect<&param_counter::on_parameter_created>(&counter);
  auto& created = counter.created;

  auto param
      = ossia::net::find_or_create_parameter(dev.get_root_node(), "/foo", "float");
  REQUIRE(param != nullptr);
  REQUIRE(created == 1);

  // Asking again must not create a second parameter
  auto param2
      = ossia::net::find_or_create_parameter(dev.get_root_node(), "/foo", "float");
  REQUIRE(param2 == param);
  REQUIRE(created == 1);
}
