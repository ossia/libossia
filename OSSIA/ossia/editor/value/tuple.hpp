#pragma once
#include <ossia_export.h>
#include <initializer_list>
#include <vector>

namespace ossia
{
class value;
/*! \details Tuple value */
struct OSSIA_EXPORT Tuple
{
  using value_type = std::vector<ossia::value>;
  value_type value;

  /*! constructor for an empty tuple */
  Tuple();
  Tuple(const Tuple&) = default;
  Tuple(Tuple&&) = default;
  Tuple& operator=(const Tuple&) = default;
  Tuple& operator=(Tuple&&) = default;

  /*! constructor for one value
   \param const value */
  explicit Tuple(const ossia::value& v);

  /*! constructor for any number of values
  \param const value
  \param const value
  \param ... */
  Tuple(std::initializer_list<ossia::value> v);

  /*! constructor passing a value vector
   \param std::vector<const #Value> value */
  Tuple(const value_type& v);
  Tuple(value_type&& v);

  bool operator== (const ossia::value&) const;
  bool operator!= (const ossia::value&) const;
  bool operator>  (const ossia::value&) const;
  bool operator>= (const ossia::value&) const;
  bool operator<  (const ossia::value&) const;
  bool operator<= (const ossia::value&) const;
};

}
