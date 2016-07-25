#include "Editor/Curve.h"
#include <Editor/Value/Value.h>
#include <Editor/Value/Value.h>
#include "Network/Node.h"
#include <Editor/Value/ValueComparisonImpl.h>
#include <Editor/Value/Behavior.h>
#include <sstream>

using namespace OSSIA;


bool Bool::operator== (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Bool::operator!= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::not_equal_to<>{}); }

bool Bool::operator> (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Bool::operator>= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Bool::operator< (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Bool::operator<= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }



bool Int::operator== (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Int::operator!= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::not_equal_to<>{}); }

bool Int::operator> (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Int::operator>= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Int::operator< (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Int::operator<= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }



bool Float::operator== (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Float::operator!= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::not_equal_to<>{}); }

bool Float::operator> (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Float::operator>= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Float::operator< (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Float::operator<= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }



bool Char::operator== (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::equal_to<>{}); }

bool Char::operator!= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::not_equal_to<>{}); }

bool Char::operator> (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater<>{}); }

bool Char::operator>= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::greater_equal<>{}); }

bool Char::operator< (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less<>{}); }

bool Char::operator<= (const Value& v) const
{ return Comparisons::NumericValue::apply(*this, v, std::less_equal<>{}); }




bool String::operator== (const Value& v) const
{ return Comparisons::StringValue::apply(*this, v, std::equal_to<>{}); }

bool String::operator!= (const Value& v) const
{ return Comparisons::StringValue::apply(*this, v, std::not_equal_to<>{}); }

bool String::operator> (const Value& v) const
{ return Comparisons::StringValue::apply(*this, v, std::greater<>{}); }

bool String::operator>= (const Value& v) const
{ return Comparisons::StringValue::apply(*this, v, std::greater_equal<>{}); }

bool String::operator< (const Value& v) const
{ return Comparisons::StringValue::apply(*this, v, std::less<>{}); }

bool String::operator<= (const Value& v) const
{ return Comparisons::StringValue::apply(*this, v, std::less_equal<>{}); }



Tuple::Tuple() = default;
Tuple::Tuple(std::initializer_list<Value> v):
  value(v)
{
}

Tuple::Tuple(const std::vector<Value>& v):
  value(v)
{
}

Tuple::Tuple(std::vector<Value>&& v):
  value(std::move(v))
{
}

Tuple::Tuple(const Value& v)
{
  value.push_back(v);
}


bool Tuple::operator== (const Value& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::equal_to<>{}); }

bool Tuple::operator!= (const Value& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::not_equal_to<>{}); }

bool Tuple::operator> (const Value& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::greater<>{}); }

bool Tuple::operator>= (const Value& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::greater_equal<>{}); }

bool Tuple::operator< (const Value& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::less<>{}); }

bool Tuple::operator<= (const Value& v) const
{ return Comparisons::TupleValue::apply(*this, v, std::less_equal<>{}); }


Destination::Destination() = default;

Destination::Destination(const Destination& other) = default;
Destination::Destination(Destination&& other) = default;

Destination& Destination::operator=(const Destination&) = default;
Destination& Destination::operator=(Destination&&) = default;

Destination::Destination(std::shared_ptr<Node> v):
    value(v)
{
}

Destination::Destination(
        std::shared_ptr<Node> v,
        DestinationIndex idx) :
    value(std::move(v)),
    index(std::move(idx))
{
}




bool Destination::operator== (const Value& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::equal_to<>{}); }

bool Destination::operator!= (const Value& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::not_equal_to<>{}); }

bool Destination::operator> (const Value& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::greater<>{}); }

bool Destination::operator>= (const Value& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::greater_equal<>{}); }

bool Destination::operator< (const Value& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::less<>{}); }

bool Destination::operator<= (const Value& v) const
{ return Comparisons::DestinationValue::apply(*this, v, std::less_equal<>{}); }


bool Value::operator==(const Value &rhs) const
{ return v == rhs.v; }

bool Value::operator!=(const Value &rhs) const
{ return v != rhs.v; }

bool Value::operator>(const Value &rhs) const
{ return v > rhs.v; }

bool Value::operator>=(const Value &rhs) const
{ return v >= rhs.v; }

bool Value::operator<(const Value &rhs) const
{ return v < rhs.v; }

bool Value::operator<=(const Value &rhs) const
{ return v <= rhs.v; }


namespace {

static std::string getTupleAsString(const OSSIA::Tuple& tuple);
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

static std::string getTupleAsString(const OSSIA::Tuple& tuple)
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

namespace OSSIA
{

std::string getValueAsString(const OSSIA::Value& val)
{
    std::stringstream s;
    ValueStringVisitor vis{s};
    if(val.valid())
      eggs::variants::apply(vis, val.v);

    return s.str();
}
}
