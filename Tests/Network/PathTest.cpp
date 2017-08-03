// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>
#include <ossia/network/common/path.hpp>

using namespace ossia;
using namespace ossia::net;
using namespace std::placeholders;

class PathTest : public QObject
{
  Q_OBJECT
  void debug(std::vector<ossia::net::node_base*> t)
  {
    for(auto n : t)
      qDebug() << QString::fromStdString(ossia::net::address_string_from_node(*n));
  }

private Q_SLOTS:

  void test_regex()
  {
    {
      std::regex r("b[a-z][a-z]");
      QVERIFY(std::regex_match("bar", r));
    }
    {
      std::regex r("b[a-zA-Z0-9_~().-][a-zA-Z0-9_~().-]");
      QVERIFY(std::regex_match("bar", r));
    }
    {
      std::regex r("b[a-zA-Z0-9_~().-]?[a-zA-Z0-9_~().-]*");
      QVERIFY(std::regex_match("bar", r));
    }
    {
      // std::regex r("b[a-zA-Z0-9_~().-]?[a-zA-Z0-9_~().-]*?");
      // QVERIFY(std::regex_match("bar", r));
    }
  }

  void test_is_pattern()
  {
    QVERIFY(ossia::traversal::is_pattern("/foo*ba?r/"));
    QVERIFY(ossia::traversal::is_pattern("//foo/bar"));
    QVERIFY(!ossia::traversal::is_pattern("/foo/r/"));
  }

  void test_traversal()
  {
    // Note : to allow access to character classes, we have to change :
    // - in expressions, the { } characters for delimitation. Maybe # ?
    // - in adresses, the [ ] characters for [dataspace.unit] or [0].
    // Maybe a space ? foo:/bar 345, foo:/bar color.rgb

    ossia::net::generic_device device1{std::make_unique<ossia::net::multiplex_protocol>(), "test"};
    ossia::net::generic_device device2{std::make_unique<ossia::net::multiplex_protocol>(), "banana"};

    auto& n1 = ossia::net::find_or_create_node(device1, "foo/bar/baz");
    auto& n2 = ossia::net::find_or_create_node(device1, "foo/bar/blop");
    auto& n5 = ossia::net::find_or_create_node(device1, "foo/war/waz");
    auto& n6 = ossia::net::find_or_create_node(device1, "foo/kar/kaz");

    auto& n3 = ossia::net::find_or_create_node(device2, "fizz/baz");
    auto& n4 = ossia::net::find_or_create_node(device2, "foo/baz.2/blop");

    {
      auto p = traversal::make_path("foo/b??" "/b?*"); // separated because trigraphs...
      QVERIFY(bool(p));
      std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
      traversal::apply(*p, vec);
      debug(vec);

      std::vector<ossia::net::node_base*> expected{&n1, &n2};
      QVERIFY(vec == expected);
    }

    {
      auto p = traversal::make_path("//baz");
      QVERIFY(bool(p));
      std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
      traversal::apply(*p, vec);
      std::vector<ossia::net::node_base*> expected{&n1, &n3};
      QVERIFY(vec == expected);
    }

    {
      auto p = traversal::make_path("//baz.*/*");
      QVERIFY(bool(p));
      std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
      traversal::apply(*p, vec);
      std::vector<ossia::net::node_base*> expected{&n4};
      QVERIFY(vec == expected);
    }

    {
      auto p = traversal::make_path("//baz/..");
      QVERIFY(bool(p));
      std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
      traversal::apply(*p, vec);
      std::vector<ossia::net::node_base*> expected{n1.get_parent(), n3.get_parent()};
      QVERIFY(vec == expected);
    }

    {
      auto p = traversal::make_path("//bar/../war/waz");
      QVERIFY(bool(p));
      std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
      traversal::apply(*p, vec);
      std::vector<ossia::net::node_base*> expected{&n5};
      QVERIFY(vec == expected);
    }


    {
      auto p = traversal::make_path("foo/[bw]*/[bw]*");
      QVERIFY(bool(p));
      std::vector<ossia::net::node_base*> vec{&device1.get_root_node(), &device2.get_root_node()};
      traversal::apply(*p, vec);
      debug(vec);
      std::vector<ossia::net::node_base*> expected{&n1, &n2, &n5, &n4};
      QVERIFY(vec == expected);
    }
  }

  void test_traversal_relative()
  {
    ossia::net::generic_device device1{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

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
      QVERIFY(vec == expected);
    }

    {
      auto p = traversal::make_path("../");
      std::vector<ossia::net::node_base*> vec{&bar};
      traversal::apply(*p, vec);
      debug(vec);
      std::vector<ossia::net::node_base*> expected{&foo};
      QVERIFY(vec == expected);
    }

    {
      auto p = traversal::make_path("../bar");
      std::vector<ossia::net::node_base*> vec{&bar};
      traversal::apply(*p, vec);
      debug(vec);
      std::vector<ossia::net::node_base*> expected{&bar};
      QVERIFY(vec == expected);
    }

    {
      auto p = traversal::make_path("//bar.*");
      std::vector<ossia::net::node_base*> vec{&foo};
      traversal::apply(*p, vec);
      debug(vec);
      std::vector<ossia::net::node_base*> expected{&n1, &n2};
      QVERIFY(vec == expected);
    }

    {
      auto p = traversal::make_path("//bar*");
      std::vector<ossia::net::node_base*> vec{&foo};
      traversal::apply(*p, vec);
      debug(vec);
      std::vector<ossia::net::node_base*> expected{&bar, &n1, &n2};
      QVERIFY(vec == expected);
    }

    {
      auto p = traversal::make_path("//bar*");
      std::vector<ossia::net::node_base*> vec{&bar};
      traversal::apply(*p, vec);
      debug(vec);
      std::vector<ossia::net::node_base*> expected{&n1, &n2};
      QVERIFY(vec == expected);
    }

    {
      auto p = traversal::make_path("//bar.*/..");
      std::vector<ossia::net::node_base*> vec{&foo};
      traversal::apply(*p, vec);
      debug(vec);
      std::vector<ossia::net::node_base*> expected{n1.get_parent()};
      QVERIFY(vec == expected);
    }

  }

  void test_match()
  {
    ossia::net::generic_device device1{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

    auto& n1 = ossia::net::find_or_create_node(device1, "foo/bar/baz");
    auto& n2 = ossia::net::find_or_create_node(device1, "foo/bar/blop");
    auto& n4 = ossia::net::find_or_create_node(device1, "foo/baz.2/blop");
    auto& n5 = ossia::net::find_or_create_node(device1, "foo/war/waz");
    auto& n6 = ossia::net::find_or_create_node(device1, "foo/kar/kaz");

    {
      auto p = *traversal::make_path("foo/[bw]*/[bw]*");
      QVERIFY(traversal::match(p, n1));
      QVERIFY(traversal::match(p, n2));
      QVERIFY(traversal::match(p, n4));
      QVERIFY(traversal::match(p, n5));
      QVERIFY(!traversal::match(p, n6));
    }

    {
      auto p = *traversal::make_path("foo/bar/../?ar/?az");
      QVERIFY(traversal::match(p, n1));
      QVERIFY(!traversal::match(p, n2));
      QVERIFY(!traversal::match(p, n4));
      QVERIFY(traversal::match(p, n5));
      QVERIFY(traversal::match(p, n6));
    }

  }
};

QTEST_APPLESS_MAIN(PathTest)

#include "PathTest.moc"
