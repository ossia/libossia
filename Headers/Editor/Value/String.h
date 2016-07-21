#pragma once
#include <ossia_export.h>
#include <string>

namespace OSSIA
{
class SafeValue;
/*! \details String value */
struct OSSIA_EXPORT String
{
  std::string value;

  /*! constructor
   \param std::string value */
  String(std::string v = "") :
      value(std::move(v))
  {
  }
  virtual ~String();
  String(const String&) = default;
  String(String&&) = default;
  String& operator=(const String&) = default;
  String& operator=(String&&) = default;

  /*! equal operator */
  bool operator== (const SafeValue&) const;

  /*! different operator */
  bool operator!= (const SafeValue&) const;

  /*! greater than operator */
  bool operator> (const SafeValue&) const;

  /*! greater than and equal operator */
  bool operator>= (const SafeValue&) const;

  /*! less than operator */
  bool operator< (const SafeValue&) const;

  /*! less than and equal operator */
  bool operator<= (const SafeValue&) const;
};

}
