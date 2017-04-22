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

inline QDebug operator<<(QDebug s, const ossia::value& v)
{
    return s << QString::fromStdString(ossia::value_to_pretty_string(v));
}
