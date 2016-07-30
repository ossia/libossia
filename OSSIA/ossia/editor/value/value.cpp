#include <ossia/editor/curve/curve.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_comparison.hpp>
#include <ossia/network/base/node.hpp>
#include <sstream>

namespace ossia
{
bool Bool::operator== (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Bool::operator!= (const ossia::value& v) const
{ return !comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Bool::operator> (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Bool::operator>= (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Bool::operator< (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Bool::operator<= (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }



bool Int::operator== (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Int::operator!= (const ossia::value& v) const
{ return !comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Int::operator> (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Int::operator>= (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Int::operator< (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Int::operator<= (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }



bool Float::operator== (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Float::operator!= (const ossia::value& v) const
{ return !comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Float::operator> (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Float::operator>= (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Float::operator< (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Float::operator<= (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }



bool Char::operator== (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Char::operator!= (const ossia::value& v) const
{ return !comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Char::operator> (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Char::operator>= (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Char::operator< (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Char::operator<= (const ossia::value& v) const
{ return comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }




bool String::operator== (const ossia::value& v) const
{ return comparisons::StringValue::apply(*this, v, std::equal_to<>{}); }

bool String::operator!= (const ossia::value& v) const
{ return !comparisons::StringValue::apply(*this, v, std::equal_to<>{}); }

bool String::operator> (const ossia::value& v) const
{ return comparisons::StringValue::apply(*this, v, std::greater<>{}); }

bool String::operator>= (const ossia::value& v) const
{ return comparisons::StringValue::apply(*this, v, std::greater_equal<>{}); }

bool String::operator< (const ossia::value& v) const
{ return comparisons::StringValue::apply(*this, v, std::less<>{}); }

bool String::operator<= (const ossia::value& v) const
{ return comparisons::StringValue::apply(*this, v, std::less_equal<>{}); }



Tuple::Tuple() = default;
Tuple::Tuple(std::initializer_list<ossia::value> v):
  value(v)
{
}

Tuple::Tuple(const std::vector<ossia::value>& v):
  value(v)
{
}

Tuple::Tuple(std::vector<ossia::value>&& v):
  value(std::move(v))
{
}

Tuple::Tuple(const ossia::value& v)
{
  value.push_back(v);
}


bool Tuple::operator== (const ossia::value& v) const
{ return comparisons::TupleValue::apply(*this, v, std::equal_to<>{}); }

bool Tuple::operator!= (const ossia::value& v) const
{ return !comparisons::TupleValue::apply(*this, v, std::equal_to<>{}); }

bool Tuple::operator> (const ossia::value& v) const
{ return comparisons::TupleValue::apply(*this, v, std::greater<>{}); }

bool Tuple::operator>= (const ossia::value& v) const
{ return comparisons::TupleValue::apply(*this, v, std::greater_equal<>{}); }

bool Tuple::operator< (const ossia::value& v) const
{ return comparisons::TupleValue::apply(*this, v, std::less<>{}); }

bool Tuple::operator<= (const ossia::value& v) const
{ return comparisons::TupleValue::apply(*this, v, std::less_equal<>{}); }


Destination::Destination() = default;

Destination::Destination(const Destination& other) = default;
Destination::Destination(Destination&& other) = default;

Destination& Destination::operator=(const Destination&) = default;
Destination& Destination::operator=(Destination&&) = default;

Destination::Destination(ossia::net::Node& v):
    value(&v)
{
}

Destination::Destination(
        ossia::net::Node& v,
        destination_index idx) :
    value(&v),
    index(std::move(idx))
{
}




bool Destination::operator== (const ossia::value& v) const
{ return comparisons::DestinationValue::apply(*this, v, std::equal_to<>{}); }

bool Destination::operator!= (const ossia::value& v) const
{ return !comparisons::DestinationValue::apply(*this, v, std::equal_to<>{}); }

bool Destination::operator> (const ossia::value& v) const
{ return comparisons::DestinationValue::apply(*this, v, std::greater<>{}); }

bool Destination::operator>= (const ossia::value& v) const
{ return comparisons::DestinationValue::apply(*this, v, std::greater_equal<>{}); }

bool Destination::operator< (const ossia::value& v) const
{ return comparisons::DestinationValue::apply(*this, v, std::less<>{}); }

bool Destination::operator<= (const ossia::value& v) const
{ return comparisons::DestinationValue::apply(*this, v, std::less_equal<>{}); }


template<typename Comparator>
struct value_comparison_visitor
{
        template<typename T, typename U>
        bool operator()(const T& lhs, const U& rhs)
        {
            return Comparator{}(lhs, rhs);
        }
};

bool value::operator==(const value &rhs) const
{
    return eggs::variants::apply(value_comparison_visitor<std::equal_to<>>{}, v, rhs.v);
}

bool value::operator!=(const value &rhs) const
{
    return eggs::variants::apply(value_comparison_visitor<std::not_equal_to<>>{}, v, rhs.v);
}

bool value::operator>(const value &rhs) const
{
    return eggs::variants::apply(value_comparison_visitor<std::greater<>>{}, v, rhs.v);
}

bool value::operator>=(const value &rhs) const
{
    return eggs::variants::apply(value_comparison_visitor<std::greater_equal<>>{}, v, rhs.v);
}

bool value::operator<(const value &rhs) const
{
    return eggs::variants::apply(value_comparison_visitor<std::less<>>{}, v, rhs.v);
}

bool value::operator<=(const value &rhs) const
{
    return eggs::variants::apply(value_comparison_visitor<std::less_equal<>>{}, v, rhs.v);
}


namespace {

static std::string getTupleAsString(const ossia::Tuple& tuple);
struct ValueStringVisitor {
  std::stringstream& s;

  void operator()(Impulse) const { s << "impulse"; }
  void operator()(Int i) const   { s << "int: " << i.value; }
  void operator()(Float f) const { s << "float: " << f.value; }
  void operator()(Bool b) const  { s << "bool: " << b.value; }
  void operator()(Char c) const  { s << "char: " << c.value; }
  void operator()(const String& str) const { s << "string: " << str.value; }
  void operator()(Vec2f vec) const { }
  void operator()(Vec3f vec) const { }
  void operator()(Vec4f vec) const { }
  void operator()(const Destination& d) const { s << "destination"; }
  void operator()(const Behavior&) const { s << "behavior"; }
  void operator()(const Tuple& t) const { s << "tuple:" << getTupleAsString(t); }
};

static std::string getTupleAsString(const ossia::Tuple& tuple)
{
  std::stringstream s;

  ValueStringVisitor vis{s};

  int n = tuple.value.size();

  s << "[";
  for(int i = 0; i < n; i++)
  {
    const auto& val = tuple.value[i];

    if(val.valid())
      eggs::variants::apply(vis, val.v);
    if(i < n - 1)
      s << ", ";
  }
  s << "]";

  return s.str();
}

}

std::string getValueAsString(const ossia::value& val)
{
    std::stringstream s;
    ValueStringVisitor vis{s};
    if(val.valid())
      eggs::variants::apply(vis, val.v);

    return s.str();
}
}
