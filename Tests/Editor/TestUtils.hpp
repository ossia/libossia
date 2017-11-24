#pragma once
#include <ossia/ossia.hpp>
#include <ossia-qt/js_utilities.hpp>
#include <QMetaType>
#include <QTest>
namespace ossia
{
struct TestDevice
{
  ossia::net::generic_device device{std::make_unique<ossia::net::multiplex_protocol>(), "test"};
  ossia::net::parameter_base* impulse_addr = device.create_child("impulse")->create_parameter(val_type::IMPULSE);
  ossia::net::parameter_base* bool_addr = device.create_child("bool")->create_parameter(val_type::BOOL);
  ossia::net::parameter_base* int_addr = device.create_child("int")->create_parameter(val_type::INT);
  ossia::net::parameter_base* float_addr = device.create_child("float")->create_parameter(val_type::FLOAT);
  ossia::net::parameter_base* char_addr = device.create_child("char")->create_parameter(val_type::CHAR);
  ossia::net::parameter_base* string_addr = device.create_child("string")->create_parameter(val_type::STRING);
  ossia::net::parameter_base* vec2f_addr = device.create_child("vec2f")->create_parameter(val_type::VEC2F);
  ossia::net::parameter_base* vec3f_addr = device.create_child("vec3f")->create_parameter(val_type::VEC3F);
  ossia::net::parameter_base* vec4f_addr = device.create_child("vec4f")->create_parameter(val_type::VEC4F);
  ossia::net::parameter_base* tuple_addr = device.create_child("tuple")->create_parameter(val_type::LIST);
};
struct TestDeviceRef
{
  ossia::net::generic_device& device;
  ossia::net::parameter_base* impulse_addr = device.create_child("impulse")->create_parameter(val_type::IMPULSE);
  ossia::net::parameter_base* bool_addr = device.create_child("bool")->create_parameter(val_type::BOOL);
  ossia::net::parameter_base* int_addr = device.create_child("int")->create_parameter(val_type::INT);
  ossia::net::parameter_base* float_addr = device.create_child("float")->create_parameter(val_type::FLOAT);
  ossia::net::parameter_base* char_addr = device.create_child("char")->create_parameter(val_type::CHAR);
  ossia::net::parameter_base* string_addr = device.create_child("string")->create_parameter(val_type::STRING);
  ossia::net::parameter_base* vec2f_addr = device.create_child("vec2f")->create_parameter(val_type::VEC2F);
  ossia::net::parameter_base* vec3f_addr = device.create_child("vec3f")->create_parameter(val_type::VEC3F);
  ossia::net::parameter_base* vec4f_addr = device.create_child("vec4f")->create_parameter(val_type::VEC4F);
  ossia::net::parameter_base* tuple_addr = device.create_child("tuple")->create_parameter(val_type::LIST);
};
}
namespace QTest {
inline char* toString(const ossia::value &point)
{
    QByteArray ba = QByteArray::fromStdString(ossia::value_to_pretty_string(point));
    return qstrdup(ba.data());
}
inline char* toString(const ossia::token_request& tk)
{
  QString s = "["
              + QString::number(tk.date.impl) + ", "
              + QString::number(tk.position) + ", "
              + QString::number(tk.offset.impl) + "]";
  QByteArray ba = s.toLatin1();
  return qstrdup(ba.data());
}
inline char* toString(const std::string& str)
{
    QByteArray ba = QByteArray::fromStdString(str);
    return qstrdup(ba.data());
}
inline char* toString(const ossia::time_event::status& str)
{
  switch(str)
  {
    case ossia::time_event::status::DISPOSED: return qstrdup("disposed");
    case ossia::time_event::status::HAPPENED: return qstrdup("happened");
    case ossia::time_event::status::NONE: return qstrdup("none");
    case ossia::time_event::status::PENDING: return qstrdup("pending");
  }
  return qstrdup("error");
}
inline char* toString(const ossia::time_value& val)
{
  return QTest::toString(QByteArray::number((qint64)val.impl));
}

inline char* toString(const chobo::small_vector<ossia::token_request, 4>& val)
{
  QString s = "[";
  for(auto& token : val)
    s += toString(token) + QString(", ");
  s += "]";
  QByteArray ba = s.toLatin1();
  return qstrdup(ba.data());
}
}

struct root_scenario
{
  std::shared_ptr<ossia::time_sync> start_node{std::make_shared<ossia::time_sync>()};
  std::shared_ptr<ossia::time_sync> end_node{std::make_shared<ossia::time_sync>()};

  std::shared_ptr<ossia::time_event> start_event{std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *start_node, ossia::expressions::make_expression_true())};
  std::shared_ptr<ossia::time_event> end_event{std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *end_node, ossia::expressions::make_expression_true())};

  std::shared_ptr<ossia::time_interval> interval{ossia::time_interval::create(
            [] (auto&&...) {},
            *start_event, *end_event,
            ossia::time_value(15000), ossia::time_value(15000), ossia::time_value(15000))};
  std::shared_ptr<ossia::scenario> scenario{std::make_shared<ossia::scenario>()};
  ossia::clock clck{*interval};

  root_scenario()
  {
    using namespace ossia;
    start_node->insert(start_node->get_time_events().end(), start_event);
    end_node->insert(end_node->get_time_events().end(), end_event);

    using namespace std::literals;
    clck.set_granularity(50ms);

    interval->add_time_process(scenario);
    auto scen_sn = scenario->get_start_time_sync();
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
inline QDebug operator<<(QDebug d, ossia::time_event::status s)
{
  return d << QTest::toString(s);
}
inline QDebug operator<<(QDebug d, const ossia::token_request&  s)
{
  return d << QTest::toString(s);
}
inline QDebug operator<<(QDebug d, const chobo::small_vector<ossia::token_request, 4>&  s)
{
  return d << QTest::toString(s);
}
