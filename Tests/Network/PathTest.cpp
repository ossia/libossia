// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <regex>
#include <iostream>
#include <ossia/network/common/path.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "TestUtils.hpp"
using namespace ossia;
using namespace ossia::net;
using namespace std::placeholders;

void debug(std::vector<ossia::net::node_base*> t)
{
  for(auto n : t)
    std::cerr << ossia::net::address_string_from_node(*n);
}
std::vector<std::string> to_string(const std::vector<ossia::net::node_base*>& match)
{
  std::vector<std::string> addresses;
  ossia::transform(
        match,
        std::back_inserter(addresses),
        [] (auto n) { return ossia::net::osc_parameter_string(*n); });
  return addresses;
}


TEST_CASE ("test_regex", "test_regex")
{
  {
    std::regex r("b[a-z][a-z]");
    REQUIRE(std::regex_match("bar", r));
  }
  {
    std::regex r("b[a-zA-Z0-9_~().-][a-zA-Z0-9_~().-]");
    REQUIRE(std::regex_match("bar", r));
  }
  {
    std::regex r("b[a-zA-Z0-9_~().-]?[a-zA-Z0-9_~().-]*");
    REQUIRE(std::regex_match("bar", r));
  }
  {
    // std::regex r("b[a-zA-Z0-9_~().-]?[a-zA-Z0-9_~().-]*?");
    // REQUIRE(std::regex_match("bar", r));
  }

  {
    std::regex reg{"\\[\\]"};
    REQUIRE(std::regex_match("[]", reg));
  }
  {
    std::regex reg{"\\[tata\\]"};
    REQUIRE(std::regex_match("[tata]", reg));
  }
  {
    std::regex reg{"\\[[a-z]*\\]"};
    REQUIRE(std::regex_match("[tata]", reg));
  }
  {
    std::regex reg{"\\[[a-zA-Z0-9-]+\\]"};
    REQUIRE(std::regex_match("[ta-ta]", reg));
  }
  {
    std::regex reg{"\\[[a-zA-Z0-9-]+\\]"};

    std::string ex1 = "foo[0-9]b[a-b5-6]ar";
    std::regex_iterator<std::string::iterator> rit ( ex1.begin(), ex1.end(), reg);
    std::regex_iterator<std::string::iterator> rend;
    REQUIRE(rit != rend);
    REQUIRE(rit->str() == "[0-9]");
    ++rit;
    REQUIRE(rit->str() == "[a-b5-6]");
    ++rit;
    REQUIRE(rit == rend);
  }
  {
    std::regex reg{"[a-zA-Z0-9]-[a-zA-Z0-9]"};
    {
      std::string ex = "[0-9]";
      std::regex_iterator<std::string::iterator> rit ( ex.begin(), ex.end(), reg);
      std::regex_iterator<std::string::iterator> rend;

      REQUIRE(rit != rend);
      REQUIRE(rit->str() == "0-9");
    }
    {
      std::string ex = "[a-b5-6xyz]";
      std::regex_iterator<std::string::iterator> rit ( ex.begin(), ex.end(), reg);
      std::regex_iterator<std::string::iterator> rend;

      REQUIRE(rit != rend);
      REQUIRE(rit->str() == "a-b");
      ++rit;
      REQUIRE(rit->str() == "5-6");
    }
  }
  {
    std::regex reg{R"_(\{(-?[0-9]+)\.\.(-?[0-9]+)\})_"};
    {
      std::string ex = "{0..32}";
      std::regex_iterator<std::string::iterator> rit ( ex.begin(), ex.end(), reg);
      std::regex_iterator<std::string::iterator> rend;

      REQUIRE(rit != rend);
      REQUIRE(rit->str(1) == std::string("0"));
      REQUIRE(rit->str(2) == std::string("32"));
    }
    {
      std::string ex = "{-50..50}";
      std::regex_iterator<std::string::iterator> rit ( ex.begin(), ex.end(), reg);
      std::regex_iterator<std::string::iterator> rend;

      REQUIRE(rit != rend);
      REQUIRE(rit->str(1) == "-50");
      REQUIRE(rit->str(2) == "50");
    }
  }

}


TEST_CASE ("test_is_pattern", "test_is_pattern")
{
  REQUIRE(ossia::traversal::is_pattern("/foo*ba?r/"));
  REQUIRE(ossia::traversal::is_pattern("//foo/bar"));
  REQUIRE(!ossia::traversal::is_pattern("/foo/r/"));
}

TEST_CASE ("test_root_only", "test_root_only")
{
  using namespace std::literals;
  auto parts = ossia::net::address_parts("/"sv);
  ossia::traversal::make_path("/"sv);
}

TEST_CASE ("test_traversal", "test_traversal")
{
  // Note : to allow access to character classes, we have to change :
  // - in expressions, the { } characters for delimitation. Maybe # ?
  // - in adresses, the [ ] characters for [dataspace.unit] or [0].
  // Maybe a space ? foo:/bar 345, foo:/bar color.rgb

  ossia::net::generic_device device1{"test"};
  ossia::net::generic_device device2{"banana"};

  auto& n1 = ossia::net::find_or_create_node(device1, "foo/bar/baz");
  auto& n2 = ossia::net::find_or_create_node(device1, "foo/bar/blop");
  auto& n5 = ossia::net::find_or_create_node(device1, "foo/war/waz");
  ossia::net::find_or_create_node(device1, "foo/kar/kaz");

  auto& n3 = ossia::net::find_or_create_node(device2, "fizz/baz");
  auto& n4 = ossia::net::find_or_create_node(device2, "foo/baz.2/blop");

  {
    auto p = traversal::make_path("foo/b??" "/b?*"); // separated because trigraphs...
    REQUIRE(bool(p));
    std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
    traversal::apply(*p, vec);
    debug(vec);

    std::vector<ossia::net::node_base*> expected{&n1, &n2};
    REQUIRE(vec == expected);
  }

  {
    auto p = traversal::make_path("//baz");
    REQUIRE(bool(p));
    std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
    traversal::apply(*p, vec);
    std::vector<ossia::net::node_base*> expected{&n1, &n3};
    REQUIRE(vec == expected);
  }

  {
    auto p = traversal::make_path("//baz.*/*");
    REQUIRE(bool(p));
    std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
    traversal::apply(*p, vec);
    std::vector<ossia::net::node_base*> expected{&n4};
    REQUIRE(vec == expected);
  }

  {
    auto p = traversal::make_path("//baz/..");
    REQUIRE(bool(p));
    std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
    traversal::apply(*p, vec);
    std::vector<ossia::net::node_base*> expected{n1.get_parent(), n3.get_parent()};
    REQUIRE(vec == expected);
  }

  {
    auto p = traversal::make_path("//bar/../war/waz");
    REQUIRE(bool(p));
    std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
    traversal::apply(*p, vec);
    std::vector<ossia::net::node_base*> expected{&n5};
    REQUIRE(vec == expected);
  }


  {
    auto p = traversal::make_path("foo/[bw]*/[bw]*");
    REQUIRE(bool(p));
    std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
    traversal::apply(*p, vec);
    debug(vec);
    std::vector<ossia::net::node_base*> expected{&n1, &n2, &n5, &n4};
    REQUIRE(vec == expected);
  }
}

TEST_CASE ("test_traversal_relative", "test_traversal_relative")
{
  ossia::net::generic_device device1{"test"};

  auto& foo = ossia::net::find_or_create_node(device1, "foo");
  auto& bar= ossia::net::find_or_create_node(device1, "foo/bar");
  auto& n1 = ossia::net::find_or_create_node(device1, "foo/bar/foo/bar.0");
  auto& n2 = ossia::net::find_or_create_node(device1, "foo/bar/foo/bar.1");

  {
    auto p = traversal::make_path("..");
    std::vector<ossia::net::node_base*> vec{&bar};
    traversal::apply(*p, vec);
    debug(vec);
    std::vector<ossia::net::node_base*> expected{&foo};
    REQUIRE(vec == expected);
  }

  {
    auto p = traversal::make_path("../");
    std::vector<ossia::net::node_base*> vec{&bar};
    traversal::apply(*p, vec);
    debug(vec);
    std::vector<ossia::net::node_base*> expected{&foo};
    REQUIRE(vec == expected);
  }

  {
    auto p = traversal::make_path("../bar");
    std::vector<ossia::net::node_base*> vec{&bar};
    traversal::apply(*p, vec);
    debug(vec);
    std::vector<ossia::net::node_base*> expected{&bar};
    REQUIRE(vec == expected);
  }

  {
    auto p = traversal::make_path("//bar.*");
    std::vector<ossia::net::node_base*> vec{&foo};
    traversal::apply(*p, vec);
    debug(vec);
    std::vector<ossia::net::node_base*> expected{&n1, &n2};
    REQUIRE(vec == expected);
  }
  {
    auto p = traversal::make_path("//bar!");
    std::vector<ossia::net::node_base*> vec{&foo};
    traversal::apply(*p, vec);
    debug(vec);
    std::vector<ossia::net::node_base*> expected{&bar, &n1, &n2};
    REQUIRE(vec == expected);
  }

  {
    auto p = traversal::make_path("//bar*");
    std::vector<ossia::net::node_base*> vec{&foo};
    traversal::apply(*p, vec);
    debug(vec);
    std::vector<ossia::net::node_base*> expected{&bar, &n1, &n2};
    REQUIRE(vec == expected);
  }

  {
    auto p = traversal::make_path("//bar*");
    std::vector<ossia::net::node_base*> vec{&bar};
    traversal::apply(*p, vec);
    debug(vec);
    std::vector<ossia::net::node_base*> expected{&n1, &n2};
    REQUIRE(vec == expected);
  }

  {
    auto p = traversal::make_path("//bar.*/..");
    std::vector<ossia::net::node_base*> vec{&foo};
    traversal::apply(*p, vec);
    debug(vec);
    std::vector<ossia::net::node_base*> expected{n1.get_parent()};
    REQUIRE(vec == expected);
  }
}


TEST_CASE ("test_traversal_relative2", "test_traversal_relative2")
{
  ossia::net::generic_device device1{"test"};

  ossia::net::find_or_create_node(device1, "model");
  auto& t1= ossia::net::find_or_create_node(device1, "model/tutu.1");
  auto& t2= ossia::net::find_or_create_node(device1, "model/tutu.2");
  auto& t3= ossia::net::find_or_create_node(device1, "model/tutu.3");

  {
    auto p = traversal::make_path("//tutu!");
    std::vector<ossia::net::node_base*> vec{&device1.get_root_node()};
    traversal::apply(*p, vec);
    debug(vec);
    std::vector<ossia::net::node_base*> expected{&t1, &t2, &t3};
    REQUIRE(vec == expected);
  }
}

TEST_CASE ("test_match", "test_match")
{
  ossia::net::generic_device device1{"test"};

  auto& n1 = ossia::net::find_or_create_node(device1, "foo/bar/baz");
  auto& n2 = ossia::net::find_or_create_node(device1, "foo/bar/blop");
  auto& n4 = ossia::net::find_or_create_node(device1, "foo/baz.2/blop");
  auto& n5 = ossia::net::find_or_create_node(device1, "foo/war/waz");
  auto& n6 = ossia::net::find_or_create_node(device1, "foo/kar/kaz");

  {
    auto p = *traversal::make_path("foo/[bw]*/[bw]*");
    REQUIRE(traversal::match(p, n1));
    REQUIRE(traversal::match(p, n2));
    REQUIRE(traversal::match(p, n4));
    REQUIRE(traversal::match(p, n5));
    REQUIRE(!traversal::match(p, n6));
  }

  {
    auto p = *traversal::make_path("foo/bar/../?ar/?az");
    REQUIRE(traversal::match(p, n1));
    REQUIRE(!traversal::match(p, n2));
    REQUIRE(!traversal::match(p, n4));
    REQUIRE(traversal::match(p, n5));
    REQUIRE(traversal::match(p, n6));
  }

}

TEST_CASE ("test_create", "test_create")
{
  ossia::net::generic_device device1{};

  auto created = ossia::net::create_nodes(device1, "/foo[0-9]/{bar,baz}/{a{b,c,d},e}[x-z]");

  auto addresses = to_string(created);
  REQUIRE(ossia::contains(addresses, "/foo0/bar/abx"));
  REQUIRE(ossia::contains(addresses, "/foo0/bar/abz"));
  REQUIRE(ossia::contains(addresses, "/foo0/bar/acz"));
  REQUIRE(ossia::contains(addresses, "/foo0/bar/ez"));
  REQUIRE(ossia::contains(addresses, "/foo0/baz/adx"));

  REQUIRE(ossia::contains(addresses, "/foo5/bar/abx"));
  REQUIRE(ossia::contains(addresses, "/foo5/bar/abz"));
  REQUIRE(ossia::contains(addresses, "/foo5/bar/acz"));
  REQUIRE(ossia::contains(addresses, "/foo5/bar/ez"));
  REQUIRE(ossia::contains(addresses, "/foo5/baz/adx"));

  REQUIRE(ossia::contains(addresses, "/foo9/bar/abx"));
  REQUIRE(ossia::contains(addresses, "/foo9/bar/abz"));
  REQUIRE(ossia::contains(addresses, "/foo9/bar/acz"));
  REQUIRE(ossia::contains(addresses, "/foo9/bar/ez"));
  REQUIRE(ossia::contains(addresses, "/foo9/baz/adx"));

  REQUIRE(addresses.size() == std::size_t(10*2*4*3));
}

TEST_CASE ("test_create_range", "test_create_range")
{
  ossia::net::generic_device device1{};

  auto created = ossia::net::create_nodes(device1, "/foo.{-2..2}/bar.{5..10..2}");

  auto addresses = to_string(created);
  REQUIRE(ossia::contains(addresses, "/foo.-2/bar.5"));
  REQUIRE(ossia::contains(addresses, "/foo.-2/bar.7"));
  REQUIRE(ossia::contains(addresses, "/foo.-2/bar.9"));
  REQUIRE(ossia::contains(addresses, "/foo.-1/bar.5"));
  REQUIRE(ossia::contains(addresses, "/foo.-1/bar.7"));
  REQUIRE(ossia::contains(addresses, "/foo.-1/bar.9"));
  REQUIRE(ossia::contains(addresses, "/foo.0/bar.5"));
  REQUIRE(ossia::contains(addresses, "/foo.0/bar.7"));
  REQUIRE(ossia::contains(addresses, "/foo.0/bar.9"));
  REQUIRE(ossia::contains(addresses, "/foo.1/bar.5"));
  REQUIRE(ossia::contains(addresses, "/foo.1/bar.7"));
  REQUIRE(ossia::contains(addresses, "/foo.1/bar.9"));
  REQUIRE(ossia::contains(addresses, "/foo.2/bar.5"));
  REQUIRE(ossia::contains(addresses, "/foo.2/bar.7"));
  REQUIRE(ossia::contains(addresses, "/foo.2/bar.9"));

  REQUIRE(!ossia::contains(addresses, "/foo.-2/bar.6"));
  REQUIRE(!ossia::contains(addresses, "/foo.-3/bar.5"));
  REQUIRE(!ossia::contains(addresses, "/foo.-2/bar.10"));
  REQUIRE(addresses.size() == std::size_t(15));
}

TEST_CASE ("test_match_range", "test_match_range")
{
  {
    ossia::net::generic_device device{};
    auto created = ossia::net::create_nodes(device, "/foo.{-2..2}/bar.{5..10..2}");
    std::set<ossia::net::node_base*> created_set(created.begin(), created.end());

    {
      auto match = ossia::net::find_nodes(device, "/foo.{-2..2}/bar.{5..10..2}");
      std::set<ossia::net::node_base*> match_set(match.begin(), match.end());

      REQUIRE(created_set == match_set);
    }

    {
      auto match = ossia::net::find_nodes(device, "/foo.{0..2}/bar.{0..100}");
      auto addresses = to_string(match);

      REQUIRE(ossia::contains(addresses, "/foo.0/bar.5"));
      REQUIRE(ossia::contains(addresses, "/foo.0/bar.7"));
      REQUIRE(ossia::contains(addresses, "/foo.0/bar.9"));
      REQUIRE(ossia::contains(addresses, "/foo.1/bar.5"));
      REQUIRE(ossia::contains(addresses, "/foo.1/bar.7"));
      REQUIRE(ossia::contains(addresses, "/foo.1/bar.9"));
      REQUIRE(ossia::contains(addresses, "/foo.2/bar.5"));
      REQUIRE(ossia::contains(addresses, "/foo.2/bar.7"));
      REQUIRE(ossia::contains(addresses, "/foo.2/bar.9"));
      REQUIRE(addresses.size() == std::size_t(9));
    }
  }

  {
    ossia::net::generic_device device{};
    ossia::net::create_nodes(device, "/foo.{1..50}");

    for(int start = 1; start < 25; start++)
    {
      auto match = ossia::net::find_nodes(device, "/foo.{" + std::to_string(start) + "..25}");
      std::set<ossia::net::node_base*> match_set(match.begin(), match.end());

      auto addresses = to_string(match);
      debug(match);
      for(int i = start; i <= 25; i++) {
        std::cerr << "start " << start << " ; i" << i;
        REQUIRE(ossia::contains(addresses, "/foo." + std::to_string(i)));
      }
      REQUIRE((int)match_set.size() == (25 - start + 1));
    }
  }

}


TEST_CASE ("test_match_instances", "test_match_instances")

{
  ossia::net::generic_device device1{"test"};

  ossia::net::create_node(device1, "foo/bar/baz");
  ossia::net::create_node(device1, "foo/bar/baz");
  ossia::net::create_node(device1, "foo/bar/baz");
  ossia::net::create_node(device1, "foo/bar/baz");


  {
    auto match = ossia::net::find_nodes(device1, "/foo/bar/baz!");
    auto addresses = to_string(match);

    REQUIRE(match.size() == std::size_t(4));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.1"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.2"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.3"));
  }

  ossia::net::create_node(device1, "foo/bar/baz.1.1");
  ossia::net::create_node(device1, "foo/bar/baz.1.1");
  ossia::net::create_node(device1, "foo/bar/baz.1.1");


  {
    auto match = ossia::net::find_nodes(device1, "/foo/bar/baz!");
    auto addresses = to_string(match);

    std::cerr << "MATCH ==> ";
    debug(match);

    REQUIRE(match.size() == std::size_t(4));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.1"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.2"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.3"));
  }

  {
    auto match = ossia::net::find_nodes(device1, "/foo/bar/baz!!");
    auto addresses = to_string(match);

    REQUIRE(match.size() == std::size_t(7));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.1"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.1.1"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.1.2"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.1.3"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.2"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.3"));
  }

  ossia::net::create_node(device1, "foo/bar.1/baz.1");
  ossia::net::create_node(device1, "foo/bar.2/baz.1");

  {
    auto match = ossia::net::find_nodes(device1, "/foo/bar/baz!");
    auto addresses = to_string(match);

    REQUIRE(match.size() == std::size_t(4));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.1"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.2"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.3"));
  }

  {
    auto match = ossia::net::find_nodes(device1, "/foo/bar!/baz!");
    auto addresses = to_string(match);

    REQUIRE(match.size() == std::size_t(6));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.1"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.2"));
    REQUIRE(ossia::contains(addresses, "/foo/bar/baz.3"));

    REQUIRE(ossia::contains(addresses, "/foo/bar.1/baz.1"));
    REQUIRE(ossia::contains(addresses, "/foo/bar.2/baz.1"));
  }

}

TEST_CASE ("test_device", "test_device")
{
  ossia::net::generic_device device1{"test"};

  auto& a1 = ossia::net::create_node(device1, "foo/bar");
  auto& a2 = ossia::net::create_node(device1, "foo/bar");

  ossia::net::generic_device device2{"boo"};

  auto& b1 = ossia::net::create_node(device2, "foo/bar");
  auto& b2 = ossia::net::create_node(device2, "foo/bar");

  auto p = *traversal::make_path("boo:/*/bar.1");
  std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
  traversal::apply(p, vec);

  REQUIRE(vec == (std::vector<ossia::net::node_base*>{&b2}));

}
