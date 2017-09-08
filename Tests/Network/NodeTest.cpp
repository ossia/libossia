// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>
#include <ossia/network/common/path.hpp>
#include <ossia-qt/js_utilities.hpp>

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
    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};
    QVERIFY(device.get_name() == "test");

    device.set_name("app");
    QVERIFY(device.get_name() == "app");

    device.create_child("child");
    auto& node = device.children().front();
    QVERIFY(node != nullptr);
    QVERIFY(device.children().size() == 1);

    QVERIFY(node->get_parent() == &device);

    QVERIFY(node->get_name() == "child");

    node->set_name("foo");
    QVERIFY(node->get_name() == "foo");

    QVERIFY(node->get_parameter() == nullptr);

    auto brother = device.create_child("foo");

    QVERIFY(brother->get_name() == "foo.1");
  }

  void test_instances()
  {
      ossia::net::generic_device dev;
      QVERIFY((ossia::net::create_node(dev, "/foo/bar").get_name()) == "bar");
      QVERIFY((ossia::net::create_node(dev, "/foo/bar").get_name()) == "bar.1");
      QVERIFY((ossia::net::create_node(dev, "/foo/bar").get_name()) == "bar.2");
      QVERIFY((ossia::net::create_node(dev, "/foo/bar").get_name()) == "bar.3");

      QVERIFY((ossia::net::create_node(dev, "/foo/baz.0").get_name()) == "baz.0");
      QVERIFY((ossia::net::create_node(dev, "/foo/baz.0").get_name()) == "baz.1");
      QVERIFY((ossia::net::create_node(dev, "/foo/baz.0").get_name()) == "baz.2");
      QVERIFY((ossia::net::create_node(dev, "/foo/baz.0").get_name()) == "baz.3");

      QVERIFY((ossia::net::create_node(dev, "/foo/blop.2").get_name()) == "blop.2");
      QVERIFY((ossia::net::create_node(dev, "/foo/blop").get_name()) == "blop");
      QVERIFY((ossia::net::create_node(dev, "/foo/blop").get_name()) == "blop.3");
      QVERIFY((ossia::net::create_node(dev, "/foo/blop").get_name()) == "blop.4");

      QVERIFY((ossia::net::create_node(dev, "/foo/flop.2").get_name()) == "flop.2");
      QVERIFY((ossia::net::create_node(dev, "/foo/flop.2").get_name()) == "flop.3");
      QVERIFY((ossia::net::create_node(dev, "/foo/flop.2").get_name()) == "flop.4");
      QVERIFY((ossia::net::create_node(dev, "/foo/flop.2").get_name()) == "flop.5");
  }

  /*! test edition functions */
  void test_edition()
  {
    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

    // edit a node and its address and then remove it
    {
      auto node = device.create_child("child");

      auto address = node->create_parameter();
      QVERIFY(node->get_parameter() != nullptr);
      QVERIFY(address == node->get_parameter());

      QVERIFY(node->remove_parameter());
      QVERIFY(node->get_parameter() == nullptr);

      device.remove_child("child");
      QVERIFY(device.children().size() == 0);
    }

    // edit the same node again to see if it have been
    // correctly destroyed and removed from the namespace
    {
      auto node = device.create_child("child");

      QVERIFY(node->get_name() == "child");
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
    ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};

    /* TODO */
  }

  void test_complex_type()
  {

    ossia::net::generic_device device;
    QVERIFY(ossia::try_setup_parameter("time.Hz", device.get_root_node()));
    QVERIFY(ossia::try_setup_parameter("time.hz", device.get_root_node()));
    QVERIFY(ossia::try_setup_parameter("time.Hertz", device.get_root_node()));
    QVERIFY(ossia::try_setup_parameter("time.hertz", device.get_root_node()));
    QVERIFY(ossia::try_setup_parameter("Hertz", device.get_root_node()));
    QVERIFY(ossia::try_setup_parameter("hertz", device.get_root_node()));
    QVERIFY(ossia::try_setup_parameter("Hz", device.get_root_node()));
    QVERIFY(ossia::try_setup_parameter("hz", device.get_root_node()));
  }

  void test_sanitize()
  {
      using namespace std::literals;
      QCOMPARE(QString::fromStdString(sanitize_name("foo")), QString("foo"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo*")), QString("foo_"));
      QCOMPARE(QString::fromStdString(sanitize_name("fo$o$*")), QString("fo_o__"));
      QCOMPARE(QString::fromStdString(sanitize_name("")), QString(""));

      QCOMPARE(QString::fromStdString(sanitize_name("foo"s, {"foo"})), QString("foo.1"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo"s, {"foo", "foo.1"})), QString("foo.2"));

      QCOMPARE(QString::fromStdString(sanitize_name("foo.1"s, {"foo"})), QString("foo.1"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo.1"s, {"foo.1"})), QString("foo.2"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo.1"s, {"foo", "foo.1"})), QString("foo.2"));

      QCOMPARE(QString::fromStdString(sanitize_name("foo.2"s, {"foo"})), QString("foo.2"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo.2"s, {"foo", "foo.1"})), QString("foo.2"));

      QCOMPARE(QString::fromStdString(sanitize_name("foo.3"s, {"foo"})), QString("foo.3"));
      QCOMPARE(QString::fromStdString(sanitize_name("foo.3"s, {"foo", "foo.1"})), QString("foo.3"));


      QVERIFY((ossia::net::sanitize_name("foo"s, {"foo"})) == "foo.1");
      QVERIFY((ossia::net::sanitize_name("foo"s, {"foo", "foo.1"})) == "foo.2");

      QVERIFY((ossia::net::sanitize_name(QString("foo"), {QString("foo")})) == "foo.1");
      QVERIFY((ossia::net::sanitize_name(QString("foo"), {QString("foo"), QString("foo.1")})) == "foo.2");

      QVERIFY((ossia::net::sanitize_name(QString("foo34"), {QString("foo34")})) == "foo34.1");
      QVERIFY((ossia::net::sanitize_name(QString("foo34"), {QString("foo34"), QString("foo34.1")})) == "foo34.2");

      qDebug() <<ossia::net::sanitize_name(QString("State.1"), {QString("State.1")});
      QVERIFY((ossia::net::sanitize_name(QString("State.1"), {QString("State.1")})) == "State.2");
      QVERIFY((ossia::net::sanitize_name(QString("State.1"), {QString("State.1"), QString("State.2")})) == "State.3");
      QVERIFY((ossia::net::sanitize_name(QString("State.1"), {QString("State.1"), QString("State.2")})) == "State.3");


      QVERIFY((ossia::net::sanitize_name(QString("State.2"), {QString("State.1")})) == "State.2");
      QVERIFY((ossia::net::sanitize_name(QString("State.2"), {QString("State.1"), QString("State.2")})) == "State.3");


      const char state1[]{'S', 't', 'a', 't', 'e', '.', '1'};
      QVERIFY((ossia::net::sanitize_name(
                   QString("State.1"),
                    {"TimeSync.0", "Event.0", "State.0", "TimeSync.1", "bogs3tone68", "State.1"})) == "State.2");
      QVERIFY((ossia::net::sanitize_name(
                   QString::fromLatin1(state1, 7),
      {QString::fromLatin1(state1, 7)})) == "State.2");
  }


  void test_attributes()
  {
    generic_device dev{std::make_unique<multiplex_protocol>(), "A"};
    ossia::net::node_base& n = find_or_create_node(dev, "/main");

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

    auto a = n.create_parameter(ossia::val_type::INT);

    QVERIFY((bool)get_access_mode(n));
    QVERIFY((bool)get_bounding_mode(n));
    QVERIFY(!get_domain(n));
    QVERIFY(clone_value(n).valid());
    QVERIFY((bool)get_value_type(n));
    QCOMPARE(*get_value_type(n), ossia::val_type::INT);

    a->push_value(6);

    n.set(access_mode_attribute{}, access_mode::GET);
    QVERIFY((bool)get_access_mode(n));
    QCOMPARE(*get_access_mode(n), access_mode::GET);

    n.set(bounding_mode_attribute{}, bounding_mode::FOLD);
    QVERIFY((bool)get_bounding_mode(n));
    QCOMPARE(*get_bounding_mode(n), bounding_mode::FOLD);

    n.set(domain_attribute{}, make_domain(-10, 10));
    QVERIFY((bool)get_domain(n));
    QCOMPARE(get_domain(n), make_domain(-10, 10));

    n.set(default_value_attribute{}, 0);
    QVERIFY((bool)get_default_value(n));
    QCOMPARE(*get_default_value(n), ossia::value(0));

    n.set(default_value_attribute{}, true);
    QVERIFY((bool)get_default_value(n));
    QCOMPARE(*get_default_value(n), ossia::value(true));

    n.set(default_value_attribute{}, false);
    QVERIFY((bool)get_default_value(n));
    QCOMPARE(*get_default_value(n), ossia::value(false));

    n.set(default_value_attribute{}, "foo");
    QVERIFY((bool)get_default_value(n));
    QCOMPARE(*get_default_value(n), ossia::value("foo"));

    tags the_tags{"fancy", "wow", "1234"};
    n.set(tags_attribute{}, the_tags);
    auto t = n.get_extended_attributes().find("tags");
    auto tg = t != n.get_extended_attributes().end();
    QVERIFY(tg);
    qDebug() << ((ossia::any)t.value()).type().name();
    tags tgs = ossia::any_cast<tags>(t.value());
    QVERIFY(!tgs.empty());
    QVERIFY((tgs == the_tags));


    QVERIFY((bool)get_tags(n));
    QCOMPARE(*get_tags(n), the_tags);

    n.set(refresh_rate_attribute{}, 100);
    QVERIFY((bool)get_refresh_rate(n));
    QCOMPARE(*get_refresh_rate(n), 100);

    n.set(value_step_size_attribute{}, 0.5);
    QVERIFY((bool)get_value_step_size(n));
    QCOMPARE(*get_value_step_size(n), 0.5);

    n.set(repetition_filter_attribute{}, repetition_filter::ON);
    QVERIFY(get_repetition_filter(n));
    QCOMPARE(get_repetition_filter(n), repetition_filter::ON);

    n.set(critical_attribute{}, true);
    QVERIFY(get_critical(n));

    n.set(unit_attribute{}, meter_per_second_u{});
    QVERIFY((bool)get_unit(n));
    QCOMPARE(get_unit(n), ossia::unit_t(meter_per_second_u{}));

    n.set(priority_attribute{}, 50.f);
    QVERIFY((bool)get_priority(n));
    QCOMPARE(*get_priority(n), 50.f);

    n.set(description_attribute{}, "Such a fancy node?! Incredible! すごい!!");
    QVERIFY((bool)get_description(n));
    QCOMPARE(*get_description(n), std::string("Such a fancy node?! Incredible! すごい!!"));

    n.set(extended_type_attribute{}, "custom");
    QVERIFY((bool)get_extended_type(n));
    QCOMPARE(*get_extended_type(n), std::string("custom"));

    n.set(app_name_attribute{}, "AppName");
    QVERIFY((bool)get_app_name(n));
    QCOMPARE(*get_app_name(n), std::string("AppName"));

    n.set(app_version_attribute{}, "1.0.0");
    QVERIFY((bool)get_app_version(n));
    QCOMPARE(*get_app_version(n), std::string("1.0.0"));

    n.set(app_creator_attribute{}, "Lelouch vi Brittania");
    QVERIFY((bool)get_app_creator(n));
    QCOMPARE(*get_app_creator(n), std::string("Lelouch vi Brittania"));
  }


  void test_attributes_2()
  {
    generic_device dev{std::make_unique<multiplex_protocol>(), "A"};
    ossia::net::node_base& n = find_or_create_node(dev, "/main");

    n.create_parameter(ossia::val_type::INT);

    set_access_mode(n, access_mode::GET);
    QVERIFY((bool)get_access_mode(n));
    QCOMPARE(*get_access_mode(n), access_mode::GET);

    set_bounding_mode(n, bounding_mode::FOLD);
    QVERIFY((bool)get_bounding_mode(n));
    QCOMPARE(*get_bounding_mode(n), bounding_mode::FOLD);

    set_domain(n, make_domain(-10, 10));
    QVERIFY((bool)get_domain(n));
    QCOMPARE(get_domain(n), make_domain(-10, 10));

    set_default_value(n, 0);
    QVERIFY((bool)get_default_value(n));
    QCOMPARE(*get_default_value(n), ossia::value(0));

    set_default_value(n, true);
    QVERIFY((bool)get_default_value(n));
    QCOMPARE(*get_default_value(n), ossia::value(true));

    set_default_value(n, false);
    QVERIFY((bool)get_default_value(n));
    QCOMPARE(*get_default_value(n), ossia::value(false));

    set_default_value(n, "foo");
    QVERIFY((bool)get_default_value(n));
    QCOMPARE(*get_default_value(n), ossia::value("foo"));

    tags the_tags{"fancy", "wow", "1234"};
    set_tags(n, the_tags);
    QVERIFY((bool)get_tags(n));
    QCOMPARE(*get_tags(n), the_tags);

    set_refresh_rate(n, 100);
    QVERIFY((bool)get_refresh_rate(n));
    QCOMPARE(*get_refresh_rate(n), 100);

    set_value_step_size(n, 0.5);
    QVERIFY((bool)get_value_step_size(n));
    QCOMPARE(*get_value_step_size(n), 0.5);

    set_repetition_filter(n, repetition_filter::ON);
    QVERIFY(get_repetition_filter(n));
    QCOMPARE(get_repetition_filter(n), repetition_filter::ON);

    set_critical(n, true);
    QVERIFY(get_critical(n));

    set_unit(n, meter_per_second_u{});
    QVERIFY((bool)get_unit(n));
    QCOMPARE(get_unit(n), ossia::unit_t(meter_per_second_u{}));

    set_priority(n, 50.f);
    QVERIFY((bool)get_priority(n));
    QCOMPARE(*get_priority(n), 50.f);

    set_description(n, "Such a fancy node?! Incredible! すごい!!");
    QVERIFY((bool)get_description(n));
    QCOMPARE(*get_description(n), std::string("Such a fancy node?! Incredible! すごい!!"));

    set_extended_type(n, ossia::filesystem_path_type());
    QVERIFY((bool)get_extended_type(n));
    QCOMPARE(*get_extended_type(n), ossia::filesystem_path_type());

    set_app_name(n, "AppName");
    QVERIFY((bool)get_app_name(n));
    QCOMPARE(*get_app_name(n), std::string("AppName"));

    set_app_version(n, "1.0.0");
    QVERIFY((bool)get_app_version(n));
    QCOMPARE(*get_app_version(n), std::string("1.0.0"));

    set_app_creator(n, "Lelouch vi Brittania");
    QVERIFY((bool)get_app_creator(n));
    QCOMPARE(*get_app_creator(n), std::string("Lelouch vi Brittania"));
  }
};

QTEST_APPLESS_MAIN(NodeTest)

#include "NodeTest.moc"
