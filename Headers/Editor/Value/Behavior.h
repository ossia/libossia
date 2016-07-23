#pragma once
#include <Editor/CurveAbstract.h>
#include <ossia_export.h>
#include <memory>

namespace OSSIA
{
class Value;
/*! \details Behavior value */
struct OSSIA_EXPORT Behavior
{
  std::shared_ptr<CurveAbstract> value;

  /*! constructor */
  Behavior(std::shared_ptr<CurveAbstract> v) :
      value(std::move(v))
  {
  }

  Behavior(const Behavior& other) = default;
  Behavior(Behavior&& other) = default;
  Behavior& operator=(const Behavior&) = default;
  Behavior& operator=(Behavior&&) = default;

  virtual ~Behavior();

  /*! equal operator */
  bool operator== (const Value&) const
  { return false; }

  /*! different operator */
  bool operator!= (const Value&) const
  { return false; }

  /*! greater than operator */
  bool operator> (const Value&) const
  { return false; }

  /*! greater than and equal operator */
  bool operator>= (const Value&) const
  { return false; }

  /*! less than operator */
  bool operator< (const Value&) const
  { return false; }

  /*! less than and equal operator */
  bool operator<= (const Value&) const
  { return false; }
};

}
