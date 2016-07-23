#pragma once
#include <ossia_export.h>
#include <initializer_list>
#include <vector>

namespace OSSIA
{
class Value;
/*! \details Tuple value */
struct OSSIA_EXPORT Tuple
{
  std::vector<Value> value;

  /*! constructor for an empty tuple */
  Tuple();
  Tuple(const Tuple&) = default;
  Tuple(Tuple&&) = default;
  Tuple& operator=(const Tuple&) = default;
  Tuple& operator=(Tuple&&) = default;
  virtual ~Tuple();

  /*! constructor for one value
   \param const value */
  explicit Tuple(const Value& v);

  /*! constructor for any number of values
  \param const value
  \param const value
  \param ... */
  Tuple(std::initializer_list<Value> v);

  /*! constructor passing a value vector
   \param std::vector<const #Value> value */
  Tuple(const std::vector<Value>& v);
  Tuple(std::vector<Value>&& v);

  /*! equal operator */
  bool operator== (const Value&) const;

  /*! different operator */
  bool operator!= (const Value&) const;

  /*! greater than operator */
  bool operator> (const Value&) const;

  /*! greater than and equal operator */
  bool operator>= (const Value&) const;

  /*! less than operator */
  bool operator< (const Value&) const;

  /*! less than and equal operator */
  bool operator<= (const Value&) const;
};

}
