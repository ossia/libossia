#pragma once
#include <ossia/editor/curve/curve_abstract.hpp>
#include <ossia_export.h>
#include <memory>

namespace ossia
{
class value;
/*! \details Behavior value */
struct OSSIA_EXPORT Behavior
{
  std::shared_ptr<curve_abstract> value;

  /*! constructor */
  Behavior(std::shared_ptr<curve_abstract> v) :
      value(std::move(v))
  {
  }

  Behavior(const Behavior& other) = default;
  Behavior(Behavior&& other) = default;
  Behavior& operator=(const Behavior&) = default;
  Behavior& operator=(Behavior&&) = default;

  bool operator== (const ossia::value&) const { return false; }
  bool operator!= (const ossia::value&) const { return false; }
  bool operator>  (const ossia::value&) const { return false; }
  bool operator>= (const ossia::value&) const { return false; }
  bool operator<  (const ossia::value&) const { return false; }
  bool operator<= (const ossia::value&) const { return false; }
};

}
