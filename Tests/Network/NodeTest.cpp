#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>
#include <ossia/network/base/path.hpp>

using namespace ossia;
using namespace ossia::net;
using namespace std::placeholders;

class NodeTest : public QObject
{
  Q_OBJECT
private Q_SLOTS:

  /*! test life cycle and accessors functions */
  void test_basic()
  {
    ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};
    QVERIFY(device.getName() == "test");

    device.setName("app");
    QVERIFY(device.getName() == "app");

    device.createChild("child");
    auto& node = device.children().front();
    QVERIFY(node != nullptr);
    QVERIFY(device.children().size() == 1);

    QVERIFY(node->getParent() == &device);

    QVERIFY(node->getName() == "child");

    node->setName("foo");
    QVERIFY(node->getName() == "foo");

    QVERIFY(node->getAddress() == nullptr);

    auto brother = device.createChild("foo");

    QVERIFY(brother->getName() == "foo.1");
  }

  /*! test edition functions */
  void test_edition()
  {
    ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};

    // edit a node and its address and then remove it
    {
      auto node = device.createChild("child");

      auto address = node->createAddress();
      QVERIFY(node->getAddress() != nullptr);
      QVERIFY(address == node->getAddress());

      QVERIFY(node->removeAddress());
      QVERIFY(node->getAddress() == nullptr);

      device.removeChild("child");
      QVERIFY(device.children().size() == 0);
    }

    // edit the same node again to see if it have been
    // correctly destroyed and removed from the namespace
    {
      auto node = device.createChild("child");

      QVERIFY(node->getName() == "child");
    }
  }

  void test_path()
  {
    using namespace ossia::regex_path;
    {
      auto path = device("foo") / "bar" / any_instance("baz");
      QVERIFY(std::regex_match("foo:/bar/baz.2", path.regex()));
      QVERIFY(std::regex_match("foo:/bar/baz", path.regex()));
      QVERIFY(!std::regex_match("foo:/bar/baz/", path.regex()));
      QVERIFY(!std::regex_match("foo:/bob/baz.2", path.regex()));
      QVERIFY(!std::regex_match("foo:/bob/bim/blurg/baz.2", path.regex()));
    }

    {
      auto path = device("foo") / any_node() / any_instance("baz");
      QVERIFY(std::regex_match("foo:/bar/baz.2", path.regex()));
      QVERIFY(std::regex_match("foo:/bob/baz.2", path.regex()));
      QVERIFY(std::regex_match("foo:/bar/baz", path.regex()));
      QVERIFY(!std::regex_match("foo:/bar/baz/", path.regex()));
      QVERIFY(!std::regex_match("foo:/bob/bim/blurg/baz.2", path.regex()));
    }

    {
      auto path = device("foo") / any_path() / any_instance("baz");
      QVERIFY(std::regex_match("foo:/bar/baz.2", path.regex()));
      QVERIFY(std::regex_match("foo:/bob/baz.2", path.regex()));
      QVERIFY(std::regex_match("foo:/bob/bim/blurg/baz.2", path.regex()));
      QVERIFY(std::regex_match("foo:/bar/baz", path.regex()));
      QVERIFY(!std::regex_match("foo:/bar/baz/", path.regex()));
    }

    {
      auto path = device("foo") / any_between{"bob", "bar"} / any_instance("baz");
      QVERIFY(std::regex_match("foo:/bar/baz.2", path.regex()));
      QVERIFY(std::regex_match("foo:/bob/baz.2", path.regex()));
      QVERIFY(!std::regex_match("foo:/bin/baz.2", path.regex()));
      QVERIFY(!std::regex_match("foo:/bob/bim/blurg/baz.2", path.regex()));
      QVERIFY(std::regex_match("foo:/bar/baz", path.regex()));
      QVERIFY(!std::regex_match("foo:/bar/baz/", path.regex()));
    }

    {
      auto path = any_path() / any_instance("baz");
      std::cerr << "regex: " << path;
      auto regex = path.regex();
      QVERIFY(std::regex_match("foo:/bar/baz.2", regex));
      QVERIFY(std::regex_match("blob:/baz.2", regex));
      QVERIFY(!std::regex_match("bin/baz.2", regex));
      QVERIFY(std::regex_match("foo:/bob/bim/blurg/baz.2", regex));
      QVERIFY(!std::regex_match("foo:/bar/baz/azeaze", regex));
      QVERIFY(!std::regex_match("foo:/bar/baz/", regex));
    }
  }

  /*! test callback notifications */
  void test_callback()
  {
    ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};

    /* TODO */
  }

  void test_sanitize()
  {
      QCOMPARE(QString::fromStdString(sanitize_name("foo")), QString("foo"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo*")), QString("foo_"));
      QCOMPARE(QString::fromStdString(sanitize_name("fo$o$*")), QString("fo_o__"));
      QCOMPARE(QString::fromStdString(sanitize_name("")), QString(""));

      QCOMPARE(QString::fromStdString(sanitize_name("foo", {"foo"})), QString("foo.1"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo", {"foo", "foo.1"})), QString("foo.2"));

      QCOMPARE(QString::fromStdString(sanitize_name("foo.1", {"foo"})), QString("foo.1"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo.1", {"foo", "foo.1"})), QString("foo.2"));

      QCOMPARE(QString::fromStdString(sanitize_name("foo.2", {"foo"})), QString("foo.2"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo.2", {"foo", "foo.1"})), QString("foo.2"));

      QCOMPARE(QString::fromStdString(sanitize_name("foo.3", {"foo"})), QString("foo.3"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo.3", {"foo", "foo.1"})), QString("foo.3"));
  }

};

QTEST_APPLESS_MAIN(NodeTest)

#include "NodeTest.moc"
