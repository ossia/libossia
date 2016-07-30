#pragma once
#include <ossia_export.h>
#include <string>

namespace ossia
{
class value;
/*! \details String value */
struct OSSIA_EXPORT String
{
  std::string value;

  /*! constructor
   \param std::string value */
  String(std::string v = {}) :
      value(std::move(v))
  {
  }

  String(const String&) = default;
  String(String&&) = default;
  String& operator=(const String&) = default;
  String& operator=(String&&) = default;

  bool operator== (const ossia::value&) const;
  bool operator!= (const ossia::value&) const;
  bool operator>  (const ossia::value&) const;
  bool operator>= (const ossia::value&) const;
  bool operator<  (const ossia::value&) const;
  bool operator<= (const ossia::value&) const;
};

}
