#pragma once
#include <Editor/Value/ValueBase.h>
#include <initializer_list>
#include <vector>

namespace OSSIA
{
class SafeValue;
/*! \details Tuple value */
struct OSSIA_EXPORT Tuple final : public Value
{
  std::vector<SafeValue> value;

  /*! constructor for an empty tuple */
  Tuple();
  Tuple(const Tuple&) = default;
  Tuple(Tuple&&) = default;
  Tuple& operator=(const Tuple&) = default;
  Tuple& operator=(Tuple&&) = default;
  virtual ~Tuple();

  /*! constructor for one value
   \param const value */
  explicit Tuple(const SafeValue& v);

  /*! constructor for any number of values
  \param const value
  \param const value
  \param ... */
  Tuple(std::initializer_list<SafeValue> v);

  /*! constructor passing a value vector
   \param std::vector<const #Value> value */
  Tuple(const std::vector<SafeValue>& v);
  Tuple(std::vector<SafeValue>&& v);

  /*! equal operator */
  bool operator== (const SafeValue&) const override;

  /*! different operator */
  bool operator!= (const SafeValue&) const override;

  /*! greater than operator */
  bool operator> (const SafeValue&) const override;

  /*! greater than and equal operator */
  bool operator>= (const SafeValue&) const override;

  /*! less than operator */
  bool operator< (const SafeValue&) const override;

  /*! less than and equal operator */
  bool operator<= (const SafeValue&) const override;
};

}
