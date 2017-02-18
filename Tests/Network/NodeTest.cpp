#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>
#include <ossia/network/common/path.hpp>

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


  void test_attributes()
  {
    generic_device dev{std::make_unique<local_protocol>(), "A"};
    auto& n = find_or_create_node(dev, "/main");

    QVERIFY(!get_access_mode(n));
    QVERIFY(!get_bounding_mode(n));
    QVERIFY(!get_domain(n));
    QVERIFY(!clone_value(n).valid());
    QVERIFY(!get_default_value(n));
    QVERIFY(!get_tags(n));
    QVERIFY(!get_refresh_rate(n));
    QVERIFY(!get_value_step_size(n));
    QVERIFY(!get_repetition_filter(n));
    QVERIFY(!get_critical(n));
    QVERIFY(!get_unit(n));
    QVERIFY(!get_priority(n));
    QVERIFY(!get_description(n));
    QVERIFY(!get_extended_type(n));
    QVERIFY(!get_app_name(n));
    QVERIFY(!get_app_version(n));
    QVERIFY(!get_app_creator(n));

    auto a = n.createAddress(ossia::val_type::INT);

    QVERIFY(get_access_mode(n));
    QVERIFY(get_bounding_mode(n));
    QVERIFY(!get_domain(n));
    QVERIFY(clone_value(n).valid());
    QVERIFY(get_value_type(n));
    QCOMPARE(*get_value_type(n), ossia::val_type::INT);

    a->pushValue(6);

    n.set(access_mode_attribute{}, access_mode::GET);
    QVERIFY(get_access_mode(n));
    QCOMPARE(*get_access_mode(n), access_mode::GET);

    n.set(bounding_mode_attribute{}, bounding_mode::FOLD);
    QVERIFY(get_bounding_mode(n));
    QCOMPARE(*get_bounding_mode(n), bounding_mode::FOLD);

    n.set(domain_attribute{}, make_domain(-10, 10));
    QVERIFY(get_domain(n));
    QCOMPARE(get_domain(n), make_domain(-10, 10));

    n.set(default_value_attribute{}, ossia::value(0));
    QVERIFY(get_default_value(n));
    QCOMPARE(*get_default_value(n), ossia::value(0));

    tags the_tags{"fancy", "wow", "1234"};
    n.set(tags_attribute{}, the_tags);
    QVERIFY(get_tags(n));
    QCOMPARE(*get_tags(n), the_tags);

    n.set(refresh_rate_attribute{}, 100);
    QVERIFY(get_refresh_rate(n));
    QCOMPARE(*get_refresh_rate(n), 100);

    n.set(value_step_size_attribute{}, 0.5);
    QVERIFY(get_value_step_size(n));
    QCOMPARE(*get_value_step_size(n), 0.5);

    n.set(repetition_filter_attribute{}, repetition_filter::ON);
    QVERIFY(get_repetition_filter(n));
    QCOMPARE(get_repetition_filter(n), repetition_filter::ON);

    n.set(critical_attribute{}, true);
    QVERIFY(get_critical(n));

    n.set(unit_attribute{}, meter_per_second_u{});
    QVERIFY(get_unit(n));
    QCOMPARE(get_unit(n), ossia::unit_t(meter_per_second_u{}));

    n.set(priority_attribute{}, 50);
    QVERIFY(get_priority(n));
    QCOMPARE(*get_priority(n), 50);

    n.set(description_attribute{}, "Such a fancy node?! Incredible! すごい!!");
    QVERIFY(get_description(n));
    QCOMPARE(*get_description(n), std::string("Such a fancy node?! Incredible! すごい!!"));

    n.set(extended_type_attribute{}, "custom");
    QVERIFY(get_extended_type(n));
    QCOMPARE(*get_extended_type(n), std::string("custom"));

    n.set(app_name_attribute{}, "AppName");
    QVERIFY(get_app_name(n));
    QCOMPARE(*get_app_name(n), std::string("AppName"));

    n.set(app_version_attribute{}, "1.0.0");
    QVERIFY(get_app_version(n));
    QCOMPARE(*get_app_version(n), std::string("1.0.0"));

    n.set(app_creator_attribute{}, "Lelouch vi Brittania");
    QVERIFY(get_app_creator(n));
    QCOMPARE(*get_app_creator(n), std::string("Lelouch vi Brittania"));



  }
};

QTEST_APPLESS_MAIN(NodeTest)

#include "NodeTest.moc"
