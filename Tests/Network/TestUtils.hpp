#pragma once

#include <ossia/network/base/device.hpp>
// #include <ossia/network/base/node.hpp>

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
