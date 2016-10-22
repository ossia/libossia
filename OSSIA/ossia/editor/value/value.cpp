#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_comparison.hpp>
#include <ossia/editor/value/value_algorithms.hpp>
#include <ossia/editor/value/value_traits.hpp>
#include <ossia/editor/value/detail/value_conversion_impl.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/detail/logger.hpp>
#include <sstream>

namespace ossia
{


template
Impulse convert<Impulse>(const ossia::value& val);
template
int convert<int>(const ossia::value& val);
template
float convert<float>(const ossia::value& val);
template
double convert<double>(const ossia::value& val);
template
bool convert<bool>(const ossia::value& val);
template
char convert<char>(const ossia::value& val);
template
std::string convert<std::string>(const ossia::value& val);
template
std::vector<ossia::value> convert<std::vector<ossia::value>>(const ossia::value& val);
template
std::array<float, 2> convert<std::array<float, 2>>(const ossia::value& val);
template
std::array<float, 3> convert<std::array<float, 3>>(const ossia::value& val);
template
std::array<float, 4> convert<std::array<float, 4>>(const ossia::value& val);


template
std::array<float, 2> convert<std::array<float, 2>>(const ossia::Tuple& val);
template
std::array<float, 3> convert<std::array<float, 3>>(const ossia::Tuple& val);
template
std::array<float, 4> convert<std::array<float, 4>>(const ossia::Tuple& val);

/*
bool Bool::operator==(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::equal_to<>{});
}

bool Bool::operator!=(const ossia::value& v) const
{
  return !comparisons::NumericValue::apply(*this, v, std::equal_to<>{});
}

bool Bool::operator>(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::greater<>{});
}

bool Bool::operator>=(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::greater_equal<>{});
}

bool Bool::operator<(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::less<>{});
}

bool Bool::operator<=(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::less_equal<>{});
}

bool Int::operator==(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::equal_to<>{});
}

bool Int::operator!=(const ossia::value& v) const
{
  return !comparisons::NumericValue::apply(*this, v, std::equal_to<>{});
}

bool Int::operator>(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::greater<>{});
}

bool Int::operator>=(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::greater_equal<>{});
}

bool Int::operator<(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::less<>{});
}

bool Int::operator<=(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::less_equal<>{});
}

bool Float::operator==(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::equal_to<>{});
}

bool Float::operator!=(const ossia::value& v) const
{
  return !comparisons::NumericValue::apply(*this, v, std::equal_to<>{});
}

bool Float::operator>(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::greater<>{});
}

bool Float::operator>=(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::greater_equal<>{});
}

bool Float::operator<(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::less<>{});
}

bool Float::operator<=(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::less_equal<>{});
}

bool Char::operator==(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::equal_to<>{});
}

bool Char::operator!=(const ossia::value& v) const
{
  return !comparisons::NumericValue::apply(*this, v, std::equal_to<>{});
}

bool Char::operator>(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::greater<>{});
}

bool Char::operator>=(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::greater_equal<>{});
}

bool Char::operator<(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::less<>{});
}

bool Char::operator<=(const ossia::value& v) const
{
  return comparisons::NumericValue::apply(*this, v, std::less_equal<>{});
}

bool String::operator==(const ossia::value& v) const
{
  return comparisons::StringValue::apply(*this, v, std::equal_to<>{});
}

bool String::operator!=(const ossia::value& v) const
{
  return !comparisons::StringValue::apply(*this, v, std::equal_to<>{});
}

bool String::operator>(const ossia::value& v) const
{
  return comparisons::StringValue::apply(*this, v, std::greater<>{});
}

bool String::operator>=(const ossia::value& v) const
{
  return comparisons::StringValue::apply(*this, v, std::greater_equal<>{});
}

bool String::operator<(const ossia::value& v) const
{
  return comparisons::StringValue::apply(*this, v, std::less<>{});
}

bool String::operator<=(const ossia::value& v) const
{
  return comparisons::StringValue::apply(*this, v, std::less_equal<>{});
}

Tuple::Tuple() = default;
Tuple::Tuple(std::initializer_list<ossia::value> v) : value(v)
{
}

Tuple::Tuple(const std::vector<ossia::value>& v) : value(v)
{
}

Tuple::Tuple(std::vector<ossia::value>&& v) : value(std::move(v))
{
}

Tuple::Tuple(const ossia::value& v)
{
  value.push_back(v);
}

bool Tuple::operator==(const ossia::value& v) const
{
  return comparisons::TupleValue::apply(*this, v, std::equal_to<>{});
}

bool Tuple::operator!=(const ossia::value& v) const
{
  return !comparisons::TupleValue::apply(*this, v, std::equal_to<>{});
}

bool Tuple::operator>(const ossia::value& v) const
{
  return comparisons::TupleValue::apply(*this, v, std::greater<>{});
}

bool Tuple::operator>=(const ossia::value& v) const
{
  return comparisons::TupleValue::apply(*this, v, std::greater_equal<>{});
}

bool Tuple::operator<(const ossia::value& v) const
{
  return comparisons::TupleValue::apply(*this, v, std::less<>{});
}

bool Tuple::operator<=(const ossia::value& v) const
{
  return comparisons::TupleValue::apply(*this, v, std::less_equal<>{});
}
*/
Destination::Destination(const Destination& other) = default;
Destination::Destination(Destination&& other) = default;

Destination& Destination::operator=(const Destination&) = default;
Destination& Destination::operator=(Destination&&) = default;

Destination::Destination(ossia::net::address_base& v) : value(v)
{
  // TODO should we also copy the unit of the address ?
}

Destination::Destination(ossia::net::address_base& v, destination_index idx)
    : value(v), index(std::move(idx))
{
  // TODO should we also copy the unit of the address ?
}

Destination::Destination(ossia::net::address_base& v, destination_index idx, const ossia::unit_t& u)
    : value(v), index(std::move(idx)), unit{u}
{
}

Destination::Destination(net::address_base& v, const unit_t&u)
  : value(v), unit{u}
{

}

bool Destination::operator==(const ossia::value& v) const
{
  throw;
  // TODO return comparisons::DestinationValue::apply(*this, v, std::equal_to<>{});
}

bool Destination::operator!=(const ossia::value& v) const
{
  throw;
  // return !comparisons::DestinationValue::apply(*this, v, std::equal_to<>{});
}

bool Destination::operator>(const ossia::value& v) const
{
  throw;
  // return comparisons::DestinationValue::apply(*this, v, std::greater<>{});
}

bool Destination::operator>=(const ossia::value& v) const
{
  throw;
  /*
  return comparisons::DestinationValue::apply(
      *this, v, std::greater_equal<>{});
  */
}

bool Destination::operator<(const ossia::value& v) const
{
  throw;
  //return comparisons::DestinationValue::apply(*this, v, std::less<>{});
}

bool Destination::operator<=(const ossia::value& v) const
{
  throw;
  //return comparisons::DestinationValue::apply(*this, v, std::less_equal<>{});
}

bool operator==(const Destination& lhs, const Destination& rhs)
{
  throw;
  //return lhs.value == rhs.value && lhs.index == rhs.index;
}

bool operator!=(const Destination& lhs, const Destination& rhs)
{
  throw;
  //return lhs.value != rhs.value || lhs.index != rhs.index;
}

bool operator==(const Destination& lhs, const ossia::net::address_base& rhs)
{
  throw;
  //return lhs.value == rhs && lhs.index.empty();
}

bool operator!=(const Destination& lhs, const ossia::net::address_base& rhs)
{
  throw;
  //return lhs.value != rhs || !lhs.index.empty();
}


std::string to_pretty_string(const destination_index& index)
{
  std::string str;

  const int n = index.size();
  if(n > 0)
  {
    str += "[";
    str += std::to_string(index[0]);

    for(int i = 1; i < n; i++)
    {
      str += ", " + std::to_string(index[i]);
    }
    str += "]";
  }

  return str;
}

std::string to_pretty_string(const Destination& d)
{
  return ossia::net::address_string_from_node(d.value.get()) + to_pretty_string(d.index);
}

template <typename Comparator>
struct value_comparison_visitor
{
  const value& rhs;
  template <typename T>
  bool operator()(const T& lhs)
  {
    assert(false);
    return false;
    // TODO
    // return Comparator{}(lhs, rhs);
  }
};

bool value::operator==(const value& rhs) const
{
  if(v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor<std::equal_to<>>{rhs}, v);
  }
  else if(!v && !rhs.v)
  {
    return true;
  }
  return false;
}

bool value::operator!=(const value& rhs) const
{
  if(v && rhs.v)
  {
    return eggs::variants::apply(
        value_comparison_visitor<std::not_equal_to<>>{rhs}, v);
  }
  else if(!v && !rhs.v)
  {
    return false;
  }
  return true;
}

bool value::operator>(const value& rhs) const
{
  if(v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor<std::greater<>>{rhs}, v);
  }
  else
  {
    return false;
  }
}

bool value::operator>=(const value& rhs) const
{
  if(v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor<std::greater_equal<>>{rhs}, v);
  }
  else if(!v && !rhs.v)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool value::operator<(const value& rhs) const
{
  if(v && rhs.v)
  {
    return eggs::variants::apply(value_comparison_visitor<std::less<>>{rhs}, v);
  }
  else
  {
    return false;
  }
}

bool value::operator<=(const value& rhs) const
{
  if(v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor<std::less_equal<>>{rhs}, v);
  }
  else if(!v && !rhs.v)
  {
    return true;
  }
  else
  {
    return false;
  }
}

namespace
{

static void getTupleAsString(const ossia::Tuple& tuple, fmt::MemoryWriter&);
struct value_prettyprint_visitor
{
  fmt::MemoryWriter& s;

  void operator()(Impulse) const
  {
    s << "impulse";
  }
  void operator()(Int i) const
  {
    s << "int: " << i;
  }
  void operator()(Float f) const
  {
    s << "float: " << f;
  }
  void operator()(Bool b) const
  {
    s << "bool: " << b;
  }
  void operator()(Char c) const
  {
    s << "char: " << c;
  }
  void operator()(const String& str) const
  {
    s << "string: " << str;
  }
  void operator()(Vec2f vec) const
  {
    s << "vec2f: [" << vec[0] << " " << vec[1] << "]";
  }
  void operator()(Vec3f vec) const
  {
    s << "vec3f: [" << vec[0] << " " << vec[1] << " " << vec[2] << "]";
  }
  void operator()(Vec4f vec) const
  {
    s << "vec4f: [" << vec[0] << " " << vec[1] << " " << vec[2] << " " << vec[3] << "]";
  }
  void operator()(const Destination& d) const
  {
    s << "destination" << ossia::net::address_string_from_node(d.value);
    if(d.unit)
    {
      s << " " << ossia::get_pretty_unit_text(d.unit);
    }
  }
  void operator()(const Tuple& t) const
  {
    s << "tuple:";
    getTupleAsString(t, s);
  }
  void operator()() const
  {
    s << "invalid";
  }
};

static void getTupleAsString(const ossia::Tuple& tuple, fmt::MemoryWriter& s)
{
  value_prettyprint_visitor vis{s};

  int n = tuple.size();

  s << "[";
  for (int i = 0; i < n; i++)
  {
    const auto& val = tuple[i];

    if (val.valid())
      eggs::variants::apply(vis, val.v);
    if (i < n - 1)
      s << ", ";
  }
  s << "]";
}
}

std::string value_to_pretty_string(const ossia::value& val)
{
  fmt::MemoryWriter s;
  val.apply(value_prettyprint_visitor{s});
  return s.str();
}


ossia::value get_value_at_index(
    const ossia::value& val,
    const ossia::destination_index& idx)
{
  if(idx.empty())
    return val;
  return val.apply(detail::destination_index_retriever{idx, idx.cbegin()});
}

bool is_numeric(const ossia::value& val)
{
  if(val.valid())
    return eggs::variants::apply(
          [] (auto& v) {
      return ossia::value_trait<std::remove_const_t<std::remove_reference_t<decltype(v)>>>::is_numeric;
    }, val.v);
  return false;
}

bool is_array(const ossia::value& val)
{
  if(val.valid())
    return eggs::variants::apply(
          [] (auto& v) {
      return ossia::value_trait<std::remove_const_t<std::remove_reference_t<decltype(v)>>>::is_array;
    }, val.v);
  return false;
}

ossia::value convert(const ossia::value& val, ossia::val_type newtype)
{
  return lift(newtype, [&] (auto t) -> ossia::value {
    using ossia_type = typename decltype(t)::ossia_type;
    return convert<ossia_type>(val);
  });
}
}
