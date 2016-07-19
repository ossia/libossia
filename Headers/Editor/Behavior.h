#pragma once
#include <Editor/CurveAbstract.h>
#include <memory>
#include <Editor/Value.h>

namespace OSSIA
{

/*! \details Behavior value */
struct OSSIA_EXPORT Behavior : public Value
{
  std::shared_ptr<CurveAbstract> value;

  /*! constructor */
  Behavior(std::shared_ptr<CurveAbstract> v) :
      Value{Type::BEHAVIOR},
      value(std::move(v))
  {
  }

  Behavior(const Behavior& other) = delete;
  virtual ~Behavior();

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override { return false; }

  /*! different operator */
  bool operator!= (const Value&) const override { return false; }

  /*! greater than operator */
  bool operator> (const Value&) const override { return false; }

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override { return false; }

  /*! less than operator */
  bool operator< (const Value&) const override { return false; }

  /*! less than and equal operator */
  bool operator<= (const Value&) const override { return false; }
};

}
