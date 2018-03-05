#pragma once

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/local/local.hpp>
namespace ossia
{

struct TestDevice
{
  TestDevice():
    device{std::make_unique<ossia::net::multiplex_protocol>(), "test"}
  {
    impulse_addr = device.create_child("impulse")->create_parameter(val_type::IMPULSE);
    bool_addr = device.create_child("bool")->create_parameter(val_type::BOOL);
    int_addr = device.create_child("int")->create_parameter(val_type::INT);
    float_addr = device.create_child("float")->create_parameter(val_type::FLOAT);
    char_addr = device.create_child("char")->create_parameter(val_type::CHAR);
    string_addr = device.create_child("string")->create_parameter(val_type::STRING);
    vec2f_addr = device.create_child("vec2f")->create_parameter(val_type::VEC2F);
    vec3f_addr = device.create_child("vec3f")->create_parameter(val_type::VEC3F);
    vec4f_addr = device.create_child("vec4f")->create_parameter(val_type::VEC4F);
    tuple_addr = device.create_child("tuple")->create_parameter(val_type::LIST);

    a = device.create_child("a")->create_parameter(val_type::LIST);
    b = device.create_child("b")->create_parameter(val_type::LIST);
    c = device.create_child("c")->create_parameter(val_type::LIST);
    d = device.create_child("d")->create_parameter(val_type::LIST);

    f1 = device.create_child("f1")->create_parameter(val_type::FLOAT);
    f2 = device.create_child("f2")->create_parameter(val_type::FLOAT);
    f3 = device.create_child("f3")->create_parameter(val_type::FLOAT);
    f4 = device.create_child("f4")->create_parameter(val_type::FLOAT);


    rgb = device.create_child("argb")->create_parameter(val_type::VEC4F); rgb->set_unit(ossia::argb_u{});
    hsv = device.create_child("hsv")->create_parameter(val_type::VEC3F); hsv->set_unit(ossia::hsv_u{});
    rad = device.create_child("rad")->create_parameter(val_type::FLOAT); rad->set_unit(ossia::radian_u{});
    polar = device.create_child("polar")->create_parameter(val_type::VEC2F); polar->set_unit(ossia::polar_u{});
    all_params.push_back(impulse_addr);
    all_params.push_back(bool_addr);
    all_params.push_back(int_addr);
    all_params.push_back(float_addr);
    all_params.push_back(char_addr);
    all_params.push_back(string_addr);
    all_params.push_back(vec2f_addr);
    all_params.push_back(vec3f_addr);
    all_params.push_back(vec4f_addr);
    all_params.push_back(tuple_addr);
    all_params.push_back(a);
    all_params.push_back(b);
    all_params.push_back(c);
    all_params.push_back(d);
    all_params.push_back(rgb);
    all_params.push_back(hsv);
    all_params.push_back(rad);
    all_params.push_back(polar);

    float_params.push_back(float_addr);
    float_params.push_back(f1);
    float_params.push_back(f2);
    float_params.push_back(f3);
    float_params.push_back(f4);
  }

  ossia::net::generic_device device;
  ossia::net::parameter_base* impulse_addr;
  ossia::net::parameter_base* bool_addr;
  ossia::net::parameter_base* int_addr;
  ossia::net::parameter_base* float_addr;
  ossia::net::parameter_base* char_addr;
  ossia::net::parameter_base* string_addr;
  ossia::net::parameter_base* vec2f_addr;
  ossia::net::parameter_base* vec3f_addr;
  ossia::net::parameter_base* vec4f_addr;
  ossia::net::parameter_base* tuple_addr;
  ossia::net::parameter_base* a;
  ossia::net::parameter_base* b;
  ossia::net::parameter_base* c;
  ossia::net::parameter_base* d;
  ossia::net::parameter_base* f1;
  ossia::net::parameter_base* f2;
  ossia::net::parameter_base* f3;
  ossia::net::parameter_base* f4;

  ossia::net::parameter_base* rgb;
  ossia::net::parameter_base* hsv;
  ossia::net::parameter_base* rad;
  ossia::net::parameter_base* polar;

  std::vector<ossia::net::parameter_base*> all_params;
  std::vector<ossia::net::parameter_base*> float_params;
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
