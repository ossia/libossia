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
OSSIA_EXPORT impulse convert<impulse>(const ossia::value& val);
template
OSSIA_EXPORT int convert<int>(const ossia::value& val);
template
OSSIA_EXPORT float convert<float>(const ossia::value& val);
template
OSSIA_EXPORT double convert<double>(const ossia::value& val);
template
OSSIA_EXPORT bool convert<bool>(const ossia::value& val);
template
OSSIA_EXPORT char convert<char>(const ossia::value& val);
template
OSSIA_EXPORT std::string convert<std::string>(const ossia::value& val);
template
OSSIA_EXPORT std::vector<ossia::value> convert<std::vector<ossia::value>>(const ossia::value& val);
template
OSSIA_EXPORT std::array<float, 2> convert<std::array<float, 2>>(const ossia::value& val);
template
OSSIA_EXPORT std::array<float, 3> convert<std::array<float, 3>>(const ossia::value& val);
template
OSSIA_EXPORT std::array<float, 4> convert<std::array<float, 4>>(const ossia::value& val);


template
OSSIA_EXPORT std::array<float, 2> convert<std::array<float, 2>>(const std::vector<ossia::value>& val);
template
OSSIA_EXPORT std::array<float, 3> convert<std::array<float, 3>>(const std::vector<ossia::value>& val);
template
OSSIA_EXPORT std::array<float, 4> convert<std::array<float, 4>>(const std::vector<ossia::value>& val);

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


std::string to_pretty_string(const destination_index& index)
{
  std::string str;

  const int n = index.size();
  if(n > 0)
  {
    str += "[";
    str += boost::lexical_cast<std::string>(index[0]);

    for(int i = 1; i < n; i++)
    {
      str += ", " + boost::lexical_cast<std::string>(index[i]);
    }
    str += "]";
  }

  return str;
}

std::string to_pretty_string(const Destination& d)
{
  using namespace std::literals;
  return
      ossia::net::address_string_from_node(d.value.get()) +
      to_pretty_string(d.index)
      + (d.unit ? ("["s + ossia::get_pretty_unit_text(d.unit) + "]"s) : ":"s);
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

template <typename Comparator>
struct value_comparison_visitor2;

// Definition is afterwards
template <typename Value, typename Comparator>
struct partial_lhs_value_comparison_visitor2
{
  const Value& lhs;

  template<typename T>
  bool operator()(const T& rhs);
};

template <typename Value, typename Comparator>
struct partial_rhs_value_comparison_visitor2
{
  const Value& rhs;
  template<typename T>
  bool operator()(const T& lhs);
};

template <typename Comparator>
struct value_comparison_visitor2
{
  template <typename T, typename U>
  bool operator()(const T& lhs, const U& rhs) const
  {
    return Comparator{}(lhs, rhs);
  }

  template <typename T>
  bool operator()(const T& lhs, const T& rhs) const
  {
    return Comparator{}(lhs, rhs);
  }

  // String - *
  bool operator()(const std::string& lhs, int32_t v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const std::string& lhs, float v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const std::string& lhs, bool v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const std::string& lhs, char v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }

  bool operator()(const std::string& lhs, const vec2f& v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const std::string& lhs, const vec3f& v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const std::string& lhs, const vec4f& v) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }


  // * - String
  bool operator()(int32_t v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(float v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(bool v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(char v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }

  bool operator()(const vec2f& v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const vec3f& v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }
  bool operator()(const vec4f& v, const std::string& lhs) const
  {
    return Comparator{}(v, comparisons::String_T{});
  }

  // Destination
  template<typename T>
  bool operator()(const T& lhs, const Destination& d) const
  {
    return Comparator{}(lhs, d.address().value(d.index));
  }

  template<typename T>
  bool operator()(const Destination& d, const T& rhs) const
  {
    return Comparator{}(d.address().value(d.index), rhs);
  }

  bool operator()(const impulse& lhs, const Destination& d) const
  {
    return Comparator{}(impulse{}, impulse{});
  }

  bool operator()(const Destination& d, const impulse& rhs) const
  {
    return Comparator{}(impulse{}, impulse{});
  }

  bool operator()(const Destination& lhs, const Destination& d) const
  {
    return Comparator{}(lhs.address().value(d.index), d.address().value(d.index));
  }

  bool operator()(const std::vector<ossia::value>& lhs, const Destination& d) const
  {
    return Comparator{}(lhs, d.address().value(d.index));
  }

  bool operator()(const Destination& d, const std::vector<ossia::value>& rhs) const
  {
    return Comparator{}(d.address().value(d.index), rhs);
  }

  bool operator()(const std::array<float, 2>& lhs, const Destination& d) const
  { return Comparator{}(lhs, d.address().value(d.index)); }

  bool operator()(const Destination& d, const std::array<float, 2>& rhs) const
  { return Comparator{}(d.address().value(d.index), rhs); }

  bool operator()(const std::array<float, 3>& lhs, const Destination& d) const
  { return Comparator{}(lhs, d.address().value(d.index)); }

  bool operator()(const Destination& d, const std::array<float, 3>& rhs) const
  { return Comparator{}(d.address().value(d.index), rhs); }

  bool operator()(const std::array<float, 4>& lhs, const Destination& d) const
  { return Comparator{}(lhs, d.address().value(d.index)); }

  bool operator()(const Destination& d, const std::array<float, 4>& rhs) const
  { return Comparator{}(d.address().value(d.index), rhs); }




  // Tuple
  template<typename T>
  bool operator()(const T& lhs, const std::vector<ossia::value>& v) const
  {
    // Note : v.size == 1 only makes sense if comparator is ==...
    return (v.size() == 1) && v[0].v && (
          eggs::variants::apply(
            partial_lhs_value_comparison_visitor2<T, Comparator>{lhs},
            v[0].v));
  }

  template<typename T>
  bool operator()(const std::vector<ossia::value>& v, const T& rhs) const
  {
    // Note : v.size == 1 only makes sense if comparator is ==...
    return (v.size() == 1) && v[0].v && (
          eggs::variants::apply(
            partial_rhs_value_comparison_visitor2<T, Comparator>{rhs},
            v[0].v));
  }

  bool operator()(const impulse& lhs, const std::vector<ossia::value>& v) const
  {
    return Comparator{}(v, lhs);
  }
  bool operator()(const std::vector<ossia::value>& lhs, const impulse& v) const
  {
    return Comparator{}(v, lhs);
  }

  bool operator()(const std::vector<ossia::value>& lhs, const std::vector<ossia::value>& rhs) const
  {
    if (lhs.size() != rhs.size())
      return false;

    bool result = true;
    auto tit = rhs.begin();
    for (const auto& val : lhs)
    {
      if(val.valid() && tit->valid())
      {
        result &= eggs::variants::apply(*this, val.v, tit->v);
      }
      else // TODO handle case where !val && !tit
      {
        result = false;
        break;
      }
      if (!result)
        break;
      tit++;
    }

    return result;
  }


  template<std::size_t N>
  bool operator()(const std::vector<ossia::value>& lhs, const std::array<float, N>& v) const
  {
    // TODO
    return false;
  }

  template<std::size_t N>
  bool operator()(const std::array<float, N>& v, const std::vector<ossia::value>& rhs) const
  {
    // TODO
    return false;
  }

  template<typename T, std::size_t N>
  bool operator()(const T& lhs, const std::array<float, N>& v) const
  {
    return false;
  }

  template<typename T, std::size_t N>
  bool operator()(const std::array<float, N>& v, const T& rhs) const
  {
    return false;
  }

  template<std::size_t N>
  bool operator()(const std::array<float, N>& lhs, const std::array<float, N>& rhs) const
  {
    bool b = true;
    for(std::size_t i = 0; i < N; i++)
      b &= Comparator{}(lhs[i], rhs[i]);
    return b;
  }

  template<std::size_t N, std::size_t M>
  bool operator()(const std::array<float, N>& lhs, const std::array<float, M>& rhs) const
  {
    return false;
  }

};

  template<typename Value, typename Comparator> template<typename T>
  bool partial_lhs_value_comparison_visitor2<Value, Comparator>::operator()(const T& rhs)
  {
    return value_comparison_visitor2<Comparator>{}(lhs, rhs);
  }

  template<typename Value, typename Comparator> template<typename T>
  bool partial_rhs_value_comparison_visitor2<Value, Comparator>::operator()(const T& lhs)
  {
    return value_comparison_visitor2<Comparator>{}(lhs, rhs);
  }

bool operator==(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor2<std::equal_to<>>{}, lhs.v, rhs.v);
  }
  else if(!lhs.v && !rhs.v)
  {
    return true;
  }
  return false;
}

bool operator!=(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return !eggs::variants::apply(
        value_comparison_visitor2<std::equal_to<>>{}, lhs.v, rhs.v);
  }
  else if(!lhs.v && !rhs.v)
  {
    return false;
  }
  return true;
}

bool operator>(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor2<std::greater<>>{}, lhs.v, rhs.v);
  }
  else
  {
    return false;
  }
}

bool operator>=(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor2<std::greater_equal<>>{}, lhs.v, rhs.v);
  }
  else if(!lhs.v && !rhs.v)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool operator<(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return eggs::variants::apply(value_comparison_visitor2<std::less<>>{}, lhs.v, rhs.v);;
  }
  else
  {
    return false;
  }
}

bool operator<=(const value& lhs, const value& rhs)
{
  if(lhs.v && rhs.v)
  {
    return eggs::variants::apply(
          value_comparison_visitor2<std::less_equal<>>{}, lhs.v, rhs.v);
  }
  else if(!lhs.v && !rhs.v)
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
template<typename Vis>
struct value_prettyprint_visitor
{
  Vis& s;

  void operator()(impulse) const
  {
    s << "impulse";
  }
  void operator()(int32_t i) const
  {
    s << "int: " << i;
  }
  void operator()(float f) const
  {
    s << "float: " << f;
  }
  void operator()(bool b) const
  {
    s << "bool: " << b;
  }
  void operator()(char c) const
  {
    s << "char: " << c;
  }
  void operator()(const std::string& str) const
  {
    s << "string: " << str;
  }
  void operator()(vec2f vec) const
  {
    s.write("vec2f: {}", vec);
  }
  void operator()(vec3f vec) const
  {
    s.write("vec3f: {}", vec);
  }
  void operator()(vec4f vec) const
  {
    s.write("vec4f: {}", vec);
  }
  void operator()(const Destination& d) const
  {
    s << "destination" << ossia::net::address_string_from_node(d.value);
    if(d.unit)
    {
      s << " " << ossia::get_pretty_unit_text(d.unit);
    }
  }
  void operator()(const std::vector<ossia::value>& t) const
  {
    s.write("tuple: {}", t);
  }
  void operator()() const
  {
    s << "invalid";
  }
};

}

std::string value_to_pretty_string(const ossia::value& val)
{
  fmt::MemoryWriter s;
  val.apply(value_prettyprint_visitor<fmt::MemoryWriter>{s});
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

namespace detail
{
struct is_numeric_helper
{
  template<typename T>
  bool operator()(const T&)
  {
    return ossia::value_trait<T>::is_numeric;
  }
};

struct is_array_helper
{
  template<typename T>
  bool operator()(const T&)
  {
    return ossia::value_trait<T>::is_array;
  }
};
}

bool is_numeric(const ossia::value& val)
{
  if(val.valid())
    return eggs::variants::apply(detail::is_numeric_helper{}, val.v);
  return false;
}

bool is_array(const ossia::value& val)
{
  if(val.valid())
    return eggs::variants::apply(detail::is_array_helper{}, val.v);
  return false;
}

ossia::value convert(const ossia::value& val, ossia::val_type newtype)
{
  return lift(newtype, [&] (auto t) -> ossia::value {
    using ossia_type = typename decltype(t)::ossia_type;
    return convert<ossia_type>(val);
  });
}

value::~value() noexcept
{

}

}

namespace std {
std::ostream& operator<<(
    std::ostream& s,
    const std::vector<ossia::value>& tuple)
{

  const int n = tuple.size();

  s << "[";
  for (int i = 0; i < n; i++)
  {
    const auto& val = tuple[i];

    if (val.valid())
    {
      fmt::MemoryWriter w;
      ossia::value_prettyprint_visitor<fmt::MemoryWriter> vis{w};
      eggs::variants::apply(vis, val.v);
      s << w.str();
    }
    if (i < n - 1)
    {
      s << ", ";
    }
  }
  s << "]";
  return s;
}

std::ostream& operator<<(
    std::ostream& s,
    const std::array<float, 2>& vec)
{
  s << "[" << vec[0] << " " << vec[1] << "]";
  return s;
}

std::ostream& operator<<(
    std::ostream& s,
    const std::array<float, 3>& vec)
{
  s << "[" << vec[0] << " " << vec[1] << " " << vec[2] << "]";
  return s;
}

std::ostream& operator<<(
    std::ostream& s,
    const std::array<float, 4>& vec)
{
  s << "[" << vec[0] << " " << vec[1] << " " << vec[2] << " " << vec[3] << "]";
  return s;
}

std::ostream& operator<<(std::ostream& s, const std::vector<std::string>& tuple)
{
  const int n = tuple.size();

  s << "[";
  if(!tuple.empty())
  {
    s << tuple[0];
    for (int i = 1; i < n; i++)
    {
      s << ", " << tuple[i];
    }
  }
  s << "]";

  return s;
}
}
