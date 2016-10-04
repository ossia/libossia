#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_comparison.hpp>
#include <ossia/editor/value/value_algorithms.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/detail/logger.hpp>
#include <sstream>

template class eggs::variant<
    ossia::Impulse, ossia::Bool, ossia::Int, ossia::Float, ossia::Char, ossia::String, ossia::Tuple, ossia::Vec2f,
    ossia::Vec3f, ossia::Vec4f, ossia::Destination, ossia::Behavior>;

template class std::vector<ossia::value>;
namespace ossia
{

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

Destination::Destination(const Destination& other) = default;
Destination::Destination(Destination&& other) = default;

Destination& Destination::operator=(const Destination&) = default;
Destination& Destination::operator=(Destination&&) = default;

Destination::Destination(ossia::net::address_base& v) : value(v)
{
}

Destination::Destination(ossia::net::address_base& v, destination_index idx)
    : value(v), index(std::move(idx))
{
}

bool Destination::operator==(const ossia::value& v) const
{
  return comparisons::DestinationValue::apply(*this, v, std::equal_to<>{});
}

bool Destination::operator!=(const ossia::value& v) const
{
  return !comparisons::DestinationValue::apply(*this, v, std::equal_to<>{});
}

bool Destination::operator>(const ossia::value& v) const
{
  return comparisons::DestinationValue::apply(*this, v, std::greater<>{});
}

bool Destination::operator>=(const ossia::value& v) const
{
  return comparisons::DestinationValue::apply(
      *this, v, std::greater_equal<>{});
}

bool Destination::operator<(const ossia::value& v) const
{
  return comparisons::DestinationValue::apply(*this, v, std::less<>{});
}

bool Destination::operator<=(const ossia::value& v) const
{
  return comparisons::DestinationValue::apply(*this, v, std::less_equal<>{});
}

bool operator==(const Destination& lhs, const Destination& rhs)
{
  return lhs.value == rhs.value && lhs.index == rhs.index;
}

bool operator!=(const Destination& lhs, const Destination& rhs)
{
  return lhs.value != rhs.value || lhs.index != rhs.index;
}

bool operator==(const Destination& lhs, const ossia::net::address_base& rhs)
{
  return lhs.value == rhs && lhs.index.empty();
}

bool operator!=(const Destination& lhs, const ossia::net::address_base& rhs)
{
  return lhs.value != rhs || !lhs.index.empty();
}



template <typename Comparator>
struct value_comparison_visitor
{
  const value& rhs;
  template <typename T>
  bool operator()(const T& lhs)
  {
    return Comparator{}(lhs, rhs);
  }
};

bool value::operator==(const value& rhs) const
{
  return eggs::variants::apply(
      value_comparison_visitor<std::equal_to<>>{rhs}, v);
}

bool value::operator!=(const value& rhs) const
{
  return eggs::variants::apply(
      value_comparison_visitor<std::not_equal_to<>>{rhs}, v);
}

bool value::operator>(const value& rhs) const
{
  return eggs::variants::apply(
      value_comparison_visitor<std::greater<>>{rhs}, v);
}

bool value::operator>=(const value& rhs) const
{
  return eggs::variants::apply(
      value_comparison_visitor<std::greater_equal<>>{rhs}, v);
}

bool value::operator<(const value& rhs) const
{
  return eggs::variants::apply(value_comparison_visitor<std::less<>>{rhs}, v);
}

bool value::operator<=(const value& rhs) const
{
  return eggs::variants::apply(
      value_comparison_visitor<std::less_equal<>>{rhs}, v);
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
    s << "int: " << i.value;
  }
  void operator()(Float f) const
  {
    s << "float: " << f.value;
  }
  void operator()(Bool b) const
  {
    s << "bool: " << b.value;
  }
  void operator()(Char c) const
  {
    s << "char: " << c.value;
  }
  void operator()(const String& str) const
  {
    s << "string: " << str.value;
  }
  void operator()(Vec2f vec) const
  {
    s << "vec2f: [" << vec.value[0] << " " << vec.value[1] << "]";
  }
  void operator()(Vec3f vec) const
  {
    s << "vec3f: [" << vec.value[0] << " " << vec.value[1] << " " << vec.value[2] << "]";
  }
  void operator()(Vec4f vec) const
  {
    s << "vec4f: [" << vec.value[0] << " " << vec.value[1] << " " << vec.value[2] << " " << vec.value[3] << "]";
  }
  void operator()(const Destination& d) const
  {
    s << "destination" << ossia::net::address_string_from_node(d.value);
    // TODO pretty print index
  }
  void operator()(const Behavior&) const
  {
    s << "behavior";
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

  int n = tuple.value.size();

  s << "[";
  for (int i = 0; i < n; i++)
  {
    const auto& val = tuple.value[i];

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

}
