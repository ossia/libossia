#pragma once
#include <ossia/ossia.hpp>

namespace ossia
{
struct TestUtils
{
  ossia::net::generic_device device{std::make_unique<ossia::net::local_protocol>(), "test"};
  ossia::net::address_base* impulse_addr = device.createChild("impulse")->createAddress(val_type::IMPULSE);
  ossia::net::address_base* bool_addr = device.createChild("bool")->createAddress(val_type::BOOL);
  ossia::net::address_base* int_addr = device.createChild("int")->createAddress(val_type::INT);
  ossia::net::address_base* float_addr = device.createChild("float")->createAddress(val_type::FLOAT);
  ossia::net::address_base* char_addr = device.createChild("char")->createAddress(val_type::CHAR);
  ossia::net::address_base* string_addr = device.createChild("string")->createAddress(val_type::STRING);
  ossia::net::address_base* vec2f_addr = device.createChild("vec2f")->createAddress(val_type::VEC2F);
  ossia::net::address_base* vec3f_addr = device.createChild("vec3f")->createAddress(val_type::VEC3F);
  ossia::net::address_base* vec4f_addr = device.createChild("vec4f")->createAddress(val_type::VEC4F);
  ossia::net::address_base* tuple_addr = device.createChild("tuple")->createAddress(val_type::TUPLE);
};
}
