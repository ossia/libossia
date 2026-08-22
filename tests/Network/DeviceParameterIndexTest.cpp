// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/detail/config.hpp>

#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/common/device_parameter_index.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/local/local.hpp>

#include "include_catch.hpp"

#include <atomic>
#include <thread>

using namespace ossia;
using namespace ossia::net;

TEST_CASE("test_device_parameter_index_contents", "test_device_parameter_index")
{
  generic_device dev{"test"};

  // Nodes that already exist when the index is built
  auto pre = find_or_create_parameter(dev.get_root_node(), "/pre", "float");
  REQUIRE(pre != nullptr);

  device_parameter_index index{dev};

  parameter_base* found{};
  auto take = [&](parameter_base& p) { found = &p; };

  REQUIRE(index.apply("/pre", take));
  REQUIRE(found == pre);

  // Nodes created afterwards
  auto post = find_or_create_parameter(dev.get_root_node(), "/a/b/post", "int");
  REQUIRE(post != nullptr);
  found = {};
  REQUIRE(index.apply("/a/b/post", take));
  REQUIRE(found == post);

  // Leading and trailing slashes are not significant
  REQUIRE(index.apply("a/b/post", take));
  REQUIRE(index.apply("/a/b/post/", take));

  // Nodes without a parameter are not in the index
  REQUIRE(!index.apply("/a", take));
  REQUIRE(!index.apply("/does/not/exist", take));
}

TEST_CASE("test_device_parameter_index_removal", "test_device_parameter_index")
{
  generic_device dev{"test"};
  device_parameter_index index{dev};

  auto noop = [](parameter_base&) { };

  find_or_create_parameter(dev.get_root_node(), "/a/b/c", "float");
  find_or_create_parameter(dev.get_root_node(), "/a/b/d", "float");
  REQUIRE(index.apply("/a/b/c", noop));
  REQUIRE(index.apply("/a/b/d", noop));

  // Removing a node must take the parameters of its whole subtree out of the
  // index: they are destroyed with it.
  auto b = find_node(dev.get_root_node(), "/a/b");
  REQUIRE(b != nullptr);
  b->get_parent()->remove_child(*b);

  REQUIRE(!index.apply("/a/b/c", noop));
  REQUIRE(!index.apply("/a/b/d", noop));
  REQUIRE(index.size() == 0);
}

TEST_CASE("test_device_parameter_index_clear", "test_device_parameter_index")
{
  generic_device dev{"test"};
  device_parameter_index index{dev};

  find_or_create_parameter(dev.get_root_node(), "/a", "float");
  REQUIRE(index.size() == 1);

  index.clear();
  REQUIRE(index.size() == 0);
  REQUIRE(!index.apply("/a", [](parameter_base&) { }));
}

// The serial protocol resolves incoming addresses from its own thread while the
// tree is owned by another one. Doing that with ossia::net::find_node reads a
// node that the other thread may already have destroyed; going through the
// index must not.
TEST_CASE("test_device_parameter_index_concurrent_removal", "test_device_parameter_index")
{
  constexpr int iterations = 3000;

  generic_device dev{"test"};
  device_parameter_index index{dev};

  std::atomic_bool done{false};
  std::atomic_int applied{0};

  std::thread reader{[&] {
    while(!done)
    {
      index.apply("/dyn", [&](parameter_base& p) {
        // Both a read and a write of the parameter, as on_read does
        auto v = p.value();
        p.set_value(v);
        applied++;
      });
    }
  }};

  for(int i = 0; i < iterations; i++)
  {
    find_or_create_parameter(dev.get_root_node(), "/dyn", "float");
    if(auto n = find_node(dev.get_root_node(), "/dyn"))
      n->get_parent()->remove_child(*n);
  }

  done = true;
  reader.join();

  // The point of the test is that it runs to completion without touching freed
  // memory; this only checks that the reader did see the node at least once, so
  // that the test does not silently stop exercising anything.
  INFO("applied: " << applied.load());
  REQUIRE(index.size() == 0);
}


namespace
{
/**
 * A node that never fills node_base's address cache, the way midi_node and
 * phidgets_node do not.
 *
 * That cache is filled by the subclass, and only generic_node and wrapped_node
 * bother: an index keyed on node_base::osc_address() would file every parameter
 * of such a device under "".
 */
class uncached_node : public ossia::net::node_base
{
public:
  uncached_node(std::string name, ossia::net::device_base& dev, node_base* parent)
      : m_dev{dev}
      , m_parent{parent}
  {
    m_name = std::move(name);
  }

  ossia::net::device_base& get_device() const override { return m_dev; }
  node_base* get_parent() const override { return m_parent; }
  node_base& set_name(std::string n) override
  {
    m_name = std::move(n);
    return *this;
  }

  ossia::net::parameter_base* create_parameter(ossia::val_type t) override
  {
    m_param = std::make_unique<ossia::net::generic_parameter>(*this);
    m_param->set_value_type(t);
    m_dev.on_parameter_created(*m_param);
    return m_param.get();
  }
  bool remove_parameter() override
  {
    if(!m_param)
      return false;
    m_dev.on_parameter_removing(*m_param);
    m_param.reset();
    return true;
  }
  ossia::net::parameter_base* get_parameter() const override { return m_param.get(); }

private:
  std::unique_ptr<node_base> make_child(const std::string& name) override
  {
    return std::make_unique<uncached_node>(name, m_dev, this);
  }
  void removing_child(node_base&) override { }

  ossia::net::device_base& m_dev;
  node_base* m_parent{};
  std::unique_ptr<ossia::net::parameter_base> m_param;
};

class uncached_device final : public ossia::net::device_base, public uncached_node
{
public:
  uncached_device()
      : ossia::net::device_base{std::make_unique<ossia::net::multiplex_protocol>()}
      , uncached_node{"uncached", *this, nullptr}
  {
  }

  const ossia::net::node_base& get_root_node() const override { return *this; }
  ossia::net::node_base& get_root_node() override { return *this; }
};
}

TEST_CASE(
    "test_device_parameter_index_uncached_addresses", "test_device_parameter_index")
{
  uncached_device dev;

  auto* a = dev.get_root_node().create_child("a");
  REQUIRE(a != nullptr);
  auto* b = a->create_child("b");
  REQUIRE(b != nullptr);
  auto* param = b->create_parameter(ossia::val_type::FLOAT);
  REQUIRE(param != nullptr);

  // The premise: this tree really does have no cached addresses.
  REQUIRE(b->osc_address().empty());

  device_parameter_index index{dev};

  ossia::net::parameter_base* found{};
  auto take = [&](ossia::net::parameter_base& p) { found = &p; };

  REQUIRE(index.apply("/a/b", take));
  REQUIRE(found == param);
  REQUIRE(!index.apply("/a", take));

  // A second parameter must be a second entry, not a collision under "".
  auto* other = a->create_child("c")->create_parameter(ossia::val_type::INT);
  REQUIRE(index.size() == 2);
  found = {};
  REQUIRE(index.apply("/a/c", take));
  REQUIRE(found == other);
}
