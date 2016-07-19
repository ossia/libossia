#pragma once
#include <Editor/Value/ValueBase.h>
#include <initializer_list>
#include <vector>

namespace OSSIA
{

/*! \details Tuple value */
struct OSSIA_EXPORT Tuple final : public Value
{
  std::vector<Value*> value;

  /*! constructor for an empty tuple */
  Tuple():
      Value{Type::TUPLE}
  {
  }
  Tuple(const Tuple&) = delete;
  virtual ~Tuple();

  /*! Mechanism for building a Tuple with a list of
   * values, to remove the need for spurious memory allocations.
   * Usage : Tuple t{Tuple::ValueInit, Int{2}, Float{3.145}, String{"foo"}};
   */
  struct ValueInit {} ;
  template<typename... Args>
  explicit Tuple(ValueInit, Args&&... args):
      Value{Type::TUPLE}
  {
      value.reserve(sizeof...(args));
      append(std::forward<Args>(args)...);
  }

  void append() { }
  template<typename Arg, typename... Args>
  void append(Arg&& arg, Args&&... args)
  {
      value.push_back(arg.clone());
      append(std::forward<Args>(args)...);
  }

  /*! constructor for one value
   \param const value */
  explicit Tuple(const Value* v):
      Value{Type::TUPLE}
  {
      value.push_back(v->clone());
  }

  /*! constructor for any number of values
  \param const value
  \param const value
  \param ... */
  Tuple(std::initializer_list<const Value*> v):
      Value{Type::TUPLE}
  {
      for (const auto & e : v)
          value.push_back(e->clone());
  }

  /*! constructor passing a value vector
   \param std::vector<const #Value> value */
  Tuple(const std::vector<const Value*>& v):
      Value{Type::TUPLE}
  {
      for (const auto & e : v)
          value.push_back(e->clone());
  }
  Tuple(std::vector<Value*>&& v):
      Value{Type::TUPLE},
      value(std::move(v))
  {
  }

  /*! clone */
  Value * clone() const override;

  /*! equal operator */
  bool operator== (const Value&) const override;

  /*! different operator */
  bool operator!= (const Value&) const override;

  /*! greater than operator */
  bool operator> (const Value&) const override;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const override;

  /*! less than operator */
  bool operator< (const Value&) const override;

  /*! less than and equal operator */
  bool operator<= (const Value&) const override;
};

}
