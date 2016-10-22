#pragma once
#include <ossia_export.h>
#include <string>
#include <boost/utility/string_view.hpp>

namespace ossia
{
class value;
using String = std::string;

///**
// * @class String string.hpp ossia/editor/value/value.hpp
// */
//struct OSSIA_EXPORT String
//{
//  std::string value;

//  operator std::string&() { return value; }
//  operator const std::string&() const { return value; }
//  operator boost::string_view() const { return value; }

//  std::size_t size() const { return value.size(); }
//  bool empty() const { return value.empty(); }
//  String(std::string v = {}) : value(std::move(v))
//  {
//  }

//  String(const String&) = default;
//  String(String&&) = default;
//  String& operator=(const String&) = default;
//  String& operator=(String&&) = default;

//  bool operator==(const ossia::value&) const;
//  bool operator!=(const ossia::value&) const;
//  bool operator>(const ossia::value&) const;
//  bool operator>=(const ossia::value&) const;
//  bool operator<(const ossia::value&) const;
//  bool operator<=(const ossia::value&) const;
//};
}
