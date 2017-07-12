#pragma once
#include <ossia/ossia.hpp>
#include <ossia-qt/js_utilities.hpp>
#include <QMetaType>
#include <QTest>
namespace ossia
{
struct TestUtils
{
  ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};
  ossia::net::address_base* impulse_addr = device.create_child("impulse")->create_address(val_type::IMPULSE);
  ossia::net::address_base* bool_addr = device.create_child("bool")->create_address(val_type::BOOL);
  ossia::net::address_base* int_addr = device.create_child("int")->create_address(val_type::INT);
  ossia::net::address_base* float_addr = device.create_child("float")->create_address(val_type::FLOAT);
  ossia::net::address_base* char_addr = device.create_child("char")->create_address(val_type::CHAR);
  ossia::net::address_base* string_addr = device.create_child("string")->create_address(val_type::STRING);
  ossia::net::address_base* vec2f_addr = device.create_child("vec2f")->create_address(val_type::VEC2F);
  ossia::net::address_base* vec3f_addr = device.create_child("vec3f")->create_address(val_type::VEC3F);
  ossia::net::address_base* vec4f_addr = device.create_child("vec4f")->create_address(val_type::VEC4F);
  ossia::net::address_base* tuple_addr = device.create_child("tuple")->create_address(val_type::TUPLE);
};
}
namespace QTest {
template<>
inline char* toString(const ossia::value &point)
{
    QByteArray ba = QByteArray::fromStdString(ossia::value_to_pretty_string(point));
    return qstrdup(ba.data());
}
}

struct root_scenario
{
  std::shared_ptr<ossia::time_node> start_node{std::make_shared<ossia::time_node>()};
  std::shared_ptr<ossia::time_node> end_node{std::make_shared<ossia::time_node>()};

  std::shared_ptr<ossia::time_event> start_event{std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *start_node, ossia::expressions::make_expression_true())};
  std::shared_ptr<ossia::time_event> end_event{std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *end_node, ossia::expressions::make_expression_true())};

  std::shared_ptr<ossia::time_constraint> constraint{ossia::time_constraint::create(
            [] (auto&&...) {},
            *start_event, *end_event,
            ossia::time_value(15000), ossia::time_value(15000), ossia::time_value(15000))};
  std::shared_ptr<ossia::scenario> scenario{std::make_shared<ossia::scenario>()};
  ossia::clock clck{*constraint};

  root_scenario()
  {
    using namespace ossia;
    start_node->insert(start_node->get_time_events().end(), start_event);
    end_node->insert(end_node->get_time_events().end(), end_event);

    using namespace std::literals;
    clck.set_granularity(50ms);

    constraint->add_time_process(scenario);
    auto scen_sn = scenario->get_start_time_node();
    auto scen_se = std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *scen_sn, ossia::expressions::make_expression_true());
    scen_sn->insert(scen_sn->get_time_events().end(), scen_se);
  }
};

inline
std::ostream& operator<<(std::ostream& s, ossia::time_event::status st)
{
  switch(st)
  {
    case ossia::time_event::status::NONE: s << "none"; break;
    case ossia::time_event::status::PENDING: s << "pending"; break;
    case ossia::time_event::status::HAPPENED: s << "happened"; break;
    case ossia::time_event::status::DISPOSED: s << "disposed"; break;
  }
  return s;
}
