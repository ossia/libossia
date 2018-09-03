// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <iostream>
#include <ossia/network/common/path.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <regex>

#if defined(OSSIA_QT)
 #include <ossia-qt/js_utilities.hpp>
#endif
using namespace ossia;
using namespace ossia::net;
using namespace std::placeholders;


/*! test life cycle and accessors functions */
TEST_CASE ("test_basic", "test_basic")
{
  ossia::net::generic_device device{"test"};
  REQUIRE(device.get_name() == "test");

  device.set_name("app");
  REQUIRE(device.get_name() == "app");

  device.create_child("child");
  auto& node = device.children().front();
  REQUIRE(node != nullptr);
  REQUIRE(device.children().size() == 1);

  REQUIRE(node->get_parent() == &device);

  REQUIRE(node->get_name() == "child");

  node->set_name("foo");
  REQUIRE(node->get_name() == "foo");

  REQUIRE(node->get_parameter() == nullptr);

  auto brother = device.create_child("foo");

  REQUIRE(brother->get_name() == "foo.1");
}

TEST_CASE ("test_instances", "test_instances")
{
  ossia::net::generic_device dev;
  REQUIRE((ossia::net::create_node(dev, "/foo/bar").get_name()) == "bar");
  REQUIRE((ossia::net::create_node(dev, "/foo/bar").get_name()) == "bar.1");
  REQUIRE((ossia::net::create_node(dev, "/foo/bar").get_name()) == "bar.2");
  REQUIRE((ossia::net::create_node(dev, "/foo/bar").get_name()) == "bar.3");

  REQUIRE((ossia::net::create_node(dev, "/foo/baz.0").get_name()) == "baz.0");
  REQUIRE((ossia::net::create_node(dev, "/foo/baz.0").get_name()) == "baz.1");
  REQUIRE((ossia::net::create_node(dev, "/foo/baz.0").get_name()) == "baz.2");
  REQUIRE((ossia::net::create_node(dev, "/foo/baz.0").get_name()) == "baz.3");

  REQUIRE((ossia::net::create_node(dev, "/foo/blop.2").get_name()) == "blop.2");
  REQUIRE((ossia::net::create_node(dev, "/foo/blop").get_name()) == "blop");
  REQUIRE((ossia::net::create_node(dev, "/foo/blop").get_name()) == "blop.3");
  REQUIRE((ossia::net::create_node(dev, "/foo/blop").get_name()) == "blop.4");

  REQUIRE((ossia::net::create_node(dev, "/foo/flop.2").get_name()) == "flop.2");
  REQUIRE((ossia::net::create_node(dev, "/foo/flop.2").get_name()) == "flop.3");
  REQUIRE((ossia::net::create_node(dev, "/foo/flop.2").get_name()) == "flop.4");
  REQUIRE((ossia::net::create_node(dev, "/foo/flop.2").get_name()) == "flop.5");
}

/*! test edition functions */
TEST_CASE ("test_edition", "test_edition")
{
  ossia::net::generic_device device{"test"};

  // edit a node and its address and then remove it
  {
    auto node = device.create_child("child");

    auto address = node->create_parameter();
    REQUIRE(node->get_parameter() != nullptr);
    REQUIRE(address == node->get_parameter());

    REQUIRE(node->remove_parameter());
    REQUIRE(node->get_parameter() == nullptr);

    device.remove_child("child");
    REQUIRE(device.children().size() == 0);
  }

  // edit the same node again to see if it have been
  // correctly destroyed and removed from the namespace
  {
    auto node = device.create_child("child");

    REQUIRE(node->get_name() == "child");
  }
}

TEST_CASE ("test_path", "test_path")
{
  using namespace ossia::regex_path;
  {
    auto path = device("foo") / "bar" / any_instance("baz");
    REQUIRE(std::regex_match("foo:/bar/baz.2", std::regex(path.address)));
    REQUIRE(std::regex_match("foo:/bar/baz", std::regex(path.address)));
    REQUIRE(!std::regex_match("foo:/bar/baz/", std::regex(path.address)));
    REQUIRE(!std::regex_match("foo:/bob/baz.2", std::regex(path.address)));
    REQUIRE(!std::regex_match("foo:/bob/bim/blurg/baz.2", std::regex(path.address)));
  }

  {
    auto path = device("foo") / any_node() / any_instance("baz");
    REQUIRE(std::regex_match("foo:/bar/baz.2", std::regex(path.address)));
    REQUIRE(std::regex_match("foo:/bob/baz.2", std::regex(path.address)));
    REQUIRE(std::regex_match("foo:/bar/baz", std::regex(path.address)));
    REQUIRE(!std::regex_match("foo:/bar/baz/", std::regex(path.address)));
    REQUIRE(!std::regex_match("foo:/bob/bim/blurg/baz.2", std::regex(path.address)));
  }

  {
    auto path = device("foo") / any_path() / any_instance("baz");
    REQUIRE(std::regex_match("foo:/bar/baz.2", std::regex(path.address)));
    REQUIRE(std::regex_match("foo:/bob/baz.2", std::regex(path.address)));
    REQUIRE(std::regex_match("foo:/bob/bim/blurg/baz.2", std::regex(path.address)));
    REQUIRE(std::regex_match("foo:/bar/baz", std::regex(path.address)));
    REQUIRE(!std::regex_match("foo:/bar/baz/", std::regex(path.address)));
  }

  {
    auto path = device("foo") / any_between{"bob", "bar"} / any_instance("baz");
    REQUIRE(std::regex_match("foo:/bar/baz.2", std::regex(path.address)));
    REQUIRE(std::regex_match("foo:/bob/baz.2", std::regex(path.address)));
    REQUIRE(!std::regex_match("foo:/bin/baz.2", std::regex(path.address)));
    REQUIRE(!std::regex_match("foo:/bob/bim/blurg/baz.2", std::regex(path.address)));
    REQUIRE(std::regex_match("foo:/bar/baz", std::regex(path.address)));
    REQUIRE(!std::regex_match("foo:/bar/baz/", std::regex(path.address)));
  }

  {
    auto path = any_path() / any_instance("baz");
    std::cerr << "regex: " << path;
    auto regex = std::regex(path.address);
    REQUIRE(std::regex_match("foo:/bar/baz.2", regex));
    REQUIRE(std::regex_match("blob:/baz.2", regex));
    REQUIRE(!std::regex_match("bin/baz.2", regex));
    REQUIRE(std::regex_match("foo:/bob/bim/blurg/baz.2", regex));
    REQUIRE(!std::regex_match("foo:/bar/baz/azeaze", regex));
    REQUIRE(!std::regex_match("foo:/bar/baz/", regex));
  }
}

/*! test callback notifications */
TEST_CASE ("test_callback", "test_callback")
{
  ossia::net::generic_device device{"test"};

  /* TODO */
}

TEST_CASE ("test_complex_type", "test_complex_type")
{

  ossia::net::generic_device device;
  REQUIRE(ossia::try_setup_parameter("time.Hz", device.get_root_node()));
  REQUIRE(ossia::try_setup_parameter("time.hz", device.get_root_node()));
  REQUIRE(ossia::try_setup_parameter("time.Hertz", device.get_root_node()));
  REQUIRE(ossia::try_setup_parameter("time.hertz", device.get_root_node()));
  REQUIRE(ossia::try_setup_parameter("Hertz", device.get_root_node()));
  REQUIRE(ossia::try_setup_parameter("hertz", device.get_root_node()));
  REQUIRE(ossia::try_setup_parameter("Hz", device.get_root_node()));
  REQUIRE(ossia::try_setup_parameter("hz", device.get_root_node()));
}

TEST_CASE ("test_sanitize", "test_sanitize")
{
  using namespace std::literals;
  REQUIRE(sanitize_name("foo") == "foo"s);
  REQUIRE(sanitize_name("foo*") == "foo_"s);
  REQUIRE(sanitize_name("fo$o$*") == "fo_o__"s);
  REQUIRE(sanitize_name("") == ""s);

  REQUIRE(sanitize_name("foo"s, {"foo"}) == "foo.1"s);
  REQUIRE(sanitize_name("foo"s, {"foo", "foo.1"}) == "foo.2"s);

  REQUIRE(sanitize_name("foo.1"s, {"foo"}) == "foo.1"s);
  REQUIRE(sanitize_name("foo.1"s, {"foo.1"}) == "foo.2"s);
  REQUIRE(sanitize_name("foo.1"s, {"foo", "foo.1"}) == "foo.2"s);

  REQUIRE(sanitize_name("foo.2"s, {"foo"}) == "foo.2"s);
  REQUIRE(sanitize_name("foo.2"s, {"foo", "foo.1"}) == "foo.2"s);

  REQUIRE(sanitize_name("foo.3"s, {"foo"}) == "foo.3"s);
  REQUIRE(sanitize_name("foo.3"s, {"foo", "foo.1"}) == "foo.3"s);


  REQUIRE((ossia::net::sanitize_name("foo"s, {"foo"})) == "foo.1");
  REQUIRE((ossia::net::sanitize_name("foo"s, {"foo", "foo.1"})) == "foo.2");

#if defined(OSSIA_QT)
  REQUIRE((ossia::net::sanitize_name(QString("foo"), {QString("foo")})) == "foo.1");
  REQUIRE((ossia::net::sanitize_name(QString("foo"), {QString("foo"), QString("foo.1")})) == "foo.2");

  REQUIRE((ossia::net::sanitize_name(QString("foo34"), {QString("foo34")})) == "foo34.1");
  REQUIRE((ossia::net::sanitize_name(QString("foo34"), {QString("foo34"), QString("foo34.1")})) == "foo34.2");

  std::cerr <<ossia::net::sanitize_name(QString("State.1"), {QString("State.1")}).toStdString();
  REQUIRE((ossia::net::sanitize_name(QString("State.1"), {QString("State.1")})) == "State.2");
  REQUIRE((ossia::net::sanitize_name(QString("State.1"), {QString("State.1"), QString("State.2")})) == "State.3");
  REQUIRE((ossia::net::sanitize_name(QString("State.1"), {QString("State.1"), QString("State.2")})) == "State.3");


  REQUIRE((ossia::net::sanitize_name(QString("State.2"), {QString("State.1")})) == "State.2");
  REQUIRE((ossia::net::sanitize_name(QString("State.2"), {QString("State.1"), QString("State.2")})) == "State.3");

  const char state1[]{'S', 't', 'a', 't', 'e', '.', '1'};
  REQUIRE((ossia::net::sanitize_name(
             QString("State.1"),
  {"TimeSync.0", "Event.0", "State.0", "TimeSync.1", "bogs3tone68", "State.1"})) == "State.2");
  REQUIRE((ossia::net::sanitize_name(
             QString::fromLatin1(state1, 7),
  {QString::fromLatin1(state1, 7)})) == "State.2");
#endif
}


TEST_CASE ("test_attributes", "test_attributes")
{
  generic_device dev{"A"};
  ossia::net::node_base& n = find_or_create_node(dev, "/main");

  REQUIRE(!get_access_mode(n));
  REQUIRE(!get_bounding_mode(n));
  REQUIRE(!get_domain(n));
  REQUIRE(!clone_value(n).valid());
  REQUIRE(!get_default_value(n));
  REQUIRE(!get_tags(n));
  REQUIRE(!get_refresh_rate(n));
  REQUIRE(!get_value_step_size(n));
  REQUIRE(!get_repetition_filter(n));
  REQUIRE(!get_critical(n));
  REQUIRE(!get_unit(n));
  REQUIRE(!get_priority(n));
  REQUIRE(!get_description(n));
  REQUIRE(!get_extended_type(n));
  REQUIRE(!get_app_name(n));
  REQUIRE(!get_app_version(n));
  REQUIRE(!get_app_creator(n));

  auto a = n.create_parameter(ossia::val_type::INT);

  REQUIRE((bool)get_access_mode(n));
  REQUIRE((bool)get_bounding_mode(n));
  REQUIRE(!get_domain(n));
  REQUIRE(clone_value(n).valid());
  REQUIRE((bool)get_value_type(n));
  REQUIRE(*get_value_type(n) == ossia::val_type::INT);

  a->push_value(6);

  n.set(access_mode_attribute{}, access_mode::GET);
  REQUIRE((bool)get_access_mode(n));
  REQUIRE(*get_access_mode(n) == access_mode::GET);

  n.set(bounding_mode_attribute{}, bounding_mode::FOLD);
  REQUIRE((bool)get_bounding_mode(n));
  REQUIRE(*get_bounding_mode(n) == bounding_mode::FOLD);

  n.set(domain_attribute{}, make_domain(-10, 10));
  REQUIRE((bool)get_domain(n));
  REQUIRE(get_domain(n) == make_domain(-10, 10));

  n.set(default_value_attribute{}, 0);
  REQUIRE((bool)get_default_value(n));
  REQUIRE(*get_default_value(n) == ossia::value(0));

  n.set(default_value_attribute{}, true);
  REQUIRE((bool)get_default_value(n));
  REQUIRE(*get_default_value(n) == ossia::value(true));

  n.set(default_value_attribute{}, false);
  REQUIRE((bool)get_default_value(n));
  REQUIRE(*get_default_value(n) == ossia::value(false));

  n.set(default_value_attribute{}, "foo");
  REQUIRE((bool)get_default_value(n));
  REQUIRE(*get_default_value(n) == ossia::value("foo"));

  tags the_tags{"fancy", "wow", "1234"};
  n.set(tags_attribute{}, the_tags);
  auto t = n.get_extended_attributes().find("tags");
  auto tg = t != n.get_extended_attributes().end();
  REQUIRE(tg);
  std::cerr << ((ossia::any)t.value()).type().name();
  tags tgs = ossia::any_cast<tags>(t.value());
  REQUIRE(!tgs.empty());
  REQUIRE((tgs == the_tags));


  REQUIRE((bool)get_tags(n));
  REQUIRE(*get_tags(n) == the_tags);

  n.set(refresh_rate_attribute{}, 100);
  REQUIRE((bool)get_refresh_rate(n));
  REQUIRE(*get_refresh_rate(n) == 100);

  n.set(value_step_size_attribute{}, 0.5);
  REQUIRE((bool)get_value_step_size(n));
  REQUIRE(*get_value_step_size(n) == 0.5);

  n.set(repetition_filter_attribute{}, repetition_filter::ON);
  REQUIRE(get_repetition_filter(n));
  REQUIRE(get_repetition_filter(n) == repetition_filter::ON);

  n.set(critical_attribute{}, true);
  REQUIRE(get_critical(n));

  n.set(unit_attribute{}, meter_per_second_u{});
  REQUIRE((bool)get_unit(n));
  REQUIRE(get_unit(n) == ossia::unit_t(meter_per_second_u{}));

  n.set(priority_attribute{}, 50.f);
  REQUIRE((bool)get_priority(n));
  REQUIRE(*get_priority(n) == 50.f);

  n.set(description_attribute{}, "Such a fancy node?! Incredible! すごい!!");
  REQUIRE((bool)get_description(n));
  REQUIRE(*get_description(n) == std::string("Such a fancy node?! Incredible! すごい!!"));

  n.set(extended_type_attribute{}, "custom");
  REQUIRE((bool)get_extended_type(n));
  REQUIRE(*get_extended_type(n) == std::string("custom"));

  n.set(app_name_attribute{}, "AppName");
  REQUIRE((bool)get_app_name(n));
  REQUIRE(*get_app_name(n) == std::string("AppName"));

  n.set(app_version_attribute{}, "1.0.0");
  REQUIRE((bool)get_app_version(n));
  REQUIRE(*get_app_version(n) == std::string("1.0.0"));

  n.set(app_creator_attribute{}, "Lelouch vi Brittania");
  REQUIRE((bool)get_app_creator(n));
  REQUIRE(*get_app_creator(n) == std::string("Lelouch vi Brittania"));
}


TEST_CASE ("test_attributes_2", "test_attributes_2")
{
  generic_device dev{"A"};
  ossia::net::node_base& n = find_or_create_node(dev, "/main");

  n.create_parameter(ossia::val_type::INT);

  set_access_mode(n, access_mode::GET);
  REQUIRE((bool)get_access_mode(n));
  REQUIRE(*get_access_mode(n) == access_mode::GET);

  set_bounding_mode(n, bounding_mode::FOLD);
  REQUIRE((bool)get_bounding_mode(n));
  REQUIRE(*get_bounding_mode(n) == bounding_mode::FOLD);

  set_domain(n, make_domain(-10, 10));
  REQUIRE((bool)get_domain(n));
  REQUIRE(get_domain(n) == make_domain(-10, 10));

  set_default_value(n, 0);
  REQUIRE((bool)get_default_value(n));
  REQUIRE(*get_default_value(n) == ossia::value(0));

  set_default_value(n, true);
  REQUIRE((bool)get_default_value(n));
  REQUIRE(*get_default_value(n) == ossia::value(true));

  set_default_value(n, false);
  REQUIRE((bool)get_default_value(n));
  REQUIRE(*get_default_value(n) == ossia::value(false));

  set_default_value(n, "foo");
  REQUIRE((bool)get_default_value(n));
  REQUIRE(*get_default_value(n) == ossia::value("foo"));

  tags the_tags{"fancy", "wow", "1234"};
  set_tags(n, the_tags);
  REQUIRE((bool)get_tags(n));
  REQUIRE(*get_tags(n) == the_tags);

  set_refresh_rate(n, 100);
  REQUIRE((bool)get_refresh_rate(n));
  REQUIRE(*get_refresh_rate(n) == 100);

  set_value_step_size(n, 0.5);
  REQUIRE((bool)get_value_step_size(n));
  REQUIRE(*get_value_step_size(n) == 0.5);

  set_repetition_filter(n, repetition_filter::ON);
  REQUIRE(get_repetition_filter(n));
  REQUIRE(get_repetition_filter(n) == repetition_filter::ON);

  set_critical(n, true);
  REQUIRE(get_critical(n));

  set_unit(n, meter_per_second_u{});
  REQUIRE((bool)get_unit(n));
  REQUIRE(get_unit(n) == ossia::unit_t(meter_per_second_u{}));

  set_priority(n, 50.f);
  REQUIRE((bool)get_priority(n));
  REQUIRE(*get_priority(n) == 50.f);

  set_description(n, "Such a fancy node?! Incredible! すごい!!");
  REQUIRE((bool)get_description(n));
  REQUIRE(*get_description(n) == std::string("Such a fancy node?! Incredible! すごい!!"));

  set_extended_type(n, ossia::filesystem_path_type());
  REQUIRE((bool)get_extended_type(n));
  REQUIRE(*get_extended_type(n) == ossia::filesystem_path_type());

  set_app_name(n, "AppName");
  REQUIRE((bool)get_app_name(n));
  REQUIRE(*get_app_name(n) == std::string("AppName"));

  set_app_version(n, "1.0.0");
  REQUIRE((bool)get_app_version(n));
  REQUIRE(*get_app_version(n) == std::string("1.0.0"));

  set_app_creator(n, "Lelouch vi Brittania");
  REQUIRE((bool)get_app_creator(n));
  REQUIRE(*get_app_creator(n) == std::string("Lelouch vi Brittania"));
}
