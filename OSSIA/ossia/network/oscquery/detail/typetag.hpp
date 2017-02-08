#pragma once
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/editor/value/value.hpp>
#include <oscpack/osc/OscTypes.h>
namespace ossia
{
namespace oscquery
{

//! Puts the OSC typetag of a value in a std::string
//! For instance "if", "T", "fTI", etc.
struct osc_type_visitor
{
  std::string& type;
  void operator()()        { type += oscpack::TypeTagValues::NIL_TYPE_TAG; }
  void operator()(impulse) { type += oscpack::TypeTagValues::INFINITUM_TYPE_TAG; }
  void operator()(int v)   { type += oscpack::TypeTagValues::INT32_TYPE_TAG; }
  void operator()(float v) { type += oscpack::TypeTagValues::FLOAT_TYPE_TAG; }
  void operator()(bool v)  { type += v ? oscpack::TypeTagValues::TRUE_TYPE_TAG : oscpack::TypeTagValues::FALSE_TYPE_TAG;}
  void operator()(char v)  { type += oscpack::TypeTagValues::CHAR_TYPE_TAG; }
  void operator()(const std::string& v) {
    // TODO BLOB
    type += oscpack::TypeTagValues::STRING_TYPE_TAG;
  }

  template<std::size_t N>
  void operator()(const std::array<float, N>& vec)
  {
    type += oscpack::TypeTagValues::ARRAY_BEGIN_TYPE_TAG;
    for(std::size_t i = 0; i < N; i++)
    {
      type += oscpack::TypeTagValues::FLOAT_TYPE_TAG;
    }
    type += oscpack::TypeTagValues::ARRAY_END_TYPE_TAG;
  }

  void operator()(const std::vector<ossia::value>& vec)
  {
    type.reserve(type.size() + vec.size() + 2);

    type += oscpack::TypeTagValues::ARRAY_BEGIN_TYPE_TAG;
    for(const auto& sub : vec)
    {
      sub.apply(*this);
    }
    type += oscpack::TypeTagValues::ARRAY_END_TYPE_TAG;
  }

  void operator()(const ossia::Destination& d)
  {
    throw;
  }
};

inline std::string get_osc_typetag_impl(const ossia::net::address_base& addr)
{
  std::string s;
  addr.cloneValue().apply(osc_type_visitor{s});
  return s;
}

inline optional<std::string> get_osc_typetag(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress())
  {
    return get_osc_typetag_impl(*addr);
  }
  else
  {
    return ossia::none;
  }
}

}
}
